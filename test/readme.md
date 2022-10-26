# Test
## Setup (Linux)
* SLD
    * `sudo apt-get install libsdl2-dev`
* LuaJIT
    * `git clone https://github.com/LuaJIT/LuaJIT.git`
    * `cd LuaJIT`
    * `make`
    * `sudo make install`
* GameNetworkingSockets
    * `sudo apt install libssl-dev`
    * `sudo apt install libprotobuf-dev protobuf-compiler`
    * `sudo apt install ninja-build`
    * `git clone https://github.com/ValveSoftware/GameNetworkingSockets.git`
    * `cd GameNetworkingSockets`
    * `mkdir build`
    * `cd build`
    * `cmake -G Ninja ..`
    * `ninja`
* Bullet3
    * `git clone https://github.com/bulletphysics/bullet3`
    * `cd bullet3`
    * `mkdir build`
    * `cd build`
    * `cmake -D BUILD_BULLET2_DEMOS=OFF -D BUILD_CLSOCKET=OFF -D BUILD_CPU_DEMOS=OFF -D BUILD_EGL=OFF -D BUILD_ENET=OFF -D BUILD_EXTRAS=OFF -D BUILD_OPENGL3_DEMOS=OFF -D BUILD_UNIT_TESTS=OFF -D INSTALL_EXTRA_LIBS=OFF -D USE_GLUT=OFF -D USE_GRAPHICAL_BENCHMARK=OFF ..`
    * `make`
## Usage (Linux)
    git clone --recursive <repository_url>
    cd <repository_name>
    chmod +x ./build.sh
    ./build.sh

## Libraries
* https://github.com/libsdl-org/SDL
* https://github.com/Dav1dde/glad
* https://github.com/nothings/stb
* https://github.com/simdjson/simdjson
* https://github.com/g-truc/glm
* https://github.com/ThePhD/sol2
* https://github.com/LuaJIT/LuaJIT
* https://github.com/ocornut/imgui
* https://github.com/CedricGuillemet/ImGuizmo
* https://github.com/Nelarius/imnodes
* https://github.com/epezent/implot
* https://github.com/skypjack/entt
* https://github.com/bulletphysics/bullet3
* https://github.com/ValveSoftware/GameNetworkingSockets

## Resources
* https://github.com/TheAlgorithms/C-Plus-Plus

## For getting latest simdjson
* wget https://raw.githubusercontent.com/simdjson/simdjson/master/singleheader/simdjson.h
* wget https://raw.githubusercontent.com/simdjson/simdjson/master/singleheader/simdjson.cpp

## To restore submodules
* git submodule add https://github.com/g-truc/glm.git libraries/glm
* git submodule add https://github.com/ThePhD/sol2.git libraries/sol2
* git submodule add https://github.com/ocornut/imgui.git libraries/imgui/imgui
* git submodule add https://github.com/CedricGuillemet/ImGuizmo.git libraries/imgui/imguizmo
* git submodule add https://github.com/Nelarius/imnodes.git libraries/imgui/imnodes
* git submodule add https://github.com/epezent/implot.git libraries/imgui/implot
* git submodule add https://github.com/strombergs-denniss/imfiledialog libraries/imgui/imfiledialog
* git submodule add https://github.com/skypjack/entt libraries/entt
