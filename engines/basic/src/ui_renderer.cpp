#include "../include/ui_renderer.h"

#include "../include/delta_basic_engine.h"

dbasic::UiRenderer::UiRenderer() {
    m_engine = nullptr;
    m_bufferSize = 0;
    m_vertexOffset = 0;

    m_font = nullptr;
    m_indexBuffer = nullptr;
    m_vertexBuffer = nullptr;
    m_mainIndexBuffer = nullptr;
    m_mainVertexBuffer = nullptr;
    m_indexOffset = 0;
}

dbasic::UiRenderer::~UiRenderer() {
    assert(m_mainIndexBuffer == nullptr);
    assert(m_mainVertexBuffer == nullptr);
}

ysError dbasic::UiRenderer::Initialize(int bufferSize) {
    YDS_ERROR_DECLARE("Initialize");

    YDS_NESTED_ERROR_CALL(InitializeGeometry(bufferSize));

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::UiRenderer::UpdateDisplay() {
    YDS_ERROR_DECLARE("UpdateDisplay");

    if (m_vertexOffset == 0) return YDS_ERROR_RETURN(ysError::None);

    m_shaders.SetScreenDimensions(
        (float)m_engine->GetScreenWidth(),
        (float)m_engine->GetScreenHeight());
    m_shaders.CalculateCamera();

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

    m_shaders.SetTexture(m_font->GetTexture());
    m_engine->DrawGeneric(
        m_shaders.GetFlags(),
        m_mainIndexBuffer,
        m_mainVertexBuffer,
        sizeof(ConsoleVertex),
        0,
        0,
        m_indexOffset / 3,
        false);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::UiRenderer::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    m_engine->GetDevice()->DestroyGPUBuffer(m_mainIndexBuffer);
    m_engine->GetDevice()->DestroyGPUBuffer(m_mainVertexBuffer);
    
    delete[] m_indexBuffer;
    delete[] m_vertexBuffer;

    return YDS_ERROR_RETURN(ysError::None);
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

    return YDS_ERROR_RETURN(ysError::None);
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

    return YDS_ERROR_RETURN(ysError::None);
}
