#include "renderer.hpp"
#include "gui.hpp"
#include "scripting.hpp"
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_opengl3.h>

struct Project {
    std::string name;
    bool isLoaded = true;
};

void createProject(const char* name) {
    std::string path = "../projects/" + std::string(name) + "/";

    if (fs::exists(path)) {
        std::cout << "Project with same name already exists" << std::endl;
    } else {
        fs::create_directory(path);
        fs::create_directory(path + "assets");
        // test.project.name = std::string(name);
    }
}

void init(Renderer &renderer) {
    renderer.shaderProgram = loadShaderProgram("../assets/shaders/main.glsl");
    renderer.grid = createGrid();

    int width = 128;
    int height = 128;
    unsigned char data[width * height * 3];

    for (unsigned int x = 0; x < width; ++x) {
        for (unsigned int y = 0; y < height; ++y) {
            unsigned int index = width * y + x;
            unsigned char value = 255;
            data[index * 3] = value;
            data[index * 3 + 1] = value;
            data[index * 3 + 2] = value;
        }
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    updateCamera(renderer.camera);
    // glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderer.models.push_back(Model());
    int modelStatus = loadModel(renderer.models[0], "../assets/models/cube.glb");

    if (modelStatus == -1) {
        std::cout << "Error while loading model" << std::endl;
    } else {
        bindModel(renderer.models[0]);
    }

    loadScript("../assets/scripts/main.lua");
}

int main() {
    // Initialize SDL
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
    SDL_Window* window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, displayMode.w, displayMode.h, windowFlags);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;

        return -1;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Initialize engine
    bool isActive = true;
    Uint32 lastTick = 0;
    float deltaTick = 0.0f;
    entt::registry registry;
    Renderer renderer;
    lua::registry = &registry;
    init(renderer);

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

    // Clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
