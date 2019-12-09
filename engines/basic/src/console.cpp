#include "../include/console.h"

#include "../include/font_map.h"
#include "../include/delta_engine.h"

dbasic::Console::Console() {
    m_engine = NULL;

    m_mainVertexBuffer = NULL;
    m_mainIndexBuffer = NULL;

    m_vertexShader = NULL;
    m_pixelShader = NULL;
    m_shaderProgram = NULL;

    m_standardInputLayout = NULL;

    // Initialize clear color to black
    m_clearColor[0] = 40.0f / 255.0f;
    m_clearColor[1] = 40.0f / 255.0f;
    m_clearColor[2] = 40.0f / 255.0f;
    m_clearColor[3] = 1.0F;

    m_bufferWidth = 0;
    m_bufferHeight = 0;

    // Resetting settings
    m_fontBold = false;

    m_engine = NULL;
}

dbasic::Console::~Console() {
    /* void */
}

ysError dbasic::Console::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    // TODO

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::Console::Initialize() {
    YDS_ERROR_DECLARE("Initialize");

    m_engine->GetDevice()->CreateTexture(&m_font, "DeltaEngineTullahoma/Fonts/dc_font_consolas.png");
    YDS_NESTED_ERROR_CALL(InitializeGeometry());

    InitializeFontMap();

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::Console::InitializeGeometry() {
    YDS_ERROR_DECLARE("InitializeGeometry");

    unsigned short *indices = new unsigned short[6 * BUFFER_SIZE];

    for (int i = 0; i < BUFFER_SIZE; i++) {
        unsigned short offset = (i * 4);

        indices[i * 6 + 0] = 2 + offset;
        indices[i * 6 + 1] = 1 + offset;
        indices[i * 6 + 2] = 0 + offset;

        indices[i * 6 + 3] = 3 + offset;
        indices[i * 6 + 4] = 2 + offset;
        indices[i * 6 + 5] = 0 + offset;
    }

    // Populate the vertex data
    m_vertexData = new Vertex[4 * BUFFER_SIZE];

    float scaleX = 22.0f / 2.0f;
    float scaleY = 32.0f / 2.0f;

    float texScaleX = 22.0f / 512.0f;
    float texScaleY = 32.0f / 512.0f;

    for (int i = 0; i < BUFFER_WIDTH; i++) {
        for (int j = 0; j < BUFFER_HEIGHT; j++) {
            int offset = j * BUFFER_WIDTH + i;

            float offsetX = (-m_engine->GetScreenWidth() / 2.0f) + (scaleX * i);
            float offsetY = (m_engine->GetScreenHeight() / 2.0f) - (scaleY * j);

            m_vertexData[offset * 4 + 0].Pos = ysVector4(0.0f * scaleX + offsetX, 0.0f * scaleY + offsetY, -1.0f, 1.0f);
            m_vertexData[offset * 4 + 1].Pos = ysVector4(1.0f * scaleX + offsetX, 0.0f * scaleY + offsetY, -1.0f, 1.0f);
            m_vertexData[offset * 4 + 2].Pos = ysVector4(1.0f * scaleX + offsetX, -1.0f * scaleY + offsetY, -1.0f, 1.0f);
            m_vertexData[offset * 4 + 3].Pos = ysVector4(0.0f * scaleX + offsetX, -1.0f * scaleY + offsetY, -1.0f, 1.0f);

            m_vertexData[offset * 4 + 0].TexCoord = ysVector2(0.0f * texScaleX, 0.0f * texScaleY);
            m_vertexData[offset * 4 + 1].TexCoord = ysVector2(1.0f * texScaleX, 0.0f * texScaleY);
            m_vertexData[offset * 4 + 2].TexCoord = ysVector2(1.0f * texScaleX, 1.0f * texScaleY);
            m_vertexData[offset * 4 + 3].TexCoord = ysVector2(0.0f * texScaleX, 1.0f * texScaleY);
        }
    }

    ysDevice *device = m_engine->GetDevice();

    YDS_NESTED_ERROR_CALL(device->CreateVertexBuffer(&m_mainVertexBuffer, sizeof(Vertex) * 4 * BUFFER_SIZE, (char *)m_vertexData));
    YDS_NESTED_ERROR_CALL(device->CreateIndexBuffer(&m_mainIndexBuffer, sizeof(unsigned short) * 6 * BUFFER_SIZE, (char *)indices));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError dbasic::Console::UpdateDisplay() {
    YDS_ERROR_DECLARE("UpdateDisplay");

    m_engine->GetDevice()->EditBufferData(m_mainVertexBuffer, (char *)m_vertexData);

    m_engine->GetDevice()->UseIndexBuffer(m_mainIndexBuffer, 0);
    m_engine->GetDevice()->UseVertexBuffer(m_mainVertexBuffer, sizeof(Vertex), 0);

    m_engine->GetDevice()->UseTexture(m_font, 0);

    m_engine->GetDevice()->Draw(2 * 12000, 0, 0);

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);

}

void dbasic::Console::MoveToLocation(const GuiPoint &location) {
    m_nominalLocation = location;
    m_actualLocation = location;
}

void dbasic::Console::MoveDownLine(int n) {
    m_nominalLocation.y += n;
    MoveToLocation(m_nominalLocation);
}

ysError dbasic::Console::SetCharacter(char character) {
    YDS_ERROR_DECLARE("SetCharacter");

    // Get offset from character
    int offset = FONT_MAP[(unsigned char)character];

    if (m_fontBold && (offset < 185)) offset += BOLD_OFFSET;

    float row = offset / (float)23;
    float column = (float)(offset % 23);

    float texOffsetX = (column * 22.0f) / 512.0f;
    float texOffsetY = ((row) * 32.0f) / 512.0f;

    float scaleX = 22.0f;
    float scaleY = 32.0f;

    float texScaleX = scaleX / 512.0f;
    float texScaleY = scaleY / 512.0f;

    int x = m_actualLocation.x;
    int y = m_actualLocation.y;

    if (x >= BUFFER_WIDTH || y >= BUFFER_HEIGHT ||
        x < 0 || y < 0) return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);

    int index = y * BUFFER_WIDTH + x;

    m_vertexData[index * 4 + 0].TexCoord = ysVector2(0.0f * texScaleX + texOffsetX, 1.0f - (0.0f * texScaleY + texOffsetY));
    m_vertexData[index * 4 + 1].TexCoord = ysVector2(1.0f * texScaleX + texOffsetX, 1.0f - (0.0f * texScaleY + texOffsetY));
    m_vertexData[index * 4 + 2].TexCoord = ysVector2(1.0f * texScaleX + texOffsetX, 1.0f - (1.0f * texScaleY + texOffsetY));
    m_vertexData[index * 4 + 3].TexCoord = ysVector2(0.0f * texScaleX + texOffsetX, 1.0f - (1.0f * texScaleY + texOffsetY));

    return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
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

    for (int i = 0; i < BUFFER_HEIGHT; i++) {
        OutputChar(' ', BUFFER_WIDTH);
        MoveDownLine();
    }
}
