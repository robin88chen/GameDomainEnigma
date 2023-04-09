//semantic World World
//semantic View View
//semantic Projection Projection
//semantic BoneAnimation BoneMatrix
matrix World : World;
matrix View : View;
matrix Projection : Projection;
matrix BoneAnimation[20] : BoneMatrix;
struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Nor : NORMAL;
	float4 Weight : BLENDWEIGHT;
	uint bone_idx : BLENDINDICES;
	float2 Coord : TEXCOORD0;
};
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Nor : TEXCOORD0;
	float2 Coord : TEXCOORD1;
	float Depth : TEXCOORD2;
};
VS_OUTPUT vs_main(const VS_INPUT v)
{
	VS_OUTPUT o = (VS_OUTPUT)0;
    float WeightsArray[4] = (float[4])v.Weight;
	uint	  IndexArray[4] = (uint[4])v.bone_idx;
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
    o.Pos = mul( float4(v.Pos, 1.0f), mx);
    o.Pos = mul( o.Pos, World);
    o.Pos = mul( o.Pos, View );
    o.Depth = o.Pos.z;
    o.Pos = mul( o.Pos, Projection );
	o.Coord = v.Coord;
	o.Nor = mul( v.Nor, mx);
	o.Nor = mul( o.Nor, World);
	return o;
};

