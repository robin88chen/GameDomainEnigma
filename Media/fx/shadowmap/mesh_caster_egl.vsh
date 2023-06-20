#version 300 es
//semantic World World
//semantic View View
//semantic Projection Projection
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 texco;
uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;
out vec2 vDepth;
void main() {
	vec4 tpos = vec4(pos, 1.0) * World;
	tpos = tpos * View;
    tpos = tpos * Projection;
	gl_Position = tpos;
	vDepth.xy = tpos.zw;
}
