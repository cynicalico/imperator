#pragma name(basic)

#pragma vertex
#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;

out vec3 out_color;

uniform mat4 mvp;

void main() {
    out_color = in_color;
    gl_Position = mvp * vec4(in_pos, 1.0);
}

#pragma fragment
#version 330 core
in vec3 out_color;

out vec4 FragColor;

void main() {
    FragColor = vec4(out_color, 1.0);
}
