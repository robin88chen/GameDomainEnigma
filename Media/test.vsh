float offset_x;
float offset_y;
struct VS_INPUT
{
   float3 Pos : POSITION;
   float2 Coord : TEXCOORD0;
};
struct VS_OUTPUT
{
   float4 Pos : SV_POSITION;
   float2 Coord : TEXCOORD0;
   float4 DiffCol : COLOR0;
};
VS_OUTPUT vs_main(const VS_INPUT v)
{
   VS_OUTPUT o = (VS_OUTPUT)0;
   o.Pos.xyz = v.Pos + float3(offset_x, offset_y, 0.0f);
 o.Pos.w = 1.0f;
   o.Coord = v.Coord;
   o.DiffCol = float4(0.5f + v.Pos.x + v.Pos.y, 0.5f + v.Pos.x - v.Pos.y, 0.5f + v.Pos.y - v.Pos.x, 1.0f);
   return o;
};

