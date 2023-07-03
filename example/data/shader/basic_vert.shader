#pragma vertex

#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out vec3 out_color;

uniform mat4 mvp;

void main() {
    out_color = color;
    gl_Position = mvp * vec4(pos, 1.0);
}
