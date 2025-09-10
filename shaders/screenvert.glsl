#version 330 core
layout (location = 0) in vec2 pos;

out vec2 f_pos;

uniform ivec2 offset;
uniform ivec2 screen_dim;

void main() {
    gl_Position = vec4(
        2 * ((pos.x + offset.x) / screen_dim.x) - 1,
        2 * ((pos.y + offset.y) / screen_dim.y) - 1,
        0, 1
    );
    f_pos = vec2(float(pos.x > 0), float(pos.y == 0));
}