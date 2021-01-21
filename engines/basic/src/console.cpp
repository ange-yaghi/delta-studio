#include "../include/console.h"

#include "../include/font_map.h"
#include "../include/delta_engine.h"

dbasic::Console::Console() {
    m_engine = nullptr;

    m_font = nullptr;

    m_bufferWidth = 0;
    m_bufferHeight = 0;

    // Resetting settings
    m_buffer = nullptr;

    m_engine = nullptr;
    m_defaultFontDirectory = "";

    m_font = nullptr;
}

dbasic::Console::~Console() {
    /* void */
}

ysError dbasic::Console::Initialize() {
    YDS_ERROR_DECLARE("Initialize");

    m_engine->LoadFont(&m_font, (m_defaultFontDirectory + "Silkscreen/slkscr.ttf").c_str());
    m_buffer = new char[BufferSize];

    Clear();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::Console::ResetScreenPosition() {
    YDS_ERROR_DECLARE("Reset");

    for (int i = 0; i < BufferWidth; i++) {
        for (int j = 0; j < BufferHeight; j++) {
            const int index = j * BufferWidth + i;
            const char c = m_buffer[index];

            SetCharacter(c);
        }
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::Console::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    delete[] m_buffer;
    m_buffer = nullptr;

    ysTexture *fontTexture = m_font->GetTexture();
    m_engine->GetDevice()->DestroyTexture(fontTexture);

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::Console::UpdateGeometry() {
    YDS_ERROR_DECLARE("UpdateGeometry");

    m_renderer->SetFont(m_font);

    const int n = GetTotalNotWhitespace();
    if (n == 0) return YDS_ERROR_RETURN(ysError::None);

    ConsoleVertex *vertexData = m_renderer->AllocateQuads(n);
    int quadIndex = 0;

    const float scale_x = 22.0f;
    const float scale_y = 32.0f;

    const float scale = scale_y / m_font->GetFontHeight();

    for (int i = 0; i < BufferWidth; ++i) {
        for (int j = 0; j < BufferHeight; ++j) {
            const int index = j * BufferWidth + i;
            const char character = m_buffer[index];

            if (IsWhiteSpace(character)) continue;

            const Font::GlyphData *data = m_font->GetGlyphData(character);

            const float offsetX = (-m_engine->GetScreenWidth() / 2.0f) + (scale_x * i);
            const float offsetY = (m_engine->GetScreenHeight() / 2.0f) - (scale_y * j) - scale_y;

            vertexData[quadIndex * 4 + 0].TexCoord = ysVector2(data->uv0.x, data->uv0.y);
            vertexData[quadIndex * 4 + 1].TexCoord = ysVector2(data->uv1.x, data->uv0.y);
            vertexData[quadIndex * 4 + 2].TexCoord = ysVector2(data->uv1.x, data->uv1.y);
            vertexData[quadIndex * 4 + 3].TexCoord = ysVector2(data->uv0.x, data->uv1.y);

            vertexData[quadIndex * 4 + 0].Pos = 
                ysVector2(data->p0.x * scale + offsetX, -data->p0.y * scale + offsetY);
            vertexData[quadIndex * 4 + 1].Pos = 
                ysVector2(data->p1.x * scale + offsetX, -data->p0.y * scale + offsetY);
            vertexData[quadIndex * 4 + 2].Pos = 
                ysVector2(data->p1.x * scale + offsetX, -data->p1.y * scale + offsetY);
            vertexData[quadIndex * 4 + 3].Pos = 
                ysVector2(data->p0.x * scale + offsetX, -data->p1.y * scale + offsetY);
            
            ++quadIndex;
        }
    }

    return YDS_ERROR_RETURN(ysError::None);
}

void dbasic::Console::MoveToLocation(const GuiPoint &location) {
    m_nominalLocation = location;
    m_actualLocation = location;
}

void dbasic::Console::MoveToOrigin() {
    MoveToLocation({ 0, 0 });
}

void dbasic::Console::MoveDownLine(int n) {
    m_nominalLocation.y += n;
    MoveToLocation(m_nominalLocation);
}

ysError dbasic::Console::SetCharacter(char character) {
    YDS_ERROR_DECLARE("SetCharacter");

    const int x = m_actualLocation.x;
    const int y = m_actualLocation.y;

    if (x >= BufferWidth || y >= BufferHeight ||
        x < 0 || y < 0) return YDS_ERROR_RETURN(ysError::None);

    const int index = y * BufferWidth + x;
    m_buffer[index] = character;

    return YDS_ERROR_RETURN(ysError::None);
}

void dbasic::Console::OutputChar(unsigned char c, int n) {
    if (c == '\t') {
        OutputChar(' ', 4);
        return;
    }

    for (int i = 0; i < n; i++) {
        SetCharacter(c);
        m_actualLocation.x += 1;
    }
}

void dbasic::Console::DrawGeneralText(const char *text, int maxLength) {
    int i = 0;

    while (text[i] != '\0' && (maxLength < 0 || i < maxLength)) {
        if (text[i] != '\n') {
            OutputChar(text[i], 1);
        }
        else MoveDownLine();

        i++;
    }
}

void dbasic::Console::DrawBoundText(const char *text, int width, int height, int xOffset, int yOffset) {
    int i = 0;
    int currentLine = 0;
    int currentColumn = 0;

    while (text[i]) {
        if (text[i] != '\n') {
            // Check boundaries
            if (currentColumn >= xOffset && currentColumn < xOffset + width) {
                if (currentLine >= yOffset && currentLine < yOffset + height) {
                    OutputChar(text[i], 1);
                }
            }

            currentColumn++;
        }
        else {
            if (currentLine >= yOffset && currentLine < yOffset + height) m_actualLocation.y++;
            currentLine++;
            currentColumn = 0;
        }

        i++;
    }
}

void dbasic::Console::DrawWrappedText(const char *text, int width) {
    if (width <= 0) return;

    int i = 0;
    int lineLength = 0;
    while (text[i]) {
        if (IsWhiteSpace(text[i]) &&
            FindEndOfNextWord(text, i + 1) - (i + 1) + lineLength > width) {

            m_actualLocation.y++;
            m_actualLocation.x = m_nominalLocation.x;
            lineLength = 0;
        }

        else if (text[i] != '\n') OutputChar(text[i]);
        else { MoveDownLine(); lineLength = 0; }

        lineLength++;
        i++;
    }
}

int dbasic::Console::GetTotalNotWhitespace() const {
    int n = 0;
    for (int i = 0; i < BufferWidth; ++i) {
        for (int j = 0; j < BufferHeight; ++j) {
            if (!IsWhiteSpace(m_buffer[i + j * BufferWidth])) {
                ++n;
            }
        }
    }

    return n;
}

// Drawing shapes

void dbasic::Console::DrawHorizontalLine(int length) {
    RealignLocation();

    OutputChar(196, length);
}

void dbasic::Console::DrawVerticalLine(int length) {
    RealignLocation();

    int i = 0;
    for (; i < length; i++) {
        OutputChar((char)179, 1);
        m_actualLocation.y++;
        m_actualLocation.x = m_nominalLocation.x;
    }
}

void dbasic::Console::DrawLineRectangle(int width, int height) {
    RealignLocation();

    GuiPoint startingPoint = m_nominalLocation;

    OutputChar(218, 1);
    m_nominalLocation.x++;
    DrawHorizontalLine(width - 2);
    MoveToLocation(startingPoint);
    MoveDownLine();
    DrawVerticalLine(height - 2);

    MoveToLocation(startingPoint + GuiPoint(0, height - 1));
    OutputChar(192, 1);
    m_nominalLocation.x++;
    DrawHorizontalLine(width - 2);

    MoveToLocation(startingPoint + GuiPoint(width - 1, 0));
    OutputChar(191, 1);

    MoveDownLine();
    DrawVerticalLine(height - 2);

    MoveToLocation(startingPoint + GuiPoint(width - 1, height - 1));
    OutputChar(217, 1);
}

// Utilities

int dbasic::Console::FindEndOfNextWord(const char *text, int location) {
    while (text[location] != '\0') {
        if (IsWhiteSpace(text[location])) return location;
        location++;
    }

    return location - 1;
}

bool dbasic::Console::IsWhiteSpace(char c) {
    if (c == '\t' || c == '\n' || c == ' ') return true;
    return false;
}

void dbasic::Console::Clear() {
    MoveToLocation(GuiPoint(0, 0));

    for (int i = 0; i < BufferHeight; i++) {
        OutputChar(' ', BufferWidth);
        MoveDownLine();
    }
}
