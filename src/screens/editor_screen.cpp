//
// Created by sneeuwpop on 18-11-19.
//

# define IMGUI_DEFINE_MATH_OPERATORS
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <GLFW/glfw3.h>
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

struct EditorPin {
    ed::PinId id;

    EditorPin(int id):
            id(id)
    {
    }
};

struct EditorNode
{
    ed::NodeId id;
    const char *name;

    std::vector<EditorPin> inputs;
    std::vector<EditorPin> outputs;

    EditorNode(int id, const char* name):
            id(id), name(name)
    {
    }
};

struct EditorLink
{
    ed::LinkId id;
    ed::PinId  inputId;
    ed::PinId  outputId;
};

static ed::EditorContext* g_Context = nullptr;
static ImVector<EditorNode>    s_Nodes;
static ImVector<EditorLink>    s_Links;
static int                  g_NextLinkId = 100;     // Counter to help generate link ids. In real application this will probably based on pointer to user data structure.
static bool firstTime = true;

#define MULTILINE(...) #__VA_ARGS__

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

        s_Nodes.push_back(EditorNode(0, "Output"));
        s_Nodes.back().inputs.emplace_back(EditorPin(2));
    }

    double time = 0;
    bool anyKeyPressed = false;
    bool connected = false;
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



        auto& io = ImGui::GetIO();
        if (ImGui::Button("load")) {
            std::cout<< "Print";
        }

        bool tmp = false;
        if (ImGui::BeginTable("split", 3))
        {
            ImGui::TableNextColumn(); ImGui::Checkbox("tmp", &tmp);
            ImGui::TableNextColumn(); ImGui::Checkbox("tmp", &tmp);
            ImGui::TableNextColumn(); ImGui::Checkbox("tmp", &tmp);
            ImGui::TableNextColumn(); ImGui::Checkbox("tmp", &tmp);
            ImGui::TableNextColumn(); ImGui::Checkbox("tmp", &tmp);
            ImGui::TableNextColumn(); ImGui::Checkbox("tmp", &tmp);
            ImGui::TableNextColumn(); ImGui::Checkbox("tmp", &tmp);
            ImGui::TableNextColumn(); ImGui::Checkbox("tmp", &tmp);
            ImGui::TableNextColumn(); ImGui::Checkbox("tmp", &tmp);
            ImGui::TableNextColumn(); ImGui::Checkbox("tmp", &tmp);
            ImGui::EndTable();
        }


        ed::SetCurrentEditor(g_Context);
        if (INPUT::KEYBOARD::pressed(GLFW_KEY_A)) {
            firstTime = false;
            s_Nodes.push_back(EditorNode(1, "Test Node"));
            s_Nodes.back().outputs.emplace_back(EditorPin(3));
        }



        // Start interaction with editor.
        ed::Begin("My Editor", ImVec2(0.0, 0.0f));


        ed::BeginNode(999);
            ImGui::Text("Color");

//            ImGui::ColorPicker4("color Picker", colorData);

            ImGui::Separator();

            ImGui::SameLine(ImGui::GetWindowWidth()-30);

            ImGui::BeginGroup();
                ed::BeginPin(998, ed::PinKind::Output);
                    ImGui::Text("Out ->");
                ed::EndPin();
            ImGui::EndGroup();
        ed::EndNode();


        for (EditorNode& node : s_Nodes) {

            ed::BeginNode(node.id);
                ImGui::Text(node.name);
                ImGui::Separator();

                ImGui::BeginGroup();
                    for (EditorPin& input : node.inputs) {
                        ed::BeginPin(input.id, ed::PinKind::Input);
                            ImGui::Text("-> In");
                        ed::EndPin();
                    }
                ImGui::EndGroup();
                ImGui::SameLine();
                ImGui::BeginGroup();
                    for (EditorPin& output : node.outputs) {
                        ed::BeginPin(output.id, ed::PinKind::Output);
                            ImGui::Text("Out ->");
                        ed::EndPin();
                    }
                ImGui::EndGroup();
            ed::EndNode();
        }


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
                        // Since we accepted new link, lets add one to our list of links.
                        s_Links.push_back({ ed::LinkId(g_NextLinkId++), inputPinId, outputPinId });

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
    void destroy() {
        ed::DestroyEditor(g_Context);
    }
};