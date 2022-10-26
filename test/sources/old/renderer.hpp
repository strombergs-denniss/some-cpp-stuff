#pragma once
#include "core.hpp"
#include "simdjson.h"
#include <utility>

enum TextureFormat {
    RGB = 3,
    RGBA = 4
};

struct Material {
    std::string_view name;
};

struct Primitive {
    std::vector<std::pair<const char*, uint8_t>> attributes;
    uint32_t indices;
    uint32_t material;
};

struct Mesh {
    std::string_view name;
    std::vector<Primitive> primitives;
};

struct Node {
    Mesh mesh;
    std::string_view name;
};

struct Scene {
    std::string_view name;
    std::vector<Node> nodes;
};

struct Accessor {
    uint32_t bufferView;
    uint32_t componentType;
    uint32_t count;
    std::string_view type;
};

struct BufferView {
    uint32_t buffer;
    uint32_t byteLength;
    uint32_t byteOffset;
};

struct Texture {
    unsigned int width;
    unsigned int height;
    TextureFormat format;
    unsigned char *data;
};

struct Model {
    std::vector<float> positions;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<unsigned int> indices;
    glm::vec3 position;
    glm::mat4 model;
    std::vector<Accessor> accessors;
    std::vector<BufferView> bufferViews;
    std::vector<Mesh> meshes;
    char* buffer;
    std::vector<GLuint> buffers;
};

struct Renderer {
    GLuint program;
    GLint projectionUniform;
    GLint viewUniform;
    GLint modelUniform;
    std::vector<Model> models;
    Texture texture;
};

Texture generateTexture(unsigned int width, unsigned int height, TextureFormat format);

Renderer setup();

void draw(const Camera &camera, Renderer &rendererData);

void generateTriangle(Model &model, glm::vec3 point0, glm::vec3 point1, glm::vec3 point2);

void generateCube(Model &model, glm::vec3 size);

Model createModel(glm::vec3 position);

void generateQuad(Model &model, glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC);

void debugAccessor(const Model &model, const Accessor &accessor, char *buffer);

void loadModel(Model &model, const char *path);

void saveModel(const Model &model);

void bindModel(const Model& model);
