#type vertex
#version 330 core
#extension GL_ARB_explicit_uniform_location : require
precision mediump float;
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_Uv;
layout(location = 0) uniform mat4 u_Projection;
layout(location = 1) uniform mat4 u_View;
layout(location = 2) uniform mat4 u_Model;
out vec3 v_Normal;
out vec2 v_Uv;

void main() {
    gl_Position = u_Projection * u_View * u_Model * vec4(in_Position, 1);
    v_Normal = in_Normal;
    v_Uv = in_Uv;
}

#type fragment
#version 330 core
precision mediump float;
in vec2 v_Uv;
in vec3 v_Normal;
uniform sampler2D u_Texture;
out vec4 FragColor;

vec3 sunColor = vec3(1.0, 0.0, 0.0);
vec3 sunPosition = vec3(1.0, 1.0, 1.0);


void main() {
    float lum = max(dot(v_Normal, normalize(sunPosition)), 0.0);
    FragColor = texture2D(u_Texture, v_Uv) * vec4((lum) * sunColor, 1.0);
    // FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
