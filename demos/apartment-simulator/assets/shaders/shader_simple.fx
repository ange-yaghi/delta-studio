Texture2D txDiffuse : register( t0 );
Texture2D txSpecular : register( t1 );
Texture2D txNormal : register ( t2 );
Texture2D txReflect : register (t3) ;
SamplerState samLinear : register( s0 );

struct VS_OUTPUT {
	float4 Pos : SV_POSITION;
	float4 VertexPosition : VEC_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
};

struct VS_INPUT {
	float4 Pos : POSITION;
	float2 TexCoord : TEXCOORD0;
	float4 Normal : NORMAL;
};

struct VS_INPUT_TANGENT {
	float4 Pos : POSITION;
	float2 TexCoord : TEXCOORD0;
	float4 Normal : NORMAL;
	float4 Tangent : TANGENT;
};

cbuffer ConstantBuffer : register(b0) {
	matrix World;
	matrix View;
	matrix Projection;

	float4 Eye;

	float4 Color;
	float4 Offset;
	float Scale;
	int AllWhiteLight;
};

struct Light {
	float4 Location;
	float4 Color;
	float4 LowColor;
	float4 Direction;
	float4 AuxCutoffDirection;
	float AuxCutoffAmount;

	float CutoffHigh;
	float CutoffLow;
	float CutoffPower;
	float CutoffMix;
	int CutoffAmbient;

	float Falloff;
	float Intensity;

	float Ambient;
	float Diffuse;

	float Specular;
	float SpecularPower;
};

struct PlaneShadow {
	float4 Location;
	float4 FringeMask;
	float ShadowIntensity;
	float Width;
	float Depth;
	float Height;
	float Falloff;
};

cbuffer LightBuffer : register(b1) {
	int NumLights;
	int NumShadows;

	Light Lights[16];
	PlaneShadow Shadows[16];
};

cbuffer ObjectData : register(b2) {
	matrix ObjectTransform;

	// Material
	float4 DiffuseColor;

	float4 FalloffColor;
	float FalloffPower;

	int UseDiffuseMap;
	int UseSpecularMap;
	int UseNormalMap;
	int UseFalloff;
	int UseAlpha;
	int UseReflectMap;

	float Specular;
	float SpecularPower;
	float ReflectPower;

	float Ambient;

	int Lit;
	int LightExclusion[16];
};

VS_OUTPUT VS_TANGENT(VS_INPUT_TANGENT input) {
	float4 inputPos = input.Pos; //float4(input.Pos, 1.0);
	
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.Pos = inputPos;

	output.Pos = mul(output.Pos, ObjectTransform);

	output.Pos = mul(output.Pos, World); output.VertexPosition = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	
	output.Tex = float2(input.TexCoord.x, 1 - input.TexCoord.y);
	output.Normal = normalize(mul(input.Normal, ObjectTransform));
	
	float3 t = mul(input.Tangent.xyz, ObjectTransform);
	float3 b = mul(cross(input.Tangent.xyz, input.Normal.xyz) * input.Tangent.w, ObjectTransform);

	output.Tangent = normalize(t);
	output.Bitangent = normalize(b);

	return output;
}

VS_OUTPUT VS(VS_INPUT input) {
	float4 inputPos = input.Pos; //float4(input.Pos, 1.0);
	
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.Pos = inputPos;

	output.Pos = mul(output.Pos, ObjectTransform);

	output.Pos = mul(output.Pos, World); output.VertexPosition = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	matrix <float, 3, 3> rot_only = ObjectTransform;
	
	output.Tex = float2(input.TexCoord.x, 1 - input.TexCoord.y);
	output.Normal = normalize(mul(input.Normal, ObjectTransform));

	output.Tangent = float3(0, 0, 0);
	output.Bitangent = float3(0, 0, 0);
	//output.Normal = input.Normal;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target {
	if (Lit == 1) {
		float3 totalDiffuse = float3(0, 0, 0);
		float3 totalAmbient = float3(0, 0, 0);
		float3 totalSpecular = float3(0, 0, 0);

		float3 viewVec = normalize(input.VertexPosition - Eye);

		float3 normal = input.Normal;
		
		if (UseNormalMap == 1) {
			float3x3 TBN = {input.Tangent, input.Bitangent, input.Normal};
			TBN = transpose(TBN);

			normal = (txNormal.Sample( samLinear, input.Tex ));
			normal.xy = 2 * normal.xy - float2(1.0, 1.0);
			normal = mul(TBN, normal);
			normal = normalize(normal);
		}

		float4 diffuseColor = DiffuseColor;
		float4 specularMap = float4(Specular, Specular, Specular, 1.0);

		if (UseReflectMap == 1) {
			float3 r = -normalize(2.0 * dot(normal, -viewVec) * normal + viewVec);
			r.z = abs(r.z);

			float m = 2 * sqrt(r.x*r.x + r.y * r.y + (r.z + 1)*(r.z + 1));
			float s = r.x/m + 0.5;
			float t = r.y/m + 0.5;

			diffuseColor *= pow(txReflect.Sample( samLinear, float2(s, t) ), ReflectPower);
		}

		if (UseDiffuseMap == 1) {
			float4 diffuseMap = txDiffuse.Sample( samLinear, input.Tex );
			diffuseColor *= diffuseMap;
		}

		for(int i = 0; i < NumLights; i++) {
			float3 lightVec = Lights[i].Location - input.VertexPosition.xyz;

			float auxDistance = -dot(lightVec, Lights[i].AuxCutoffDirection);
			auxDistance = max(auxDistance, 0.0) *  Lights[i].AuxCutoffAmount;

			float distance = length(lightVec) + auxDistance;
			float falloff = 1.0 / pow(2.0, distance * (1.0/Lights[i].Falloff));

			lightVec = lightVec / distance;
			float3 reflectVec = -normalize(2.0 * dot(normal, lightVec) * normal - lightVec);
			float diffuse = max(dot(lightVec, normal), 0.0);

			float cutoff = 1.0;

			if (Lights[i].CutoffLow < 1 || Lights[i].CutoffHigh < 1) {
				float cutoffDot = dot(lightVec, -Lights[i].Direction.xyz);
				float midway = (-Lights[i].CutoffLow + Lights[i].CutoffHigh) / 2;
	
				cutoff = clamp(abs((cutoffDot - midway) / (midway - Lights[i].CutoffHigh)), 0.0, 1.0);

				cutoff = pow(cutoff, Lights[i].CutoffPower);
				cutoff = Lights[i].CutoffMix * (1 - cutoff) + (1 - Lights[i].CutoffMix) * cutoff;
			}

			float3 color = lerp(Lights[i].LowColor, Lights[i].Color, cutoff);
			if (AllWhiteLight == 1) color = float3(1.0, 1.0, 1.0);

			totalDiffuse += diffuse * color * Lights[i].Intensity * falloff * Lights[i].Diffuse;	

			if (Lights[i].CutoffAmbient == 1) totalAmbient += falloff * color * Lights[i].Ambient * Lights[i].Intensity;
			else totalAmbient += falloff * Lights[i].Color * Lights[i].Ambient * Lights[i].Intensity;

			
			if (Specular > 0.01) {
				float spec = max(dot(reflectVec, viewVec), 0.0);
				spec = pow(spec, SpecularPower * Lights[i].SpecularPower);
				totalSpecular += pow(falloff, 0.25) *  color * Lights[i].Specular * Lights[i].Intensity * spec;
			}
			//totalDiffuse = float3(1.0, 0.0, 0.0);
		}

		//return float4(noise, noise, noise, 1.0);

		//return float4(NumShadows / 16.0, 0, 0, 1.0);
		float shadow = 1.0;
		for(int i = 0; i < NumShadows; i++) {
			float3 lightVec = input.VertexPosition.xyz - Shadows[i].Location;
			float3 fringe = abs(lightVec);
			fringe = max(fringe, float3(Shadows[i].Width, Shadows[i].Height, Shadows[i].Depth)) - float3(Shadows[i].Width, Shadows[i].Height, Shadows[i].Depth);

			float totalDistance = length(fringe * Shadows[i].FringeMask.xyz);
			float falloff = (1 / pow(2.0, totalDistance * (1.0/Shadows[i].Falloff))) * Shadows[i].ShadowIntensity;

			shadow += falloff;

			//return float4(falloff, falloff, falloff, 1.0);
		}

		totalDiffuse /= shadow;
		totalAmbient /= shadow;
		
		if (UseFalloff == 1) {
			float angleFalloff = pow(1.01 - max(dot(-viewVec, input.Normal), 0.0), FalloffPower);
			diffuseColor = (angleFalloff) * FalloffColor + (1 - angleFalloff) * diffuseColor;
			//return float4(angleFalloff, angleFalloff, angleFalloff, 1.0);
		}

		if (UseSpecularMap == 1) {
			specularMap *= txSpecular.Sample(samLinear, input.Tex);
		}

		//diffuseColor = float4(1.0, 1.0, 1.0, 1.0);

		//return diffuseColor;
		float4 finalColor = diffuseColor * float4(totalDiffuse + totalAmbient * Ambient * 1.0, 0.0) + float4(totalSpecular * specularMap, 1.0);

		//return diffuseColor;
		//return float4(0.5*(normal + float3(1.0, 1.0, 1.0)), 1.0);
		return pow(finalColor, 1);

		//return float4(length(input.Normal), length(input.Normal), length(input.Normal), 1.0);
		//return float4(0.5*(input.Normal + float3(1.0, 1.0, 1.0)), 1.0f);
		//if (RenderTechnique == 2) return float4(txDiffuse.Sample( samLinear, input.Tex ).rgb * (totalDiffuse + totalAmbient + totalSpecular), 1.0);
		//else if (RenderTechnique == 1) return float4((totalDiffuse + totalAmbient + totalSpecular), 1.0);
		//return float4((totalDiffuse + totalAmbient + totalSpecular), 1.0);
		//return float4(totalAmbient + totalSpecular, 1.0);
		//return float4(totalSpecular, 1.0);
		//return float4(totalDiffuse + totalAmbient + totalSpecular, 1.0);
	}
	else {
		if (UseDiffuseMap) {
			return float4(txDiffuse.Sample( samLinear, input.Tex ).rgb, 1.0);
		}
		else if (UseFalloff == 1) {
			
			float3 viewVec = normalize(input.VertexPosition - Eye);
			float angleFalloff = pow(1.01 - max(dot(-viewVec, input.Normal), 0.0), FalloffPower);

			return float4(((angleFalloff) * FalloffColor + (1 - angleFalloff) * DiffuseColor).rgb, 1.0);
		}
		else return float4(DiffuseColor.rgb, 1.0);
	}
}
