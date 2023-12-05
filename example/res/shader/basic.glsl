#pragma name(basic)

#pragma vertex
#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_trans;
layout (location = 2) in vec4 in_color;

out vec4 out_color;

uniform mat4 mvp;

void main() {
    out_color = in_color;

    float c = cos(in_trans.z);
    float s = sin(in_trans.z);
    float x = in_trans.x;
    float y = in_trans.y;
    float m30 = -x * c + y * s + x;
    float m31 = -x * s - y * c + y;
    mat4 trans = mat4(
        vec4(c,   s,   0.0, 0.0),
        vec4(-s,  c,   0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(m30, m31, 0.0, 1.0)
    );

    gl_Position = mvp * trans * vec4(in_pos, 1.0);
}

#pragma fragment
#version 330 core
in vec4 out_color;

out vec4 FragColor;

void main() {
    FragColor = out_color;
}
