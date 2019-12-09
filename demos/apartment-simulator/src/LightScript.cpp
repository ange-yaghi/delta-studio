#include "../include/LightScript.h"

LightScript::LightScript() {
    m_currentLight = NULL;
    m_currentShadow = NULL;
    m_manager = nullptr;
}

LightScript::~LightScript() {
    /* void */
}

ysVector4 LightScript::ReadColor() {
    ysVector4 r;
    int v;
    m_file >> v; r.x = v / 255.0f;
    m_file >> v; r.y = v / 255.0f;
    m_file >> v; r.z = v / 255.0f;
    r.w = 1.0f;

    return r;
}

ysVector4 LightScript::ReadVector() {
    ysVector4 r;
    float v;

    m_file >> v; r.x = v;
    m_file >> v; r.y = v;
    m_file >> v; r.z = v;
    r.w = 1.0f;

    return r;
}

ysVector4 LightScript::ReadDirection() {
    ysVector4 r;
    float v;

    m_file >> v; r.x = v;
    m_file >> v; r.y = v;
    m_file >> v; r.z = v;
    r.w = 0.0f;

    ysVector vec = ysMath::LoadVector(r);
    vec = ysMath::Normalize(vec);

    return ysMath::GetVector3(vec);
}

ysVector4 LightScript::ReadPosition() {
    ysVector4 r;
    r.x = ReadLength();
    r.y = ReadLength();
    r.z = ReadLength();
    r.w = 0.0;

    return r;
}

float LightScript::ReadLength() {
    float f, i;
    char c;
    m_file >> f;
    m_file >> c;
    m_file >> i;

    return f * 12 + i;
}

void LightScript::Execute(const char *fname) {
    m_file.open(fname);
    char stringBuffer[1024];

    while (!m_file.eof()) {
        m_file >> stringBuffer;

        if (strcmp(stringBuffer, "Light") == 0) {
            m_file >> stringBuffer;
            m_currentLight = m_manager->FindLight(stringBuffer);
            m_currentShadow = nullptr;
        }
        else if (strcmp(stringBuffer, "Shadow") == 0) {
            m_file >> stringBuffer;
            m_currentShadow = m_manager->FindShadow(stringBuffer);
            m_currentLight = nullptr;
        }

        if (m_currentShadow != NULL) {
            if (strcmp(stringBuffer, "FringeMask") == 0) {
                ysVector4 v = ReadVector();
                m_currentShadow->m_fringeMask = v;
            }
            else if (strcmp(stringBuffer, "Intensity") == 0) {
                float f;
                m_file >> f;
                m_currentShadow->m_shadowIntensity = f;
            }
            else if (strcmp(stringBuffer, "Falloff") == 0) {
                m_currentShadow->m_falloff = ReadLength();
            }
        }

        if (m_currentLight != NULL) {
            if (strcmp(stringBuffer, "Color") == 0) {
                ysVector4 color = ReadColor();
                m_currentLight->m_color = color;
            }
            else if (strcmp(stringBuffer, "LowColor") == 0) {
                ysVector4 color = ReadColor();
                m_currentLight->m_lowColor = color;
            }
            else if (strcmp(stringBuffer, "Direction") == 0) {
                ysVector4 dir = ReadDirection();
                m_currentLight->m_direction = dir;
            }
            else if (strcmp(stringBuffer, "CutoffHigh") == 0) {
                float f;
                m_file >> f;
                m_currentLight->m_cutoffHigh = f;
            }
            else if (strcmp(stringBuffer, "CutoffLow") == 0) {
                float f;
                m_file >> f;
                m_currentLight->m_cutoffLow = f;
            }
            else if (strcmp(stringBuffer, "CutoffPower") == 0) {
                float p;
                m_file >> p;
                m_currentLight->m_cutoffPower = p;
            }
            else if (strcmp(stringBuffer, "CutoffMix") == 0) {
                float p;
                m_file >> p;
                m_currentLight->m_cutoffMix = p;
            }
            else if (strcmp(stringBuffer, "CutoffAmbient") == 0) {
                int p;
                m_file >> p;
                m_currentLight->m_cutoffAmbient = p;
            }
            else if (strcmp(stringBuffer, "Falloff") == 0) {
                float f = ReadLength();
                m_currentLight->m_falloff = f;
            }
            else if (strcmp(stringBuffer, "Intensity") == 0) {
                float f;
                m_file >> f;
                m_currentLight->m_intensity = f;
            }
            else if (strcmp(stringBuffer, "Ambient") == 0) {
                float f;
                m_file >> f;
                m_currentLight->m_ambient = f;
            }
            else if (strcmp(stringBuffer, "Diffuse") == 0) {
                float f;
                m_file >> f;
                m_currentLight->m_diffuse = f;
            }
            else if (strcmp(stringBuffer, "Specular") == 0) {
                float f;
                m_file >> f;
                m_currentLight->m_specular = f;
            }
            else if (strcmp(stringBuffer, "SpecularPower") == 0) {
                float f;
                m_file >> f;
                m_currentLight->m_specularPower = f;
            }
            else if (strcmp(stringBuffer, "Position") == 0) {
                ysVector4 pos = ReadPosition();
                m_currentLight->m_location = pos;
            }
        }
    }

    m_file.close();
}
