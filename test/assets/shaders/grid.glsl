#type vertex
#version 330 core
#extension GL_ARB_explicit_uniform_location : require
precision mediump float;
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_Uv;
layout(location = 0) uniform mat4 u_Projection;
layout(location = 1) uniform mat4 u_View;
layout(location = 2) uniform vec3 u_CameraPosition;
out vec3 v_Normal;
out vec2 v_Uv;

void main() {
    mat4 translation = mat4(1.0);
    translation[0][0] = 1000.0;
    translation[2][2] = 1000.0;
    // translation[3] = vec4(u_CameraPosition.x, 0, u_CameraPosition.z, 1.0);
    gl_Position = u_Projection * u_View * translation * vec4(in_Position, 1);
    v_Uv = in_Uv;
}

#type fragment
#version 330 core
#extension GL_ARB_explicit_uniform_location : require
precision mediump float;
in vec2 v_Uv;
layout(location = 3) uniform vec3 u_Color;
out vec4 FragColor;

void main() {
    bool x = mod(v_Uv.x * 1000, 0.5) < 0.01;
    bool y = mod(v_Uv.y * 1000, 0.5) < 0.01;
    bool xAxis = step(0.5 - 0.00002, v_Uv.x) > 0 && step(v_Uv.x, 0.5 + 0.00002) > 0;
    bool zAxis = step(0.5 - 0.00002, v_Uv.y) > 0 && step(v_Uv.y, 0.5 + 0.00002) > 0;

    if (xAxis || zAxis) {
        FragColor = vec4(float(xAxis), 0.0, float(zAxis), 1.0);
    } else {
        FragColor = vec4(u_Color.x, u_Color.y, u_Color.z, x || y);
    }
}
