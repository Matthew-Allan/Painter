#version 330 core
in vec2 f_pos;

out vec4 col;

uniform sampler2D canvas_tex;
uniform ivec2 mouse_pos;
uniform ivec2 screen_dim;
uniform vec4 colour;
uniform bool pen_down;

void main() {
    ivec2 mp = mouse_pos;
    mp.y = screen_dim.y - mp.y;
    bool is_coloured = pen_down && length(mp - ivec2(f_pos)) < 50;
    col = mix(texelFetch(canvas_tex, ivec2(f_pos), 0), colour, float(is_coloured));
}