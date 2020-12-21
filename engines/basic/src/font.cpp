#include <stb/stb_truetype.h>
#include <stb/stb_rect_pack.h>

#include "../include/font.h"

dbasic::Font::Font() {
    m_texture = nullptr;
    m_glyphData = nullptr;
    m_firstGlyph = 0;
    m_glyphCount = 0;
    m_fontHeight = 12.0f;
}

dbasic::Font::~Font() {
    /* void */
}

void dbasic::Font::Initialize(int firstGlyph, int glyphCount, void *cdata_, float fontHeight, ysTexture *texture) {
    m_glyphData = new GlyphData[glyphCount];
    m_firstGlyph = firstGlyph;
    m_glyphCount = glyphCount;
    m_texture = texture;
    m_fontHeight = fontHeight;

    const stbtt_packedchar *cdata = reinterpret_cast<const stbtt_packedchar *>(cdata_);

    for (int i = 0; i < m_glyphCount; ++i) {
        stbtt_aligned_quad q;

        float x = 0.0, y = 0.0;
        stbtt_GetPackedQuad(cdata, texture->GetWidth(), texture->GetHeight(), i, &x, &y, &q, 1);

        GlyphData &data = m_glyphData[i];
        data.p0 = { q.x0, q.y0 };
        data.p1 = { q.x1, q.y1 };
        data.uv0 = { q.s0, q.t0 };
        data.uv1 = { q.s1, q.t1 };
        data.Shift = x;
    }
}

const dbasic::Font::GlyphData *dbasic::Font::GetGlyphData(int glyph) const {
    if (glyph < m_firstGlyph) return nullptr;
    if (glyph >= m_firstGlyph + m_glyphCount) return nullptr;

    return &m_glyphData[glyph - m_firstGlyph];
}
