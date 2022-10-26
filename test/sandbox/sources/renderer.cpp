#include "renderer.hpp"

GLuint createShader(GLenum type, const GLchar* source) {
    GLuint shader = glCreateShader(type);
    GLint compileStatus = GL_FALSE;
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    if (!compileStatus) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        char infoLog[length];
        glGetShaderInfoLog(shader, length, &length, infoLog);
        glDeleteShader(shader);

        std::cout << infoLog << std::endl;

        return GL_FALSE;
    }

    return shader;
}

GLenum getShaderType(const std::string &type) {
    if (type == "vertex") {
        return GL_VERTEX_SHADER;
    }

    if (type == "fragment") {
        return GL_FRAGMENT_SHADER;
    }

    if (type == "geometry") {
        return GL_GEOMETRY_SHADER;
    }

    return GL_FALSE;
}

GLuint createShaderProgram(const std::map<std::string, std::string> &typeSourceMap) {
    std::vector<GLuint> shaders;

    for (auto [type, source] : typeSourceMap) {
        GLuint shader = createShader(getShaderType(type), source.c_str());

        if (!shader) {
            for (auto shader : shaders) {
                glDeleteShader(shader);
            }

            return GL_FALSE;
        }

        shaders.push_back(shader);
    }

    GLuint program = glCreateProgram();

    for (auto shader : shaders) {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);

    for (auto shader : shaders) {
        glDetachShader(program, shader);
        glDeleteShader(shader);
    }

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus) {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        char infoLog[length];
        glGetProgramInfoLog(program, length, &length, infoLog);
        glDeleteProgram(program);

        std::cout << infoLog << std::endl;

        return GL_FALSE;
    }

    return program;
}

GLuint loadShaderProgram(const std::string &path) {
    std::ifstream shaderProgramFile(path);

    if (!shaderProgramFile.is_open()) {
        std::cout << "Failed to open shader program file" << std::endl;

        return GL_FALSE;
    }

    std::string line;
    std::string type;
    std::string source;
    std::map<std::string, std::string> typeSourceMap;

    while (std::getline(shaderProgramFile, line)) {
        std::cmatch matches;

        if (std::regex_match(line.c_str(), matches, std::regex("#type ([a-z]{6,8})"))) {
            if (type.empty()) {
                type = matches[1];
            } else {
                if (type != matches[1]) {
                    typeSourceMap.insert({ type, source });
                    type = matches[1];
                    source = "";
                }
            }
        } else {
            if (!type.empty()) {
                source += line + "\n";
            }
        }
    }

    typeSourceMap.insert({ type, source });

    return createShaderProgram(typeSourceMap);
}

void updateCamera(Camera &camera) {
    glm::vec3 forward = glm::vec3(
        cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch)),
        sin(glm::radians(camera.pitch)),
        sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch))
    );
    camera.forward = glm::normalize(forward);
    camera.right = glm::normalize(glm::cross(camera.forward, camera.worldUp));
    camera.up = glm::normalize(glm::cross(camera.right, camera.forward));
}

void processMouse(Camera& camera, int x, int y) {
    camera.yaw += x * camera.sensitivity;
    camera.pitch -= y * camera.sensitivity;
    camera.pitch = std::clamp(camera.pitch, -89.0f, 89.0f);
    updateCamera(camera);
}

void processKeyboard(Camera& camera, float deltaTick, const Uint8* keyboardState) {
    float velocity = camera.speed * deltaTick;

    if (keyboardState[SDL_SCANCODE_S]) {
        camera.position -= camera.forward * velocity;
    }

    if (keyboardState[SDL_SCANCODE_W]) {
        camera.position += camera.forward * velocity;
    }

    if (keyboardState[SDL_SCANCODE_A]) {
        camera.position -= camera.right * velocity;
    }

    if (keyboardState[SDL_SCANCODE_D]) {
        camera.position += camera.right * velocity;
    }
}

glm::mat4 getCameraProjection(const Camera &camera, const glm::vec2 &viewport) {
    return glm::perspective(glm::radians(camera.zoom), (float) viewport.x / (float) viewport.y, camera.near, camera.far);
}

glm::mat4 getCameraView(const Camera &camera) {
    return glm::lookAt(camera.position, camera.position + camera.forward, camera.up);
}

Grid createGrid() {
    float positions[] = {
        -1.0f, 0.0f, -1.0f,
        -1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, -1.0f,
        -1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, -1.0f
    };

    float uvs[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    Grid grid;
    grid.shaderProgram = loadShaderProgram("../assets/shaders/grid.glsl");

    glGenVertexArrays(1, &grid.vao);
    glBindVertexArray(grid.vao);

    grid.positionBuffer;
    glGenBuffers(1, &grid.positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, grid.positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    grid.uvBuffer;
    glGenBuffers(1, &grid.uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, grid.uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(0);

    return grid;
}

void renderGrid(const Renderer &renderer) {
    glUseProgram(renderer.grid.shaderProgram);
    glBindVertexArray(renderer.grid.vao);
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(getCameraProjection(renderer.camera, renderer.viewport)));
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(getCameraView(renderer.camera)));
    glUniform3fv(2, 1, glm::value_ptr(renderer.camera.position));
    glUniform3fv(3, 1, glm::value_ptr(renderer.grid.color));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw(SDL_Window* window, Renderer &renderer) {
    SDL_GL_GetDrawableSize(window, &renderer.viewport.x, &renderer.viewport.y);
    glViewport(0, 0, renderer.viewport.x, renderer.viewport.y);
    glClearColor(renderer.clearColor.x, renderer.clearColor.y, renderer.clearColor.z, renderer.clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(renderer.shaderProgram);
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(getCameraProjection(renderer.camera, renderer.viewport)));
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(getCameraView(renderer.camera)));
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    renderGrid(renderer);
}
