//semantic World World
//semantic View View
//semantic Projection Projection
matrix World : World;
matrix View : View;
matrix Projection : Projection;
struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Nor : NORMAL;
	float2 Coord : TEXCOORD0;
};
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float3 Nor : TEXCOORD0;
	float2 Coord : TEXCOORD1;
	float4 WorldPos : TEXCOORD2;
};
VS_OUTPUT vs_main(const VS_INPUT v)
{
	VS_OUTPUT o = (VS_OUTPUT)0;
    o.Pos = mul( float4(v.Pos, 1.0), World);
	o.WorldPos = o.Pos;
	//o.Pos = mul( v.Pos, World);
    o.Pos = mul( o.Pos, View );
    o.Pos = mul( o.Pos, Projection );
	o.Coord = v.Coord;
	o.Nor = mul( v.Nor, World);
	return o;
};

