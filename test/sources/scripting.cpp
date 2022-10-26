#include "scripting.hpp"

entt::registry* lua::registry;

entt::entity lua::createNode(const std::string &name) {
    if (name.empty()) {
        std::cout << "Node name must not be empty" << std::endl;

        return entt::null;
    }

    auto entity = lua::registry->create();
    lua::registry->emplace<Node>(entity, name);

    return entity;
}

template <typename T>
void lua::addComponent(entt::entity entity, T component) {
    lua::registry->emplace<T>(entity, component);
}

void loadScript(const std::string &path) {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package);
    lua["createNode"] = lua::createNode;
    lua["addComponent"] = lua::addComponent<Transform>;

    sol::usertype<glm::vec2> vec2Type = lua.new_usertype<glm::vec2>("Vec2", sol::constructors<glm::vec2(float, float)>());
    vec2Type["x"] = &glm::vec2::x;
    vec2Type["y"] = &glm::vec2::y;

    sol::usertype<glm::vec3> vec3Type = lua.new_usertype<glm::vec3>("Vec3", sol::constructors<glm::vec3(float, float, float)>());
    vec3Type["x"] = &glm::vec3::x;
    vec3Type["y"] = &glm::vec3::y;
    vec3Type["z"] = &glm::vec3::z;

    sol::usertype<glm::vec4> vec4Type = lua.new_usertype<glm::vec4>("Vec4", sol::constructors<glm::vec4(float, float, float, float)>());
    vec4Type["x"] = &glm::vec4::x;
    vec4Type["y"] = &glm::vec4::y;
    vec4Type["z"] = &glm::vec4::z;
    vec4Type["w"] = &glm::vec4::w;

    sol::usertype<Node> nodeType = lua.new_usertype<Node>("Node");
    nodeType["name"] = &Node::name;

    sol::usertype<Transform> transformType = lua.new_usertype<Transform>("Transform");
    transformType["translation"] = &Transform::translation;
    transformType["rotation"] = &Transform::rotation;
    transformType["scale"] = &Transform::scale;

    lua.script_file(path);
}
