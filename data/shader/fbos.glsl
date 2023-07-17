#pragma name(fbos)

#pragma vertex

#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec2 in_tex_coords;
layout (location = 3) in vec3 in_trans;

out vec4 out_color;
out vec2 out_tex_coords;

uniform float z_max;
uniform mat4 mvp;

void main() {
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

    float z = -(z_max - in_pos.z) / (z_max + 1.0);
    gl_Position = mvp * trans * vec4(in_pos.xy, z, 1.0);

    out_color = in_color;
    out_tex_coords = in_tex_coords;
}

#pragma fragment

#version 330 core
in vec4 out_color;
in vec2 out_tex_coords;

out vec4 FragColor;

uniform sampler2D tex;

void main() {
    FragColor = out_color * texture(tex, out_tex_coords);
}