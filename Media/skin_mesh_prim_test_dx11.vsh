//semantic World World
//semantic View View
//semantic Projection Projection
//semantic BoneAnimation BoneMatrix
matrix World : World;
matrix View : View;
matrix Projection : Projection;
matrix BoneAnimation[8] : BoneMatrix;
struct VS_INPUT
{
	float4 Pos : POSITION;
    float3 Nor : NORMAL;
	float Weight : BLENDWEIGHT;
	uint bone_idx : BLENDINDICES;
	float2 Coord : TEXCOORD0;
};
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Nor : TEXCOORD0;
    float2 Coord : TEXCOORD1;
    float4 WorldPos : TEXCOORD2;
};
VS_OUTPUT vs_main(const VS_INPUT v)
{
    VS_OUTPUT o = (VS_OUTPUT)0;
    uint id0 = v.bone_idx & 255;
    uint id1 = (v.bone_idx >> 8) & 255;
    matrix mx = mul(v.Weight , BoneAnimation[id0]) + mul((1.0f - v.Weight) , BoneAnimation[id1]);
    o.Pos = mul(v.Pos, mx); //BoneAnimation[bone_idx] );
    o.Pos = mul(o.Pos, World);
    o.WorldPos = o.Pos;
    o.Pos = mul( o.Pos, View );
    o.Pos = mul( o.Pos, Projection );
    o.Coord = v.Coord;
    o.Nor = mul(v.Nor, World);
    return o;
};

