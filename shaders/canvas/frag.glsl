#version 330 core
in vec2 f_pos;

out vec4 col;

uniform sampler2D canvas_tex;
uniform ivec2 mouse_pos;
uniform ivec2 prev_mouse_pos;
uniform vec4 colour;
uniform int size;

#define M_PI 3.1415926535897932384626433832795

float cr(vec2 a, float val) {
    return abs(a.x * abs(val) - a.y * val);
}

float largest(float a, float b) {
    return abs(a) > abs(b) ? a : b;
}

mat2 invShear(float shear) {
    return mat2(
        1, 0,
        -shear, 1
    );
}

mat2 invRotate(float theta) {
    return mat2(
        cos(theta), -sin(theta),
        sin(theta), cos(theta)
    );
}

void main() {
    vec2 u = mouse_pos - prev_mouse_pos;
    float len = length(u);
    float theta = atan(u.y, u.x);
    
    mat2 inv_rotate = invRotate(theta);

    #ifdef SQUARE
    mat2 inv_shear = invShear(1 / largest(tan((M_PI / 4) - theta), tan((3 * M_PI / 4) - theta)));
    float middle = max(cr(u, size), cr(u, -size)) / len;
    #else
    mat2 inv_shear = invShear(0);
    float middle = size;
    #endif

    vec2 pos = inv_shear * inv_rotate * (f_pos - prev_mouse_pos);
    bool is_coloured = (
        pos.x <= len && pos.x >= 0 && 
        pos.y <= middle && pos.y >= -middle ||
        #ifdef SQUARE
        (f_pos.x < mouse_pos.x + size && f_pos.x > mouse_pos.x - size &&
        f_pos.y < mouse_pos.y + size && f_pos.y > mouse_pos.y - size)
        #else
        length(f_pos - mouse_pos) < size
        #endif
    );
    
    col = mix(texelFetch(canvas_tex, ivec2(f_pos), 0), colour, float(is_coloured));
}