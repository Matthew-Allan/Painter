#version 330 core
in vec2 f_pos;

out vec4 col;

uniform sampler2D canvas_tex;
uniform ivec2 mouse_pos;
uniform ivec2 prev_mouse_pos;
uniform vec4 colour;
uniform bool pen_down;

void main() {
    const int size = 2;
    ivec2 A = mouse_pos;
    ivec2 u = prev_mouse_pos - mouse_pos;
    u.x |= int(u.x == 0 && u.y == 0);

    ivec2 P = ivec2(f_pos);
    ivec2 AP = P - A;

    float dist = cross(vec3(AP, 0), vec3(u, 0)).z / length(u);

    bool is_coloured = (
        pen_down && 
        abs(dist) < size &&
        P.x < max(mouse_pos.x, prev_mouse_pos.x) + size &&
        P.x > min(mouse_pos.x, prev_mouse_pos.x) - size &&
        P.y < max(mouse_pos.y, prev_mouse_pos.y) + size &&
        P.y > min(mouse_pos.y, prev_mouse_pos.y) - size
    );

    col = mix(texelFetch(canvas_tex, ivec2(f_pos), 0), colour, float(is_coloured));
}