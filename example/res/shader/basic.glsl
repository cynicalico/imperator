#pragma name(basic)

#pragma vertex

#version 330 core
layout (location = 0) in vec3 in_pos;

void main() {
    gl_Position = vec4(in_pos, 1.0);
}

#pragma fragment

#version 330 core

out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.5, 0.25, 1.0);
}
