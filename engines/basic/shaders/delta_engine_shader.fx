Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_OUTPUT {
	float4 Pos : SV_POSITION;
	float4 VertexPosition : VERTEX_POSITION;
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
	float4 CameraEye;
};

cbuffer ObjectVariables : register(b1) {
	matrix Transform;
	float4 MulCol;
	float2 TexOffset;
	float2 TexScale;
	float3 Scale;
	int ColorReplace;
	int Lit;
};

cbuffer SkinningVariables : register(b2) {
	matrix BoneTransform[256];
};

struct Light {
	float4 Position;
	float4 Color;
	float4 Direction;
	float Attenuation0;
	float Attenuation1;
	int FalloffEnabled;
	int Active;
};

cbuffer Lighting : register(b3) {
	Light Lights[32];
	float4 AmbientLighting;
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
	
	inputPos.xyz *= Scale;
	inputPos = mul(inputPos, Transform);
	output.VertexPosition = inputPos;

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

float pow5(float v) {
	return (v * v) * (v * v) * v;
}

float f_diffuse(float3 i, float3 o, float3 h, float3 normal, float power, float roughness) {
	float h_dot_i = dot(h, i);
	float h_dot_i_2 = h_dot_i * h_dot_i;
	float f_d90 = 0.5 + 2 * h_dot_i_2 * roughness;

	float cos_theta_i = dot(i, normal);
	float cos_theta_o = dot(o, normal);

	float f_d = (1 + (f_d90 - 1) * pow5(1 - cos_theta_i)) * (1 + (f_d90 - 1) * pow5(1 - cos_theta_o));
	return f_d * power * cos_theta_i;
}

float f_specular(float3 i, float3 o, float3 h, float3 normal, float power, float specularPower) {
	float reflected = -reflect(i, normal);
	float intensity = dot(reflected, o);

	if (intensity < 0) return 0;

	// Fresnel approximation
	float F0 = 0.08;
	float o_dot_h = dot(o, h);
	float s = pow5(1 - o_dot_h);
	float F = F0 + s * (1 - F0);

	return pow(intensity, specularPower) * F * power;
}

float f_specular_ambient(float3 o, float3 normal, float F0) {
	float reflected = reflect(-o, normal);

	// Fresnel approximation
	float F0_scaled = 0.08 * F0;
	float o_dot_n = dot(o, normal);
	float s = pow5(1 - o_dot_n);
	float F = F0_scaled + s * (1 - F0_scaled);

	return F;
}

float4 PS(VS_OUTPUT input) : SV_Target {
	float3 totalLighting = float3(1.0, 1.0, 1.0);

	float4 baseColor;
	float roughness = 0.5;
	float power = 1.0;

	if (ColorReplace == 0) {
		baseColor = txDiffuse.Sample(samLinear, input.TexCoord).rgba * MulCol;
	}
	else {
		baseColor = MulCol;
	}

	totalLighting = baseColor.rgb;

	if (Lit == 1) {
		float3 o = normalize(CameraEye.xyz - input.VertexPosition.xyz);

		float3 ambientSpecular = f_specular_ambient(o, input.Normal, 0.0) * AmbientLighting;
		float3 ambientDiffuse = f_diffuse(o, o, o, input.Normal, 1.0, 0.5) * AmbientLighting * baseColor;

		totalLighting = ambientSpecular + ambientDiffuse;
		for (int li = 0; li < 32; ++li) {
			const Light light = Lights[li];
			if (light.Active == 0) continue;

			float3 i = light.Position.xyz - input.VertexPosition.xyz;
			float inv_mag = 1.0 / length(i);
			i *= inv_mag;

			float cos_theta_i = dot(i, input.Normal);
			float cos_theta_o = dot(o, input.Normal);

			if (cos_theta_i < 0) continue;
			if (cos_theta_o < 0) continue;

			float3 h = normalize(i + o);
			float3 diffuse = f_diffuse(i, o, h, input.Normal, 1.0, 0.5) * baseColor.rgb * light.Color;
			float3 specular = f_specular(i, o, h, input.Normal, 1.0, 4.0) * light.Color;

			// Spotlight calculation
			float spotCoherence = -dot(i, light.Direction.xyz);
			float spotAttenuation = 1.0f;
			if (spotCoherence > light.Attenuation0) spotAttenuation = 1.0f;
			else if (spotCoherence < light.Attenuation1) spotAttenuation = 0.0f;
			else {
				float t = light.Attenuation0 - light.Attenuation1;
				if (t == 0) spotAttenuation = 1.0f;
				else spotAttenuation = (spotCoherence - light.Attenuation1) / t;
			}
			spotAttenuation = 1.0;

			float falloff = 1.0;
			if (light.FalloffEnabled == 1) {
				falloff = (inv_mag * inv_mag);
			}

			totalLighting += falloff * (diffuse + specular) * spotAttenuation * spotAttenuation * spotAttenuation;
		}
	}
	
	return float4(totalLighting.rgb, 1.0);
}
