#include "../include/ui_renderer.h"

#include "../include/delta_basic_engine.h"

dbasic::UiRenderer::UiRenderer() {
    m_engine = nullptr;
    m_bufferSize = 0;
    m_vertexOffset = 0;
}

dbasic::UiRenderer::~UiRenderer() {
    /* void */
}

ysError dbasic::UiRenderer::Initialize(int bufferSize) {
    YDS_ERROR_DECLARE("Initialize");

    YDS_NESTED_ERROR_CALL(InitializeGeometry(bufferSize));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::UiRenderer::Update() {
    YDS_ERROR_DECLARE("UpdateDisplay");

    if (m_vertexOffset == 0) return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);

    m_engine->GetDevice()->EditBufferDataRange(
        m_mainVertexBuffer,
        (char *)m_vertexBuffer,
        sizeof(ConsoleVertex) * m_vertexOffset,
        0);
    m_engine->GetDevice()->EditBufferDataRange(
        m_mainIndexBuffer,
        (char *)m_indexBuffer,
        sizeof(unsigned short) * m_indexOffset,
        0);

    m_engine->GetDevice()->UseIndexBuffer(m_mainIndexBuffer, 0);
    m_engine->GetDevice()->UseVertexBuffer(m_mainVertexBuffer, sizeof(ConsoleVertex), 0);

    m_engine->GetDevice()->UseTexture(m_font->GetTexture(), 0);

    m_engine->GetDevice()->Draw(m_indexOffset / 3, 0, 0);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::UiRenderer::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    m_engine->GetDevice()->DestroyGPUBuffer(m_mainIndexBuffer);
    m_engine->GetDevice()->DestroyGPUBuffer(m_mainVertexBuffer);
    
    delete[] m_indexBuffer;
    delete[] m_vertexBuffer;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

dbasic::ConsoleVertex *dbasic::UiRenderer::AllocateQuads(int n) {
    for (int i = 0; i < n; ++i) {
        m_indexBuffer[m_indexOffset + i * 6 + 0] = 2 + m_vertexOffset + i * 4;
        m_indexBuffer[m_indexOffset + i * 6 + 1] = 1 + m_vertexOffset + i * 4;
        m_indexBuffer[m_indexOffset + i * 6 + 2] = 0 + m_vertexOffset + i * 4;

        m_indexBuffer[m_indexOffset + i * 6 + 3] = 3 + m_vertexOffset + i * 4;
        m_indexBuffer[m_indexOffset + i * 6 + 4] = 2 + m_vertexOffset + i * 4;
        m_indexBuffer[m_indexOffset + i * 6 + 5] = 0 + m_vertexOffset + i * 4;
    }

    ConsoleVertex *vertexBuffer = m_vertexBuffer + m_vertexOffset;

    m_indexOffset += n * 6;
    m_vertexOffset += n * 4;

    return vertexBuffer;
}

dbasic::ConsoleVertex *dbasic::UiRenderer::AllocateTriangles(int n) {
    for (int i = 0; i < n; ++i) {
        m_indexBuffer[m_indexOffset + i * 3 + 0] = 2 + m_vertexOffset + i * 3;
        m_indexBuffer[m_indexOffset + i * 3 + 1] = 1 + m_vertexOffset + i * 3;
        m_indexBuffer[m_indexOffset + i * 3 + 2] = 0 + m_vertexOffset + i * 3;
    }

    ConsoleVertex *vertexBuffer = m_vertexBuffer + m_vertexOffset;

    m_indexOffset += n * 3;
    m_vertexOffset += n * 3;

    return vertexBuffer;
}

ysError dbasic::UiRenderer::Reset() {
    YDS_ERROR_DECLARE("Reset");

    m_indexOffset = 0;
    m_vertexOffset = 0;

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::UiRenderer::InitializeGeometry(int bufferSize) {
    YDS_ERROR_DECLARE("InitializeGeometry");

    m_bufferSize = bufferSize;

    m_indexBuffer = new unsigned short[m_bufferSize * 2];
    m_vertexBuffer = new ConsoleVertex[m_bufferSize];

    ysDevice *device = m_engine->GetDevice();

    YDS_NESTED_ERROR_CALL(
        device->CreateVertexBuffer(
            &m_mainVertexBuffer, 
            sizeof(ConsoleVertex) * m_bufferSize, 
            (char *)m_vertexBuffer));
    YDS_NESTED_ERROR_CALL(
        device->CreateIndexBuffer(
            &m_mainIndexBuffer, 
            sizeof(unsigned short) * 2 * m_bufferSize, 
            (char *)m_indexBuffer));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}
