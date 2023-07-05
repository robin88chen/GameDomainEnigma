//semantic World World
//semantic View View
//semantic Projection Projection
matrix World : World;
matrix View : View;
matrix Projection : Projection;
struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Nor : NORMAL;
	float2 Coord : TEXCOORD0;
    float2 AlphaCoord : TEXCOORD1;
};
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Nor : TEXCOORD0;
	float2 Coord : TEXCOORD1;
	float2 AlphaCoord : TEXCOORD2;
    float Depth : TEXCOORD3;
};
VS_OUTPUT vs_main(const VS_INPUT v)
{
	VS_OUTPUT o = (VS_OUTPUT)0;
    o.Pos = mul( v.Pos, World);
    o.Pos = mul( o.Pos, View );
    o.Depth = o.Pos.z;
    o.Pos = mul( o.Pos, Projection );
	o.Coord = v.Coord;
    o.AlphaCoord = v.AlphaCoord;
	o.Nor = mul( v.Nor, World);
    o.Nor = mul( o.Nor, View);
	return o;
};

