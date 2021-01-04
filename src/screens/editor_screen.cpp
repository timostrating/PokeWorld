//
// Created by sneeuwpop on 18-11-19.
//

# define IMGUI_DEFINE_MATH_OPERATORS
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <GLFW/glfw3.h>
#include <sstream>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "../util/interfaces/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/flying_camera.h"
#include "../util/input/keyboard.h"
#include "../util/splines/line.h"
#include "../graphics/frame_buffer.h"

using namespace glm;
using namespace MATH;

#include <imgui_internal.h>
#include <imgui_node_editor_internal.h>
#include <imgui_extra_math.h>
# include "../util/external/imgui_node_editor.hpp"
namespace ed = ax::NodeEditor;

class EditorNode;
struct EditorLink
{
    ed::LinkId id;
    ed::PinId  inputId;
    ed::PinId  outputId;
};


static std::vector<EditorNode*> s_pin2Node;
static int _nextId = 0;
static int getId(EditorNode *node)
{
    s_pin2Node.emplace_back(node);
    return _nextId++;
}

static ed::EditorContext* g_Context = nullptr;
static ImVector<EditorNode*>    s_Nodes;
static ImVector<EditorLink>    s_Links;


struct EditorPin
{
    ed::PinId id;

    EditorPin(int id): id(id)
    {
    }
};



class EditorNode
{
public:
    ed::NodeId id;

    std::vector<EditorNode*> inputs;
    std::vector<EditorPin> pinsIn;
    std::vector<EditorPin> pinsOut;

    EditorNode()
    {
    }

    virtual void preDefineFunction(std::stringstream *stream) {};
    virtual void node2code(std::stringstream *stream) {};
    virtual bool validCode() { return true; };
    virtual void renderNode() {};
};

static const char* MathNodeOptions[] = { "sin", "cos", "tan", "min", "max" };
class MathNode : public EditorNode
{
public:
    int selected_option_id = 0;

    MathNode()
    {
        inputs.emplace_back(nullptr);
        id = getId(this);
        pinsIn.emplace_back(getId(this));
        pinsOut.emplace_back(getId(this));
    }

    void node2code(std::stringstream *stream) override
    {
        (*stream) << MathNodeOptions[selected_option_id] << "(";
        if (inputs.at(0)) inputs.at(0)->node2code(stream);
        (*stream) <<")";
    }

    bool validCode()
    {
        if (inputs.at(0))
            return inputs.at(0)->validCode();
        return false;
    }

    void renderNode() override
    {
        ImGui::PushItemWidth(200);
        ed::BeginNode(id);
        ImGui::Text("Math");

        ImGui::Combo("", &selected_option_id, MathNodeOptions, IM_ARRAYSIZE(MathNodeOptions));

        for (EditorPin& input : pinsIn)
        {
            ed::BeginPin(input.id, ed::PinKind::Input);
                ImGui::Text("-> In");
            ed::EndPin();
        }
        ImGui::SameLine(150);
        for (EditorPin& output : pinsOut)
        {
            ed::BeginPin(output.id, ed::PinKind::Output);
                ImGui::Text("Out ->");
            ed::EndPin();
        }
        ed::EndNode();
    }
};

class InputColorNode : public EditorNode
{
public:
    ImVec4 color = ImVec4();

    InputColorNode()
    {
        inputs.emplace_back(nullptr);
        id = getId(this);
        pinsOut.emplace_back(getId(this));
    }

    void node2code(std::stringstream *stream) override
    {
        (*stream) <<"vec4("<< color.x <<","<< color.y <<","<<color.z <<","<< color.w <<")";
    }

    void renderNode() override
    {
        ImGui::PushItemWidth(200);
        ed::BeginNode(id);
        ImGui::Text("Color input");

        ImGui::ColorPicker4("", (float*)&color);

        ImGui::Spacing();
        ImGui::SameLine(180);
        for (EditorPin& output : pinsOut)
        {
            ed::BeginPin(output.id, ed::PinKind::Output);
            ImGui::Text("Out ->");
            ed::EndPin();
        }
        ed::EndNode();
    }
};

class OutputNode : public EditorNode
{
public:
    OutputNode()
    {
        inputs.emplace_back(nullptr);
        id = getId(this);
        pinsIn.emplace_back(getId(this));
    }

    void node2code(std::stringstream *stream) override
    {
        (*stream) << "outputColor = ";
        if (inputs.at(0)) inputs.at(0)->node2code(stream);
        (*stream) <<";";
    }

    bool validCode() override
    {
        return inputs.at(0);
    }

    void renderNode() override
    {
        ImGui::PushItemWidth(200);
        ed::BeginNode(id);
            ImGui::Text("Output");
            ImGui::Text("Waarom.......");

            ed::BeginPin(pinsIn.front().id, ed::PinKind::Input);
                ImGui::Text("-> In");
            ed::EndPin();
        ed::EndNode();
    }
};


static OutputNode outputNode = OutputNode();



static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

class EditorScreen : public Screen
{

    FlyingCamera camera = FlyingCamera();

    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/lib/default.vert", "shaders/lib/default.frag");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());
    SharedMesh sphere = SharedMesh(Mesh::sphere());

    GLint MVP;

public:
    EditorScreen()
    {
        flatShader.use();
        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(quad);
        VertexBuffer::uploadSingleMesh(sphere);
    }


    void setup(GLFWwindow* window)
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
        // TODO: Fill optional fields of the io structure later.
        // TODO: Load TTF/OTF fonts if you don't want to use the default font.

        const char* glsl_version = "#version 300 es"; // TODO: We should not hardcoded that here

        // Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32 and imgui_impl_dx11)
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        g_Context = ed::CreateEditor();

        s_Nodes.push_back(&outputNode);
    }

    double time = 0;
    bool anyKeyPressed = false;
    float colorData[4] = {0.0f, 0.0f, 0.0f, 0.0f};


    void render(double deltaTime) {
        time += deltaTime;
        flatShader.use();
        glUniform1f(flatShader.uniformLocation("u_time"), time);

        glClearColor(0/255.0, 0/255.0, 0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(5, 5, 5);
            camera.lookAt(vec3(0.0, -0.2, 0.0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        flatShader.use();
        glUniformMatrix4fv(flatShader.uniformLocation("MVP"), 1, GL_FALSE, &(rotate(glm::scale(Camera::main->combined, vec3(2, 2, 2)), radians(static_cast<float>(time) * 15.0f), VEC3::Y))[0][0]);
        if (!s_Links.empty())
            glUniform4fv(flatShader.uniformLocation("u_color"), 1, colorData);
        sphere->render();


        //////////////////////////////////////////////////////////////////////////////////////////////////////////// GUI

        // https://github.com/thedmd/imgui-node-editor

        flatShader.use();
        VertexBuffer::bindDefault();

        // Feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                   = ImVec4(0.92f, 0.90f, 0.85f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.04f, 0.04f, 0.04f, 0.94f);
        colors[ImGuiCol_Border]                 = ImVec4(1.00f, 1.00f, 1.00f, 0.16f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.29f, 0.29f, 0.29f, 0.66f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(1.00f, 1.00f, 1.00f, 0.40f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.98f, 0.26f, 0.26f, 0.67f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.43f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.49f, 0.09f, 0.09f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.98f, 0.26f, 0.26f, 0.31f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.04f, 0.04f, 0.04f, 0.94f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(1.00f, 1.00f, 1.00f, 0.37f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(1.00f, 1.00f, 1.00f, 0.75f);
        colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 1.00f, 1.00f, 0.38f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.00f, 1.00f, 1.00f, 0.66f);
        colors[ImGuiCol_Button]                 = ImVec4(0.98f, 0.26f, 0.26f, 0.31f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(1.00f, 0.11f, 0.11f, 0.76f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.98f, 0.06f, 0.06f, 0.85f);
        colors[ImGuiCol_Header]                 = ImVec4(0.98f, 0.26f, 0.26f, 0.31f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.98f, 0.26f, 0.26f, 0.64f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.98f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.75f, 0.10f, 0.10f, 0.78f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.75f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.98f, 0.26f, 0.26f, 0.20f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.98f, 0.26f, 0.26f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.98f, 0.26f, 0.26f, 0.95f);
        colors[ImGuiCol_Tab]                    = ImVec4(1.00f, 0.03f, 0.03f, 0.12f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.98f, 0.26f, 0.26f, 0.49f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.98f, 0.26f, 0.26f, 0.31f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.15f, 0.07f, 0.07f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.42f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.98f, 0.26f, 0.26f, 0.35f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);



        auto& io = ImGui::GetIO();

        ed::SetCurrentEditor(g_Context);

        ImGui::SetWindowSize(ImVec2(200, 200));
        ImGui::Begin("Example Output");
            static std::stringstream shaderStream;
            static ImGuiTextBuffer shaderTextBuffer;
            static int magic = 0;
            ImGui::TextUnformatted(shaderTextBuffer.begin(), shaderTextBuffer.end());
        ImGui::End();


        ImGui::SetWindowSize(ImVec2(520, 600));
        ImGui::Begin("Shader Editor");
        ImGui::BeginChild("Settings", ImVec2(100, ImGui::GetWindowHeight()));

        if (ImGui::Button("generate"))
        {
            if (outputNode.validCode())
            {
                shaderTextBuffer.clear();
                shaderStream.str("");
                outputNode.node2code(&shaderStream);
                std::cout << shaderStream.str().c_str() << "\n";
                shaderTextBuffer.append(shaderStream.str().c_str());
            }
        }

        if (ImGui::Button("Add Math Node"))
            s_Nodes.push_back(new MathNode());

        if (ImGui::Button("Add Color Node"))
            s_Nodes.push_back(new InputColorNode());


        ImGui::EndChild();

        ImGui::SameLine(120.0f);

        ImGui::BeginChild("The editor");
        ed::Begin("editor", ImVec2(0.0, 0.0f));

        // Render node
        for (auto& node : s_Nodes)
            node->renderNode();

        // Submit Links
        for (EditorLink& link : s_Links)
            ed::Link(link.id, link.inputId, link.outputId);


        //
        // 2) Handle interactions
        //

        // Handle creation action, returns true if editor want to create new object (node or link)
        if (ed::BeginCreate())
        {
            ed::PinId inputPinId, outputPinId;
            if (ed::QueryNewLink(&inputPinId, &outputPinId))
            {
                // QueryNewLink returns true if editor want to create new link between pins.
                //
                // Link can be created only for two valid pins, it is up to you to
                // validate if connection make sense. Editor is happy to make any.
                //
                // Link always goes from input to output. User may choose to drag
                // link from output pin or input pin. This determine which pin ids
                // are valid and which are not:
                //   * input valid, output invalid - user started to drag new ling from input pin
                //   * input invalid, output valid - user started to drag new ling from output pin
                //   * input valid, output valid   - user dragged link over other pin, can be validated

                if (inputPinId && outputPinId) // both are valid, let's accept link
                {
                    // ed::AcceptNewItem() return true when user release mouse button.
                    if (ed::AcceptNewItem())
                    {
                        std::cout << "Lijntje\n"; // TODO: These 2 can be rotated around
                        EditorNode* in = s_pin2Node.at(inputPinId.Get());
                        EditorNode* out = s_pin2Node.at(outputPinId.Get());

                        if (in && out) {
                            int i = 0;
                            bool foundPin = false;
                            for (EditorPin& pin : in->pinsIn)
                            {
                                if (pin.id.Get() == inputPinId.Get())
                                {
                                    std::cout << "connected\n";
                                    in->inputs[i] = out;
                                    foundPin = true;
                                }
                                i++;
                            }

                            if (!foundPin) {
                                i = 0;
                                for (EditorPin& pin : out->pinsIn)
                                {
                                    if (pin.id.Get() == outputPinId.Get())
                                    {
                                        std::cout << "connected\n";
                                        out->inputs[i] = in;
                                    }
                                    i++;
                                }
                            }
                        }

                        // Since we accepted new link, lets add one to our list of links.
                        s_Links.push_back({ ed::LinkId(getId(nullptr)), inputPinId, outputPinId });

                        // Draw new link.
                        ed::Link(s_Links.back().id, s_Links.back().inputId, s_Links.back().outputId);
                    }

                    // You may choose to reject connection between these nodes
                    // by calling ed::RejectNewItem(). This will allow editor to give
                    // visual feedback by changing link thickness and color.
                }
            }
        }
        ed::EndCreate(); // Wraps up object creation action handling.


        // Handle deletion action
        if (ed::BeginDelete())
        {
            // There may be many links marked for deletion, let's loop over them.
            ed::LinkId deletedLinkId;
            while (ed::QueryDeletedLink(&deletedLinkId))
            {
                // If you agree that link can be deleted, accept deletion.
                if (ed::AcceptDeletedItem())
                {
                    // Then remove link from your data.
                    for (auto& link : s_Links)
                    {
                        if (link.id == deletedLinkId)
                        {
                            s_Links.erase(&link);
                            break;
                        }
                    }
                }

                // You may reject link deletion by calling:
                // ed::RejectDeletedItem();
            }
        }
        ed::EndDelete(); // Wrap up deletion action



        // End of interaction with editor.
        ed::End();
        ImGui::EndChild();
        ImGui::End();

        ed::SetCurrentEditor(nullptr);

        ImGui::ShowDemoWindow();


        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }

    // TODO: It is currently not possible to switch between the screens
    void destroy()
    {
        ed::DestroyEditor(g_Context);
    }
};