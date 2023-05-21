#version 300 es
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texco;
out vec2 vTexCo;
out vec2 vProjPos;
void main() {
    gl_Position = vec4(pos, 1.0);
    vTexCo = vec2(texco.x, 1.0f - texco.y);
    vProjPos = pos.xy;
}
