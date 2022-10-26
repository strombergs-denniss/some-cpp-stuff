#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <string>
#include "renderer.hpp"
#include <entt/entt.hpp>

static sol::state state;
static entt::registry registry;

void require(std::string name) {
    state.script_file("../generators/" + name + ".lua");
}

entt::entity createNode(const std::string &name) {
    if (name.empty()) {
        std::cout << "Node name must not be empty" << std::endl;

        return entt::null;
    }

    auto entity = registry.create();
    registry.emplace<Node>(entity, name);

    return entity;
}

template <typename T>
void addComponent(entt::entity entity, T component) {
    registry.emplace<T>(entity, component);
}

void renderGui(entt::registry &registry, Renderer &renderer) {
    ImGui::ShowDemoWindow();

    ImGui::Begin("Explorer");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::ColorEdit4("Clear Color", (float*) &renderer.clearColor);
    ImGui::ColorEdit3("Grid Color", (float*) &renderer.grid.color);
    ImGui::DragFloat3("Camera Position", (float*) &renderer.camera.position, 0.1f);
    ImGui::DragFloat("Camera Speed", (float*) &renderer.camera.speed, 0.1f);

    static auto view = registry.view<Node>();
    static entt::entity selected = entt::null;

    if (ImGui::CollapsingHeader("Nodes")) {
        for(auto entity : view) {
            auto &node = view.get<Node>(entity);

            if (ImGui::Selectable(node.name.c_str(), entity == selected)) {
                selected = entity;
            }
        }
    }

    if (ImGui::CollapsingHeader("Components")) {
        if (registry.valid(selected)) {
            if (registry.any_of<Node>(selected) && ImGui::TreeNode("Node")) {
                auto &node = view.get<Node>(selected);

                ImGui::InputText("Name", node.name.data(), IM_ARRAYSIZE(node.name.data()), ImGuiInputTextFlags_ReadOnly);
                ImGui::TreePop();
            }

            if (registry.any_of<Transform>(selected) && ImGui::TreeNode("Transform")) {
                auto &transform = registry.get<Transform>(selected);

                ImGui::DragFloat3("Translation", (float*) &transform.translation, 0.1f);
                ImGui::DragFloat3("Rotation", (float*) &transform.rotation, 0.1f);
                ImGui::DragFloat3("Scale", (float*) &transform.scale, 0.1f);
                ImGui::TreePop();
            }

            if (registry.any_of<MeshReference>(selected) && ImGui::TreeNode("MeshReference")) {
                auto &meshReference = registry.get<MeshReference>(selected);

                ImGui::InputInt("Index", &meshReference.index, 0, 0, ImGuiInputTextFlags_ReadOnly);
                ImGui::TreePop();
            }
        } else {
            ImGui::Text("Select node to see its components");
        }
    }

    ImGui::End();
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cout << "Error: " << SDL_GetError() << std::endl;

        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    SDL_WindowFlags windowFlags = (SDL_WindowFlags) (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Sandbox", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, displayMode.w, displayMode.h, windowFlags);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;

        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool isActive = true;
    int width = displayMode.w;
    int height = displayMode.h;
    Uint32 lastTick = 0;
    float deltaTick = 0.0f;

    Renderer renderer;
    renderer.shaderProgram = loadShaderProgram("../assets/shaders/main.glsl");
    renderer.grid = createGrid();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    state.open_libraries(sol::lib::base);
    state["require"] = require;
    state["createNode"] = createNode;
    state["addComponent"] = sol::overload(addComponent<Transform>, addComponent<MeshReference>);

    sol::usertype<glm::vec2> vec2Type = state.new_usertype<glm::vec2>("Vec2", sol::constructors<glm::vec2(float, float)>());
    vec2Type["x"] = &glm::vec2::x;
    vec2Type["y"] = &glm::vec2::y;

    sol::usertype<glm::vec3> vec3Type = state.new_usertype<glm::vec3>("Vec3", sol::constructors<glm::vec3(float, float, float)>());
    vec3Type["x"] = &glm::vec3::x;
    vec3Type["y"] = &glm::vec3::y;
    vec3Type["z"] = &glm::vec3::z;

    sol::usertype<glm::vec4> vec4Type = state.new_usertype<glm::vec4>("Vec4", sol::constructors<glm::vec4(float, float, float, float)>());
    vec4Type["x"] = &glm::vec4::x;
    vec4Type["y"] = &glm::vec4::y;
    vec4Type["z"] = &glm::vec4::z;
    vec4Type["w"] = &glm::vec4::w;

    sol::usertype<Node> nodeType = state.new_usertype<Node>("Node");
    nodeType["name"] = &Node::name;

    sol::usertype<Transform> transformType = state.new_usertype<Transform>("Transform");
    transformType["translation"] = &Transform::translation;
    transformType["rotation"] = &Transform::rotation;
    transformType["scale"] = &Transform::scale;

    sol::usertype<MeshReference> meshReferenceType = state.new_usertype<MeshReference>("MeshReference");
    meshReferenceType["index"] = &MeshReference::index;

    state.script_file("../generators/scene.lua");

    while (isActive) {
        Uint32 tick = SDL_GetTicks();
        deltaTick = (tick - lastTick) / 1000.0f;
        lastTick = tick;

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (!SDL_GetRelativeMouseMode()) {
                ImGui_ImplSDL2_ProcessEvent(&event);
            }

            if (event.type == SDL_MOUSEMOTION && SDL_GetRelativeMouseMode()) {
                processMouse(renderer.camera, event.motion.xrel, event.motion.yrel);
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                SDL_SetRelativeMouseMode((SDL_bool) !SDL_GetRelativeMouseMode());
            }

            if (event.type == SDL_QUIT) {
                isActive = false;
            }

            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
                isActive = false;
            }
        }

        if (SDL_GetRelativeMouseMode()) {
            processKeyboard(renderer.camera, deltaTick, SDL_GetKeyboardState(NULL));
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        renderGui(registry, renderer);
        ImGui::Render();
        draw(window, renderer);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
