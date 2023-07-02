#version 300 es
//semantic World World
//semantic View View
//semantic Projection Projection
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 texco;
layout(location = 3) in vec2 alphaco;
uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;
out vec3 vNor;
out vec2 vTexCo;
out vec2 vAlphaCo;
out vec4 vWorldPos;
void main() {
	vec4 tpos = vec4(pos, 1.0) * World;
    vWorldPos = tpos;
	tpos = tpos * View;
	gl_Position = tpos * Projection;
    vTexCo = texco;
    vAlphaCo = alphaco;
    mat4 nmx = inverse(World);
    nmx = transpose(nmx);
    vec4 tnor = vec4(nor, 0.0) * nmx;
    vNor = normalize(tnor.xyz);
}
