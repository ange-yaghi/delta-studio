Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_OUTPUT {
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;

	float BoneWeight : BONE_WEIGHT;
};

struct VS_INPUT_SKINNED {
	float4 Pos : POSITION;
	float2 TexCoord : TEXCOORD0;
	float4 Normal : NORMAL;

	int4 BoneIndices : BONE_INDICES;
	float4 BoneWeights : BONE_WEIGHTS;
};

struct VS_INPUT_STANDARD {
	float4 Pos : POSITION;
	float2 TexCoord : TEXCOORD0;
	float4 Normal : NORMAL;
};

cbuffer ScreenVariables : register(b0) {
	matrix CameraView;
	matrix Projection;
};

cbuffer ObjectVariables : register(b1) {
	matrix Transform;
	float4 MulCol;
	float2 TexOffset;
	float2 TexScale;
	float3 Scale;
	int ColorReplace;
};

cbuffer SkinningVariables : register(b2) {
	matrix BoneTransform[256];
};

VS_OUTPUT VS_SKINNED(VS_INPUT_SKINNED input) {
	VS_OUTPUT output = (VS_OUTPUT) 0;

	float4 inputPos = float4(input.Pos.xyz, 1.0);

	float4 final = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 finalNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);

	output.Normal = mul(input.Normal, Transform);
	inputPos = mul(inputPos, Transform);

	if (input.BoneIndices.x >= 0) {
		final += mul(BoneTransform[input.BoneIndices.x], inputPos) * input.BoneWeights.x;
		finalNormal += mul(BoneTransform[input.BoneIndices.x], output.Normal) * input.BoneWeights.x;

		if (input.BoneIndices.y >= 0) {
			final += mul(BoneTransform[input.BoneIndices.y], inputPos) * input.BoneWeights.y;
			finalNormal += mul(BoneTransform[input.BoneIndices.y], output.Normal) * input.BoneWeights.y;

			if (input.BoneIndices.z >= 0) {
				final += mul(BoneTransform[input.BoneIndices.z], inputPos) * input.BoneWeights.z;
				finalNormal += mul(BoneTransform[input.BoneIndices.z], output.Normal) * input.BoneWeights.z;
			}
		}
	}
	else {
		final = inputPos;
		finalNormal = float4(output.Normal, 1.0);
	}

	output.BoneWeight = 0.0;

	inputPos = float4(final.xyz, 1.0);
	
	inputPos = mul(inputPos, Transform);
	inputPos.xyz *= Scale;
	inputPos = mul(inputPos, CameraView);
	inputPos = mul(inputPos, Projection);

	output.Pos = float4(inputPos.xyzw);

	input.TexCoord.y = 1 - input.TexCoord.y;
	output.TexCoord = ( (input.TexCoord) * TexScale) + TexOffset;
	output.TexCoord = float2(output.TexCoord.x, output.TexCoord.y);

	output.Normal = finalNormal;

	return output;
}

VS_OUTPUT VS_STANDARD(VS_INPUT_STANDARD input) {
	float4 inputPos = float4(input.Pos);
	
	VS_OUTPUT output = (VS_OUTPUT) 0;

	float4 final = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 finalNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);

	output.Normal = mul(input.Normal, Transform);

	final = inputPos;
	finalNormal = float4(output.Normal, 1.0);

	inputPos = float4(final.xyz, 1.0);

	//inputPos.z *= Size.x;
	
	inputPos.xyz *= Scale;
	inputPos = mul(inputPos, Transform);
	inputPos = mul(inputPos, CameraView);
	inputPos = mul(inputPos, Projection);

	output.Pos = float4(inputPos.xyzw);

	input.TexCoord.y = 1 - input.TexCoord.y;
	output.TexCoord = ( (input.TexCoord) * TexScale) + TexOffset;
	output.TexCoord = float2(output.TexCoord.x, output.TexCoord.y);

	output.Normal = finalNormal;
	output.BoneWeight = 0.0;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target {
	//return float4((input.Normal + float3(1.0, 1.0, 1.0))/2.0, 1.0);
	//return float4(input.BoneWeight, 0.0, 0.0, 1.0);
	//return float4(input.TexCoord, 1.0, 1.0);
	//return float4(input.Pos.xyz/1920.0f, 1.0);
	
	if (ColorReplace == 0) {
		return float4(txDiffuse.Sample( samLinear, input.TexCoord ).rgba * MulCol);
	}
	else {
		//return float4((input.Normal + float3(1.0, 1.0, 1.0))/2.0, 1.0);
		return MulCol;
	}
}
