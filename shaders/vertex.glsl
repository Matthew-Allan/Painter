#version 330 core
layout (location = 0) in vec2 pos;

out vec2 f_pos;

void main() {
    gl_Position = vec4(
        (int(pos.x > 0) * 2) - 1,
        (int(pos.y > 0) * 2) - 1,
        0, 1
    );
    f_pos = pos;
}