#pragma name(points)

#pragma vertex

#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec4 in_color;

out vec4 out_color;

uniform float z_max;
uniform mat4 mvp;

void main() {
    out_color = in_color;

    float z = -(z_max - in_pos.z) / (z_max + 1.0);

    gl_Position = mvp * vec4(in_pos.x + 0.5, in_pos.y + 0.5, z, 1.0);
}

#pragma fragment

#version 330 core
in vec4 out_color;

out vec4 FragColor;

void main() {
    FragColor = vec4(out_color.xyz * out_color.a, out_color.a);
}
