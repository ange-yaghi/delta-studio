Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_OUTPUT
{

	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;

};

struct VS_INPUT
{

	float3 Pos : POSITION;
	float2 TexCoord : TEXCOORD0;

};

cbuffer ConstantBuffer : register(b0)
{

	float BoxLeft;
	float BoxRight;
	float BoxTop;
	float BoxBottom;
	float4 BoxColor;
	int UseTexture;

};

VS_OUTPUT VS(VS_INPUT input)
{

	float4 inputPos = float4(input.Pos, 1.0);
	
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.Tex = float2(input.TexCoord.x, 1 - input.TexCoord.y);

	float2 size = float2(BoxRight - BoxLeft, BoxTop - BoxBottom);
	float2 middle = float2((BoxRight + BoxLeft) / 2, (BoxBottom + BoxTop) / 2);
	float2 scale = size / float2(2.0, 2.0);

	output.Pos = (inputPos * float4(scale, 1.0, 1.0)) + float4(middle, 0.0, 0.0);

	return output;

}

float4 PS(VS_OUTPUT input) : SV_Target
{

	if (UseTexture == 1) return float4(txDiffuse.Sample( samLinear, input.Tex ).rgb, 1.0f);
	else return BoxColor;

}