#include "../include/model_asset.h"

dbasic::ModelAsset::ModelAsset() : ysObject("MODEL_ASSET") {
    m_name[0] = '\0';
    m_material = nullptr;

    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;

    m_baseVertex = 0;
    m_baseIndex = 0;

    m_faceCount = 0;
    m_vertexCount = 0;
    m_UVChannelCount = 0;

    m_vertexSize = 0;

    m_manager = nullptr;
}

dbasic::ModelAsset::~ModelAsset() {
    /* void */
}
