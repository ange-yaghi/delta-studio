Texture2D txNormal : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_OUTPUT
{

	float4 Pos : SV_POSITION;
	float4 VertexPosition : VEC_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;

};

struct VS_INPUT
{

	float3 Pos : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	float4 Tangent : TANGENT;

};

cbuffer ConstantBuffer : register(b0)
{

	matrix World;
	matrix View;
	matrix Projection;

	float4 Eye;

	float4 Color;
	float4 Offset;
	float Scale;
	int AllWhiteLight;

};

cbuffer ObjectData : register(b2)
{

	matrix ObjectTransform;

	// Material
	float4 DiffuseColor;

	float4 FalloffColor;
	float FalloffPower;

	int UseDiffuseMap;
	int UseSpecularMap;
	int UseFalloff;

	float Specular;
	float SpecularPower;

	float Ambient;

	int Lit;

};

VS_OUTPUT VS(VS_INPUT input)
{

	float4 inputPos = float4(input.Pos, 1.0);
	
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.Pos = inputPos;

	output.Pos = mul(output.Pos, ObjectTransform);

	output.Pos = mul(output.Pos, World); output.VertexPosition = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	matrix <float, 3, 3> rot_only = ObjectTransform;
	
	output.Tex = float2(input.TexCoord.x, 1 - input.TexCoord.y);
	//output.Normal = mul(input.Normal, ObjectTransform);
	output.Normal = normalize(input.Normal);

	output.Tangent = input.Tangent.xyz;
	
	float3 t = input.Tangent.xyz;
	float3 b = cross(t, input.Normal) * input.Tangent.w;
	float3 n = output.Normal;

	output.Bitangent = b;

	return output;

}

float4 PS(VS_OUTPUT input) : SV_Target
{

	float3x3 TBN = {input.Tangent, input.Bitangent, input.Normal};
	TBN = transpose(TBN);

	float3 viewVec = normalize(input.VertexPosition - Eye);

	float3 LightPos = float3(20, 20, 20);
	float3 lightVec = normalize(LightPos - input.VertexPosition);

	float3 normal = mul(TBN, normalize(txNormal.Sample( samLinear, input.Tex ).rgb));
	float diffuse = dot(normal, lightVec);

	float3 reflectVec = -normalize(2.0 * dot(normal, lightVec) * normal - lightVec);
	float spec = max(dot(reflectVec, viewVec), 0.0);
	spec = pow(spec, 8.0);

	return float4(diffuse * float3(0.5, 0.5, 0.5) + spec * float3(0.5, 0.5, 0.5), 1.0);

}