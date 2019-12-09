#ifndef DELTA_BASIC_SHADER_CONTROLS_H
#define DELTA_BASIC_SHADER_CONTROLS_H

#include "delta_core.h"

struct Vertex {
    ysVector4 Pos = { 0.0f, 0.0f, 0.0f, 0.0f };
    ysVector2 TexCoord = { 0.0f, 0.0f };
    ysVector4 Normal = { 0.0f, 0.0f, 0.0f, 0.0f };
};

struct ShaderObjectVariables {
    ysMatrix Transform = ysMath::LoadIdentity();

    ysVector4 MulCol = { 1.0f, 1.0f, 1.0f, 1.0f };
    float TexOffset[2] = { 0.0f, 0.0f };
    float TexScale[2] = { 1.0f, 1.0f };
    float Scale[3] = { 1.0f, 1.0f, 1.0f };
    int ColorReplace = 0;
    int Lit = 1;

    int Pad[3] = { 0, 0, 0 };
};

struct ShaderScreenVariables {
    ysMatrix CameraView = ysMath::LoadIdentity();
    ysMatrix Projection = ysMath::LoadIdentity();

    float Eye[3] = { 0.0f, 0.0f, 0.0f };
};

struct ShaderSkinningControls {
    ysMatrix BoneTransforms[256];
};

#endif /* DELTA_BASIC_SHADER_CONTROLS_H */
