//semantic World World
//semantic View View
//semantic Projection Projection
matrix World : World;
matrix View : View;
matrix Projection : Projection;
struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 Coord : TEXCOORD0;
};
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Coord : TEXCOORD0;
};
VS_OUTPUT vs_main(const VS_INPUT v)
{
	VS_OUTPUT o = (VS_OUTPUT)0;
    o.Pos = mul( v.Pos, World);
    o.Pos = mul( o.Pos, View );
    o.Pos = mul( o.Pos, Projection );
	o.Coord = v.Coord;
	return o;
};

