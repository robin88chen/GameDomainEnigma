//semantic World World
//semantic View View
//semantic Projection Projection
//semantic BoneAnimation BoneMatrix
matrix World : World;
matrix View : View;
matrix Projection : Projection;
matrix BoneAnimation[32] : BoneMatrix;
struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Nor : NORMAL;
	float4 Weight : BLENDWEIGHT;
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
    float WeightsArray[4] = (float[4])v.Weight;
	uint	  IndexArray[4] = { (v.bone_idx & 255), ((v.bone_idx >> 8) & 255), ((v.bone_idx >> 16) & 255), ((v.bone_idx >> 24) & 255) };
	//uint	  IndexArray[4] = (uint[4])v.bone_idx;
	uint idx = IndexArray[0];

    matrix mx = mul( WeightsArray[0] , BoneAnimation[idx]);
	idx = IndexArray[1]; 
	if  (idx != 255) 
	{
		mx = mx + mul( WeightsArray[1] , BoneAnimation[idx]); 
	}
	if (IndexArray[2]!=255)
	{
		mx = mx + mul( WeightsArray[2] , BoneAnimation[IndexArray[2]] );
	}
	if (IndexArray[3]!=255)
	{
		mx = mx + mul( WeightsArray[3] , BoneAnimation[IndexArray[3]] );
	}
    o.Pos = mul( v.Pos, mx);
    o.Pos = mul( o.Pos, World);
	o.WorldPos = o.Pos;
	//o.Pos = mul( v.Pos, World);
    o.Pos = mul( o.Pos, View );
    o.Pos = mul( o.Pos, Projection );
	o.Coord = v.Coord;
	o.Nor = mul( v.Nor, mx);
	o.Nor = mul( o.Nor, World);
	return o;
};

