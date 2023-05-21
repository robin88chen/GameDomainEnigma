struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 Coord : TEXCOORD0;
};
struct VS_OUTPUT
{
    float4 Pos : POSITION;
	float2 Coord : TEXCOORD0;
	float2 ProjPos : TEXCOORD1;
};

VS_OUTPUT vs_main( VS_INPUT input )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = input.Pos;
	output.Coord = input.Coord;
	output.ProjPos = input.Pos;
    return output;
}
