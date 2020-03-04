#ifndef APTSIM_LIGHT_SCRIPT_H
#define APTSIM_LIGHT_SCRIPT_H

#include "SceneManager.h"

#include <string>
#include <fstream>

class LightScript {
public:
    LightScript();
    ~LightScript();

    void Execute(const char *fname);
    ysVector4 ReadColor();
    ysVector4 ReadVector();
    ysVector4 ReadDirection();
    ysVector4 ReadPosition();
    float ReadLength();

    SceneManager *m_manager;

protected:

    LightObject *m_currentLight;
    ShadowObject *m_currentShadow;
    std::ifstream m_file;
};

#endif /* APTSIM_LIGHT_SCRIPT_H */
