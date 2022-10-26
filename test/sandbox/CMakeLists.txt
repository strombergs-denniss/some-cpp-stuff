project(sandbox)

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

set(
    SOURCES
    sources/main.cpp
    sources/generator.cpp
    sources/renderer.cpp
)

set(
    IMGUI_SOURCES
    libraries/imgui/imgui/imgui_demo.cpp
    libraries/imgui/imgui/imgui_draw.cpp
    libraries/imgui/imgui/imgui_tables.cpp
    libraries/imgui/imgui/imgui_widgets.cpp
    libraries/imgui/imgui/imgui.cpp
    libraries/imgui/imgui/backends/imgui_impl_opengl3.cpp
    libraries/imgui/imgui/backends/imgui_impl_sdl.cpp
    libraries/imgui/imguizmo/GraphEditor.cpp
    libraries/imgui/imguizmo/ImCurveEdit.cpp
    libraries/imgui/imguizmo/ImGradient.cpp
    libraries/imgui/imguizmo/ImGuizmo.cpp
    libraries/imgui/imguizmo/ImSequencer.cpp
    libraries/imgui/imnodes/imnodes.cpp
    libraries/imgui/implot/implot_demo.cpp
    libraries/imgui/implot/implot_items.cpp
    libraries/imgui/implot/implot.cpp
)

find_package(SDL2 REQUIRED)


include_directories(${SDL2_INCLUDE_DIRS})

include_directories(libraries/glad)

include_directories(libraries/imgui)

include_directories(libraries/imgui/imgui)

include_directories(libraries/glm)

include_directories(libraries/entt/src)

include_directories(libraries/sol2/include)

include_directories(/usr/local/include/luajit-2.1)

include_directories(libraries/simdjson)


add_library(glad SHARED libraries/glad/glad.c)


add_library(imgui SHARED ${IMGUI_SOURCES})

target_link_libraries(imgui ${SDL2_LIBRARIES} GL)


add_library(simdjson SHARED libraries/simdjson/simdjson/simdjson.cpp)


add_executable(sandbox ${SOURCES})

target_link_libraries(sandbox dl glad imgui luajit-5.1 simdjson)
