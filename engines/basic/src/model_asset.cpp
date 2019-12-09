#include "../include/model_asset.h"

dbasic::ModelAsset::ModelAsset() : ysObject("MODEL_ASSET") {
    m_defaultMaterial = NULL;

    m_vertexBuffer = NULL;
    m_indexBuffer = NULL;

    m_baseVertex = 0;
    m_baseIndex = 0;

    m_faceCount = 0;
    m_vertexCount = 0;
    m_UVChannelCount = 0;

    m_vertexSize = 0;

    m_manager = NULL;
}

dbasic::ModelAsset::~ModelAsset() {
    /* void */
}
