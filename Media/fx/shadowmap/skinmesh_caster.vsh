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
	float2 Depth : TEXCOORD0;
};
VS_OUTPUT vs_main(const VS_INPUT v)
{
	VS_OUTPUT o = (VS_OUTPUT)0;
    float WeightsArray[4] = (float[4])v.Weight;
	uint	  IndexArray[4] = (uint[4])v.bone_idx; //{ (v.bone_idx & 255), ((v.bone_idx >> 8) & 255), ((v.bone_idx >> 16) & 255), ((v.bone_idx >> 24) & 255) };
	uint idx = IndexArray[0]; //v.bone_idx & 255;

    matrix mx = mul( WeightsArray[0] , BoneAnimation[idx]); //IndexArray[0]] );
	idx = IndexArray[1]; //(v.bone_idx >> 8) & 255;
	if  (idx != 255) //(IndexArray[1]!=255)
	{
		mx = mx + mul( WeightsArray[1] , BoneAnimation[idx]); //IndexArray[1]] );
	}
	if (IndexArray[2]!=255)
	{
		mx = mx + mul( WeightsArray[2] , BoneAnimation[IndexArray[2]] );
	}
	if (IndexArray[3]!=255)
	{
		mx = mx + mul( WeightsArray[3] , BoneAnimation[IndexArray[3]] );
	}
    o.Pos = mul( float4(v.Pos, 1.0f), mx); //BoneAnimation[bone_idx] );
    o.Pos = mul( o.Pos, World);
	//o.Pos = mul( v.Pos, World);
    o.Pos = mul( o.Pos, View );
    o.Pos = mul( o.Pos, Projection );
	o.Depth.xy = o.Pos.zw;
	return o;
};

