#version 300 es
precision mediump float;
uniform float offset_x;
uniform float offset_y;
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texco;
out vec2 vTexCo;
out vec4 vColor;
void main() {
    gl_Position = vec4(pos, 1.0) + vec4(offset_x, offset_y, 0.0f, 0.0f);
    vColor = vec4(0.5f + pos.x + pos.y, 0.5f + pos.x - pos.y, 0.5f + pos.y - pos.x, 1.0f);
    vTexCo = texco;
}
