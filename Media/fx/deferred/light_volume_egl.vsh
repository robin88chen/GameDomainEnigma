#version 300 es
//semantic World World
//semantic View View
//semantic Projection Projection
layout(location = 0) in vec3 pos;
uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;
out vec2 vTexCo;
out vec2 vProjPos;
void main() {
	vec4 tpos = vec4(pos, 1.0) * World;
	tpos = tpos * View;
    tpos = tpos * Projection;
	gl_Position = tpos;
    vec2 projPos = tpos.xy / tpos.w;
    vProjPos = projPos;
	vTexCo = vec2( 0.5f * projPos.x + 0.5f, 0.5f * projPos.y + 0.5f );  // quad 的貼圖軸
}
