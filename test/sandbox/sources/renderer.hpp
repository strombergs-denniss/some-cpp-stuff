#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <vector>
#include <map>

struct Mesh {
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> uvs;
};

struct Camera {
    glm::vec3 position = glm::vec3(0.0f, 2.0f, 10.0f);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float near = 0.1f;
    float far = 1000.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float zoom = 45.0f;
    float speed = 10.0f;
    float sensitivity = 0.1f;
};

struct Grid {
    GLuint shaderProgram;
    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
    GLuint vao;
    GLuint positionBuffer;
    GLuint uvBuffer;
};

struct Renderer {
    glm::ivec2 viewport = glm::ivec2(1920, 1080);
    glm::vec4 clearColor = glm::vec4(1.0f, 1.0, 1.0f, 1.0f);
    GLuint shaderProgram;
    Camera camera;
    Grid grid;
    std::vector<Mesh> mehes;
};

struct Node {
    std::string name;
    int mesh = -1;
    std::vector<int> children;
};

struct Transform {
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 matrix;
};

struct MeshReference {
    int index;
};

GLuint createShader(GLenum type, const GLchar* source);

GLenum getShaderType(const std::string &type);

GLuint createShaderProgram(const std::map<std::string, std::string> &typeSourceMap);

GLuint loadShaderProgram(const std::string &path);

void updateCamera(Camera &camera);

void processMouse(Camera &camera, int x, int y);

void processKeyboard(Camera &camera, float deltaTick, const Uint8* keyboardState);

glm::mat4 getCameraProjection(const Camera &camera, const glm::vec2 &viewport);

glm::mat4 getCameraView(const Camera &camera);

Grid createGrid();

void renderGrid(const Renderer &renderer);

void draw(SDL_Window* window, Renderer &renderer);
