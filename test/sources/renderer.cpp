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

int getComponentCount(const std::string &type) {
    if (type == "VEC2") {
        return 2;
    }

    if (type == "VEC3") {
        return 3;
    }

    if (type == "VEC4") {
        return 4;
    }

    return 0;
}

int loadModel(Model &model, const std::string &path) {
    std::ifstream modelFile(path, std::ios::binary);

    if (!modelFile.is_open()) {
        std::cout << "Failed to open model file" << std::endl;
    }

    modelFile.seekg(12);

    uint32_t jsonLength;
    modelFile.read((char*) &jsonLength, sizeof(uint32_t));
    modelFile.seekg(20);

    std::string json;
    json.resize(jsonLength);
    modelFile.read(json.data(), jsonLength);

    uint32_t bufferLength;
    modelFile.read((char*) &bufferLength, sizeof(bufferLength));
    modelFile.seekg(sizeof(uint32_t), std::ios_base::cur);

    model.buffer.reserve(bufferLength);
    modelFile.read(model.buffer.data(), bufferLength);

    try {
        simdjson::ondemand::parser parser;
        auto paddedJson = simdjson::padded_string(json.data(), jsonLength);
        auto document = parser.iterate(paddedJson);

        auto error = document["scene"].get_int64().get((int64_t&) model.scene);

        simdjson::ondemand::array scenes;
        error = document["scenes"].get_array().get(scenes);

        if (!error) {
            model.scenes.reserve(scenes.count_elements());

            for (auto sceneElement : scenes) {
                Scene scene;

                std::string_view name;
                error = sceneElement["name"].get_string().get(name);
                scene.name = std::string(name.data(), name.size());

                simdjson::ondemand::array nodes;
                error = sceneElement["nodes"].get_array().get(nodes);

                if (!error) {
                    scene.nodes.reserve(nodes.count_elements());

                    for (auto nodeItem : nodes) {
                        scene.nodes.push_back((int) nodeItem.get_int64());
                    }
                }

                model.scenes.push_back(scene);
            }
        }

        simdjson::ondemand::array nodes;
        error = document["nodes"].get_array().get(nodes);

        if (!error) {
            model.nodes.reserve(nodes.count_elements());

            for (auto nodeElement : nodes) {
                Node node;

                std::string_view name;
                error = nodeElement["name"].get_string().get(name);
                node.name = std::string(name.data(), name.size());
                error = nodeElement["mesh"].get_int64().get((int64_t&) node.mesh);

                simdjson::ondemand::array children;
                error = nodeElement["children"].get_array().get(children);

                if (!error) {
                    node.children.reserve(children.count_elements());

                    for (auto childItem : children) {
                        node.children.push_back((int) childItem.get_int64());
                    }
                }

                model.nodes.push_back(node);
            }
        }

        simdjson::ondemand::array meshes;
        error = document["meshes"].get_array().get(meshes);

        if (!error) {
            model.meshes.reserve(meshes.count_elements());

            for (auto meshElement : meshes) {
                Mesh mesh;

                std::string_view name;
                error = meshElement["name"].get_string().get(name);
                mesh.name = std::string(name.data(), name.size());

                auto meshPrimitives = meshElement["primitives"].get_array();
                mesh.primitives.reserve(meshPrimitives.count_elements());

                for (auto meshPrimitiveItem : meshPrimitives) {
                    MeshPrimitive meshPrimitive;

                    auto attributes = meshPrimitiveItem["attributes"].get_object();

                    for (auto field : attributes) {
                        PrimitiveAttribute primitiveAttribute;

                        std::string_view key = field.unescaped_key();

                        primitiveAttribute.key = std::string(key.data(), key.size());
                        primitiveAttribute.value = field.value().get_int64();
                        meshPrimitive.attributes.push_back(primitiveAttribute);
                    }

                    error = meshPrimitiveItem["indices"].get_int64().get((int64_t&) meshPrimitive.indices);
                    error = meshPrimitiveItem["material"].get_int64().get((int64_t&) meshPrimitive.material);

                    mesh.primitives.push_back(meshPrimitive);
                }

                model.meshes.push_back(mesh);
            }
        }

        simdjson::ondemand::array accessors;
        error = document["accessors"].get_array().get(accessors);

        if (!error) {
            model.accessors.reserve(accessors.count_elements());

            for (auto accessorElement : accessors) {
                Accessor accessor;
                error = accessorElement["bufferView"].get_int64().get((int64_t&) accessor.bufferView);
                accessor.componentType = (int) accessorElement["componentType"].get_int64();
                accessor.count = (int) accessorElement["count"].get_int64();

                std::string_view type;
                error = accessorElement["type"].get_string().get(type);
                accessor.type = std::string(type.data(), type.size());

                model.accessors.push_back(accessor);
            }
        }

        simdjson::ondemand::array bufferViews;
        error = document["bufferViews"].get_array().get(bufferViews);

        if (!error) {
            model.bufferViews.reserve(bufferViews.count_elements());

            for (auto bufferViewElement : bufferViews) {
                BufferView bufferView;
                
                bufferView.buffer = (int) bufferViewElement["buffer"].get_int64();
                bufferView.byteLength = (int) bufferViewElement["byteLength"].get_int64();
                error = bufferViewElement["byteOffset"].get_int64().get((int64_t&) bufferView.byteOffset);

                model.bufferViews.push_back(bufferView);
            }
        }
    } catch (simdjson::simdjson_error &e) {
        std::cout << e.error() << std::endl;

        return -1;
    }

    return 0;
}

void bindModel(Model &model) {
    Scene &scene = model.scenes[model.scene];

    for (auto nodeIndex : scene.nodes) {
        Node &node = model.nodes[nodeIndex];

        if (node.mesh > -1) {
            Mesh &mesh = model.meshes[node.mesh];
            MeshPrimitive &meshPrimitive = mesh.primitives[0];
            glGenVertexArrays(1, &model.vao);
            glBindVertexArray(model.vao);

            for (auto &primitiveAttribute : meshPrimitive.attributes) {
                Accessor &accessor = model.accessors[primitiveAttribute.value];
                BufferView &bufferView = model.bufferViews[accessor.bufferView];

                int componentCount = getComponentCount(accessor.type);

                GLuint buffer;
                glGenBuffers(1, &buffer);
                glBindBuffer(GL_ARRAY_BUFFER, buffer);
                glBufferData(GL_ARRAY_BUFFER, bufferView.byteLength, &model.buffer[0] + bufferView.byteOffset, GL_STATIC_DRAW);
                glEnableVertexAttribArray(primitiveAttribute.value);
                glVertexAttribPointer(primitiveAttribute.value, componentCount, accessor.componentType, GL_FALSE, componentCount * sizeof(float), nullptr);
            }

            Accessor &indexAccessor = model.accessors[meshPrimitive.indices];
            BufferView &indexBufferView = model.bufferViews[indexAccessor.bufferView];

            GLuint buffer;
            glGenBuffers(1, &buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferView.byteLength, &model.buffer[0] + indexBufferView.byteOffset, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
        }
    }
}

void drawModels(Renderer &renderer) {
    for (auto model : renderer.models) {
        Scene &scene = model.scenes[model.scene];

        for (auto nodeIndex : scene.nodes) {
            Node &node = model.nodes[nodeIndex];

            if (node.mesh > -1) {
                Mesh &mesh = model.meshes[node.mesh];
                MeshPrimitive &meshPrimitive = mesh.primitives[0];
                Accessor &indexAccessor = model.accessors[meshPrimitive.indices];
                glBindVertexArray(model.vao);
                glDrawElements(GL_TRIANGLES, indexAccessor.count, GL_UNSIGNED_SHORT, nullptr);
            }
        }
    }
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
    drawModels(renderer);
    renderGrid(renderer);
}
