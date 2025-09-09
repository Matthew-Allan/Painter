#version 330 core
in vec2 f_pos;

out vec4 col;

uniform sampler2D canvas_tex;

void main() {
    col = texture(canvas_tex, f_pos, 0);
}