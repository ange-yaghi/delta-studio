#include "../include/text_renderer.h"

#include "../include/delta_engine.h"

dbasic::TextRenderer::TextRenderer() {
    m_engine = nullptr;
    m_renderer = nullptr;
    m_font = nullptr;
}

dbasic::TextRenderer::~TextRenderer() {
    /* void */
}

void dbasic::TextRenderer::Initialize() {
    /* void */
}

void dbasic::TextRenderer::RenderText(const std::string &s, float x, float y, float h) {
    m_renderer->SetFont(m_font);

    const int n = (int)s.size();
    if (n == 0) return;

    ConsoleVertex *vertexData = m_renderer->AllocateQuads(n);
    int quadIndex = 0;

    const float scale_x = 22.0f;
    const float scale_y = 32.0f;

    const float scale = scale_y / m_font->GetFontHeight();

    float current_x = x;
    float current_y = y;

    for (int i = 0; i < n; ++i) {
        const char character = s[i];

        const Font::GlyphData *data = m_font->GetGlyphData(character);

        vertexData[quadIndex * 4 + 0].TexCoord = ysVector2(data->uv0.x, data->uv0.y);
        vertexData[quadIndex * 4 + 1].TexCoord = ysVector2(data->uv1.x, data->uv0.y);
        vertexData[quadIndex * 4 + 2].TexCoord = ysVector2(data->uv1.x, data->uv1.y);
        vertexData[quadIndex * 4 + 3].TexCoord = ysVector2(data->uv0.x, data->uv1.y);

        vertexData[quadIndex * 4 + 0].Pos =
            ysVector2(data->p0.x * scale + current_x, -data->p0.y * scale + current_y);
        vertexData[quadIndex * 4 + 1].Pos =
            ysVector2(data->p1.x * scale + current_x, -data->p0.y * scale + current_y);
        vertexData[quadIndex * 4 + 2].Pos =
            ysVector2(data->p1.x * scale + current_x, -data->p1.y * scale + current_y);
        vertexData[quadIndex * 4 + 3].Pos =
            ysVector2(data->p0.x * scale + current_x, -data->p1.y * scale + current_y);

        ++quadIndex;

        current_x += data->Shift * scale;
    }
}

bool dbasic::TextRenderer::IsWhitespace(char c) {
    if (c == '\t' || c == '\n' || c == ' ') return true;
    return false;
}
