#include <iostream>
#include <cstdio>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <sol/sol.hpp>

struct Mesh {
    std::string name = "mesh";
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::ivec3> indices;
};

Mesh createMesh() {
    return Mesh();
}

void addPosition(Mesh &mesh, glm::vec3 position) {
    mesh.positions.push_back(position);
}

glm::vec3 createVec3(float x, float y, float z) {
    return glm::vec3(x, y, z);
}

int main() {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua["Vec3"] = lua.create_table();
    lua["Vec3"]["create"] = createVec3;

    lua["Mesh"] = lua.create_table();
    lua["Mesh"]["create"] = createMesh;
    lua["Mesh"]["addPosition"] = addPosition;

    

    // lua["addPosition"] = addPosition;

    // sol::usertype<Mesh> meshType = lua.new_usertype<Mesh>("Mesh");

    // sol::usertype<glm::vec2> vec2Type = lua.new_usertype<glm::vec2>("Vec2", sol::constructors<glm::vec2(float, float)>());
    // vec2Type["x"] = &glm::vec2::x;
    // vec2Type["y"] = &glm::vec2::y;

    // sol::usertype<glm::vec3> vec3Type = lua.new_usertype<glm::vec3>("Vec3", sol::constructors<glm::vec3(float, float, float)>());
    // vec3Type["x"] = &glm::vec3::x;
    // vec3Type["y"] = &glm::vec3::y;
    // vec3Type["z"] = &glm::vec3::z;

    // sol::usertype<glm::vec4> vec4Type = lua.new_usertype<glm::vec4>("Vec4", sol::constructors<glm::vec4(float, float, float, float)>());
    // vec4Type["x"] = &glm::vec4::x;
    // vec4Type["y"] = &glm::vec4::y;
    // vec4Type["z"] = &glm::vec4::z;
    // vec4Type["w"] = &glm::vec4::w;

    // sol::usertype<glm::mat4> mat4Type = lua.new_usertype<glm::mat4>("Mat4", sol::constructors<glm::mat4(float)>());

    lua.script_file("../generators/test.lua");

    return 0;
}
