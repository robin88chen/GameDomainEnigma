#version 300 es
//semantic World World
//semantic View View
//semantic Projection Projection
//semantic BoneAnimation[0] BoneMatrix
layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 weight;
layout(location = 2) in uint bone_idx;
layout(location = 3) in vec3 nor;
layout(location = 4) in vec2 texco;
uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 BoneAnimation[16];
out vec3 vNor;
out vec2 vTexCo;
out vec4 vWorldPos;
//const uint bit_mask = 255;
void main() {
	//uint bit_mask = 255U;
    uint id0 = bone_idx & 255U; //bit_mask;
    uint id1 = (bone_idx>>8) & 255U; //bit_mask;
    uint id2 = (bone_idx>>16) & 255U;
    uint id3 = (bone_idx>>24) & 255U;
    mat4 mx = weight.x * BoneAnimation[id0];
    if (id1 != 255U)
    {
        mx = mx + weight.y * BoneAnimation[id1];
    }
    if (id2 != 255U)
    {
        mx = mx + weight.z * BoneAnimation[id2];
    }
    if (id3 != 255U)
    {
        mx = mx + weight.w * BoneAnimation[id3];
    }
	vec4 tpos = vec4(pos, 1.0) * mx;
	tpos = tpos * World;
    vWorldPos = tpos;
	tpos = tpos * View;
	gl_Position = tpos * Projection;
    vTexCo = texco;
    mat4 nmx = mx * World;
    nmx = inverse(nmx);
    nmx = transpose(nmx);
    vec4 tnor = vec4(nor, 0.0) * nmx;
    vNor = normalize(tnor.xyz);
}
