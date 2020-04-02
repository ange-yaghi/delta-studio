#ifndef YDS_ANIMATION_INTERCHANGE_FILE_0_0_H
#define YDS_ANIMATION_INTERCHANGE_FILE_0_0_H

#include "yds_base.h"

#include "yds_animation_curve.h"
#include "yds_math.h"

#include <fstream>

class ysAnimationAction;

class ysAnimationInterchangeFile0_0 : public ysObject {
public:
    static constexpr int MAJOR_VERSION = 0x00;
    static constexpr int MINOR_VERSION = 0x00;
    static constexpr int MAGIC_NUMBER = 0xFEA4AA;

public:
    struct IdHeader {
        unsigned int MagicNumber;
        unsigned int MajorVersion;
        unsigned int MinorVersion;
        unsigned int EditorId;
        unsigned int CompilationStatus;
    };

    struct FileHeader {
        unsigned int ActionCount;
    };

    struct ActionHeader {
        char Name[256];
        unsigned int CurveCount;
    };

    struct CurveHeader {
        char TargetBone[256];
        unsigned int KeyframeCount;
        unsigned int CurveType;
    };

    struct Keyframe {
        float Timestamp;
        float Value;
    };

public:
    ysAnimationInterchangeFile0_0();
    ~ysAnimationInterchangeFile0_0();

    ysError Open(const char *fname);
    ysError Close();

    unsigned int GetMinorVersion() const { return m_minorVersion; }
    unsigned int GetMajorVersion() const { return m_majorVersion; }

    unsigned int GetActionCount() const { return m_actionCount; }

    unsigned int GetToolId() const { return m_toolId; }
    bool GetCompilationStatus() const { return m_compilationStatus; }

    ysError ReadAction(ysAnimationAction *action);

    static ysAnimationCurve::CurveType InterpretCurveType(unsigned int curveType);

protected:
    std::fstream m_file;

    unsigned int m_minorVersion;
    unsigned int m_majorVersion;

    unsigned int m_toolId;

    unsigned int m_actionCount;

    bool m_compilationStatus;
};

#endif /* YDS_ANIMATION_INTERCHANGE_FILE_0_0_H */
