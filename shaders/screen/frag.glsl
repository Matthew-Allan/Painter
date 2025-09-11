#version 330 core
in vec2 f_pos;

out vec4 col;

uniform sampler2D canvas_tex;
uniform ivec2 screen_dim;

void main() {
    col = texture(canvas_tex, vec2(f_pos.x, screen_dim.y - f_pos.y), 0);
}