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
	float2 ProjPos : TEXCOORD1;
};

VS_OUTPUT vs_main( VS_INPUT input )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float4 tempPos = mul( input.Pos, World);
    tempPos = mul(tempPos, View);
    tempPos = mul(tempPos, Projection);
    output.Pos = tempPos;
	float2 projPos = tempPos.xy / tempPos.w;
	output.Coord = float2( 0.5f * projPos.x + 0.5f, -0.5f * projPos.y + 0.5f );  // quad 的貼圖軸
	output.ProjPos = projPos;
    return output;
}
