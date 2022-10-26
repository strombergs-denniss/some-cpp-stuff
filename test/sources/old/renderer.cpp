#include "renderer.hpp"

Texture generateTexture(unsigned int width, unsigned int height, TextureFormat format) {
    Texture texture;
    texture.width = width;
    texture.height = height;
    texture.format = format;
    texture.data = new unsigned char[width * height * format];

    for (unsigned int x = 0; x < width; ++x) {
        for (unsigned int y = 0; y < height; ++y) {
            unsigned int index = width * y + x;
            unsigned char value = x ^ y;
            texture.data[index * format] = value;
            texture.data[index * format + 1] = value;
            texture.data[index * format + 2] = value;

            if (format == TextureFormat::RGBA) {
                texture.data[index * format + 3] = value;
            }
        }
    }

    return texture;
}

Renderer setup() {
    Renderer renderer;
    renderer.program = loadShader("../assets/shaders/main.glsl");
    renderer.projectionUniform = glGetUniformLocation(renderer.program, "u_Projection");
    renderer.viewUniform = glGetUniformLocation(renderer.program, "u_View");
    renderer.modelUniform = glGetUniformLocation(renderer.program, "u_Model");
    glUseProgram(renderer.program);

    renderer.texture = generateTexture(256, 256, TextureFormat::RGB);
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    Model model = createModel(glm::vec3(0.0f, 0.0f, 0.0f));
    renderer.models.push_back(model);

    Model model1;
    loadModel(model, "../assets/models/cube.glb");
    bindModel(model);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_CULL_FACE);
    // glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    return renderer;
}

void draw(const Camera &camera, Renderer &renderer) {
    glViewport(0, 0, camera.width, camera.height);
    glClearColor(camera.clearColor.x, camera.clearColor.y, camera.clearColor.z, camera.clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUniformMatrix4fv(renderer.projectionUniform, 1, GL_FALSE, glm::value_ptr(getCameraProjection(camera)));
    glUniformMatrix4fv(renderer.viewUniform, 1, GL_FALSE, glm::value_ptr(getCameraView(camera)));

    GLenum format = renderer.texture.format == TextureFormat::RGB ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, renderer.texture.width, renderer.texture.height, 0, format, GL_UNSIGNED_BYTE, renderer.texture.data);

    for (auto model : renderer.models) {
        glUniformMatrix4fv(renderer.modelUniform, 1, GL_FALSE, glm::value_ptr(model.model));
        glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, nullptr);
    }
}

void generateTriangle(Model &model, glm::vec3 point0, glm::vec3 point1, glm::vec3 point2) {
}

void generateQuad(Model &model, glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC) {
    unsigned int lastIndex = model.positions.size() / 3 - 1;

    glm::vec3 deltaAC = pointA - pointC;
    glm::vec3 pointD = pointB + deltaAC;
    glm::vec3 direction = glm::normalize(glm::cross(pointB - pointA, pointC - pointA));

    model.positions.insert(model.positions.end(), {pointA.x, pointA.y, pointA.z,
                                                   pointC.x, pointC.y, pointC.z,
                                                   pointB.x, pointB.y, pointB.z,
                                                   pointD.x, pointD.y, pointD.z});

    model.uvs.insert(model.uvs.end(), {0.0f, 0.0f,
                                       1.0f, 0.0f,
                                       1.0f, 1.0f,
                                       0.0f, 1.0f});

    model.indices.insert(model.indices.end(), {lastIndex + 1, lastIndex + 2, lastIndex + 3,
                                               lastIndex + 1, lastIndex + 3, lastIndex + 4});

    model.normals.insert(model.normals.end(), {direction.x, direction.y, direction.z,
                                               direction.x, direction.y, direction.z,
                                               direction.x, direction.y, direction.z,
                                               direction.x, direction.y, direction.z});
}

void generateCube(Model &model, glm::vec3 size) {
    model.positions = {
        -size.x, size.y, size.z,
        size.x, size.y, size.z,
        size.x, -size.y, size.z,
        -size.x, -size.y, size.z
    };
    model.uvs = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    model.indices = {
        0, 1, 2,
        0, 2, 3
    };
}

void bindModel(const Model& model) {

}

Model createModel(glm::vec3 position) {
    Model model;
    // generateCube(model, glm::vec3(1.0f));
    generateQuad(model, glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, -1.0f)); // Back
    generateQuad(model, glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));    // Front

    generateQuad(model, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, 1.0f, 1.0f));    // Top
    generateQuad(model, glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, 1.0f)); // Bottom

    generateQuad(model, glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 1.0f, 1.0f)); // Left
    generateQuad(model, glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));    // Right

    generateQuad(model, glm::vec3(-100.0f, -10.0f, -100.0f), glm::vec3(100.0f, -10.0f, 100.0f), glm::vec3(-100.0f, -10.0f, 100.0f)); // Right

    model.position = position;
    model.model = glm::translate(glm::mat4(1.0f), position);

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(float), &model.positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FLOAT, 3 * sizeof(float), nullptr);

    GLuint uvBuffer;
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, model.uvs.size() * sizeof(float), &model.uvs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FLOAT, 2 * sizeof(float), nullptr);

    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(float), &model.normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FLOAT, 3 * sizeof(float), nullptr);

    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(unsigned int), &model.indices[0], GL_STATIC_DRAW);

    return model;
}

void debugAccessor(const Model &model, const Accessor &accessor, char *buffer) {
    BufferView bufferView = model.bufferViews[accessor.bufferView];
    uint8_t increment = accessor.type == "SCALAR" ? 2 : 4;

    for (uint32_t a = bufferView.byteOffset; a < bufferView.byteOffset + bufferView.byteLength; a += increment) {
        if (accessor.type == "SCALAR") {
            uint16_t value;
            memcpy(&value, buffer + a, sizeof(value));
            std::cout << value << "\n";
        } else {
            float value;
            memcpy(&value, buffer + a, sizeof(value));
            std::cout << value << "\n";
        }
    }
}

void loadModel(Model &model, const char *path) {
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Failed to open model file\n";
    }

    uint32_t magic;
    file.read((char *)&magic, sizeof(uint32_t));
    file.seekg(4);

    uint32_t version;
    file.read((char *)&version, sizeof(uint32_t));
    file.seekg(8);

    uint32_t length;
    file.read((char *)&length, sizeof(uint32_t));
    file.seekg(12);

    uint32_t jsonChunkLength;
    file.read((char *)&jsonChunkLength, sizeof(uint32_t));
    file.seekg(16);

    uint32_t jsonChunkType;
    file.read((char *)&jsonChunkType, sizeof(uint32_t));
    file.seekg(20);

    char jsonChunkData[jsonChunkLength];
    file.read(jsonChunkData, jsonChunkLength);
    file.seekg(20 + jsonChunkLength);

    uint32_t binaryChunkLength;
    file.read((char *)&binaryChunkLength, sizeof(uint32_t));
    file.seekg(24 + jsonChunkLength);

    uint32_t binaryChunkType;
    file.read((char *)&binaryChunkType, sizeof(uint32_t));
    file.seekg(28 + jsonChunkLength);

    char binaryChunkData[binaryChunkLength];
    file.read(binaryChunkData, binaryChunkLength);

    simdjson::ondemand::parser parser;
    simdjson::padded_string json = simdjson::padded_string(jsonChunkData, jsonChunkLength);
    simdjson::ondemand::document data = parser.iterate(json);

    for (auto meshItem : data["meshes"]) {
        Mesh mesh;
        mesh.name = meshItem["name"].get_string();

        for (auto primitiveItem : meshItem["primitives"]) {
            Primitive primitive;
            simdjson::ondemand::object attributes = primitiveItem["attributes"].get_object();

            for (auto field : attributes) {
                primitive.attributes.push_back({ field.key().raw(), field.value().get_uint64() });
            }

            primitive.indices = primitiveItem["indices"].get_uint64();
            primitive.material = primitiveItem["material"].get_uint64();
            mesh.primitives.push_back(primitive);
        }

        model.meshes.push_back(mesh); 
    }

    for (auto accessorItem : data["accessors"]) {
        Accessor accessor;
        accessor.bufferView = accessorItem["bufferView"].get_uint64();
        accessor.componentType = accessorItem["componentType"].get_uint64();
        accessor.count = accessorItem["count"].get_uint64();
        accessor.type = accessorItem["type"].get_string();
        model.accessors.push_back(accessor);
    }

    for (auto bufferViewItem : data["bufferViews"]) {
        BufferView bufferView;
        bufferView.buffer = bufferViewItem["buffer"].get_uint64();
        bufferView.byteLength = bufferViewItem["byteLength"].get_uint64();
        bufferView.byteOffset = bufferViewItem["byteOffset"].get_uint64();
        model.bufferViews.push_back(bufferView);
    }
}

void saveModel(const Model &model) {
}
