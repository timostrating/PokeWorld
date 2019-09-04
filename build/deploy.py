""" 
This is a script to deploy and start the webbuild

python3 deploy.py           ;; just do it again
python3 deploy.py fresh     ;; remove cache/MakeFiles and do it again    NOTION: This may take a while
python3 deploy.py push      ;; deploy and push to github pages

"""

import os
import sys

def shell(str):
    if os.system(str) != 0:
        print("\n YOU MADE AN ERROR \n")
        os._exit(-1)



print("Git pull ...")
shell("cd gh-pages && git pull origin gh-pages")


if sys.argv.__contains__("push"):
    print("Git push ...")
    shell("cd gh-pages && git add .")
    shell("cd gh-pages && git commit -m \"deploy\"")
    shell("cd gh-pages && git push origin gh-pages")


else:
    print("Let emscripten do its thing ...")
    if sys.argv.__contains__("fresh"):  # python3 deploy.py fresh
        shell("rm -rf ./emscripten")
        shell("mkdir emscripten")
        shell("cp desktop/CMakeLists.txt emscripten/CMakeLists.txt")
        shell("cd emscripten && emconfigure cmake ./")

    shell("cd emscripten && make -j8")  # we use 8 threads


    print("Copying data ...")
    shell("cp emscripten/out/TinyPokemonWorld.html gh-pages/index.html")
    shell("cp emscripten/out/TinyPokemonWorld.js gh-pages/TinyPokemonWorld.js")
    shell("cp emscripten/out/TinyPokemonWorld.wasm gh-pages/TinyPokemonWorld.wasm")
    shell("cp emscripten/out/TinyPokemonWorld.data gh-pages/TinyPokemonWorld.data")

    shell("emrun gh-pages/index.html")



print("Done")