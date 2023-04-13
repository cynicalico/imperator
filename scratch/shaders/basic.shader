#pragma name(basic)

#pragma vertex
#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;

out vec4 frag_color;

void main() {
    frag_color = color;
    gl_Position = vec4(pos, 1.0);
}

#pragma fragment
#version 330 core
in vec4 frag_color;

out vec4 FragColor;

void main() {
    FragColor = frag_color;
}

