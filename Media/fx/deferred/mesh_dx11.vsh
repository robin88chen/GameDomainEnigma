//semantic World World
//semantic View View
//semantic Projection Projection
//semantic wvInvTranspose WorldViewInvTranspose
matrix World : World;
matrix View : View;
matrix Projection : Projection;
matrix wvInvTranspose : WorldViewInvTranspose;
struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Nor : NORMAL;
	float2 Coord : TEXCOORD0;
};
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Nor : TEXCOORD0;  // view space normal
	float2 Coord : TEXCOORD1;
	float Depth : TEXCOORD2;
};
VS_OUTPUT vs_main(const VS_INPUT v)
{
	VS_OUTPUT o = (VS_OUTPUT)0;
    o.Pos = mul( v.Pos, World);
    o.Pos = mul( o.Pos, View );
    o.Depth = o.Pos.z;
    o.Pos = mul( o.Pos, Projection );
	o.Coord = v.Coord;
	o.Nor = mul( v.Nor, wvInvTranspose);
	return o;
};

