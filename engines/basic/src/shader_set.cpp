#include "../include/shader_set.h"

dbasic::ShaderSet::ShaderSet() {
    m_device = nullptr;
}

dbasic::ShaderSet::~ShaderSet() {
    /* void */
}

ysError dbasic::ShaderSet::Initialize(ysDevice *device) {
    YDS_ERROR_DECLARE("Initialize");

    m_device = device;

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderSet::Destroy() {
    YDS_ERROR_DECLARE("Destroy");

    const int stageCount = GetStageCount();
    for (int i = 0; i < stageCount; ++i) {
        ShaderStage *stage = m_stages[i];

        stage->Destroy();
        delete stage;
    }

    m_stages.Destroy();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderSet::NewStage(const std::string &name, ShaderStage **newStage) {
    YDS_ERROR_DECLARE("NewStage");

    ShaderStage *stage = m_stages.New() = new ShaderStage();
    stage->Initialize(m_device, name);

    *newStage = stage;

    return YDS_ERROR_RETURN(ysError::None);
}

int dbasic::ShaderSet::GetObjectDataSize() const {
    const int stageCount = GetStageCount();
    int totalSize = 0;
    for (int i = 0; i < stageCount; ++i) {
        totalSize = m_stages[i]->GetObjectDataSize();
    }

    return totalSize;
}

ysError dbasic::ShaderSet::CacheObjectData(void *memory, int size) {
    YDS_ERROR_DECLARE("CacheObjectData");

    if (GetObjectDataSize() > size) {
        YDS_ERROR_RETURN_MSG(
            ysError::InvalidOperation, 
            "Buffer is not big enough for all object data");
    }

    char *writeBuffer = reinterpret_cast<char *>(memory);

    const int stageCount = GetStageCount();
    for (int i = 0; i < stageCount; ++i) {
        m_stages[i]->CacheObjectData(writeBuffer);
        writeBuffer += m_stages[i]->GetObjectDataSize();
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderSet::ReadObjectData(void *memory, int size) {
    YDS_ERROR_DECLARE("ReadObjectData");

    if (GetObjectDataSize() > size) {
        YDS_ERROR_RETURN_MSG(
            ysError::InvalidOperation, 
            "Buffer is not big enough for all object data");
    }

    char *readBuffer = reinterpret_cast<char *>(memory);

    const int stageCount = GetStageCount();
    for (int i = 0; i < stageCount; ++i) {
        m_stages[i]->ReadObjectData(readBuffer);
        readBuffer += m_stages[i]->GetObjectDataSize();
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderSet::ReadObjectData(void *memory, int stageIndex, int size) {
    YDS_ERROR_DECLARE("ReadObjectData");

    ShaderStage *stage = m_stages[stageIndex];

    if (stage->GetObjectDataSize() > size) {
        YDS_ERROR_RETURN_MSG(
            ysError::InvalidOperation,
            "Buffer is not big enough for all object data");
    }

    stage->ReadObjectData(
        reinterpret_cast<char *>(memory) + GetStageCacheOffset(stageIndex));

    return YDS_ERROR_RETURN(ysError::None);
}

void dbasic::ShaderSet::Reset() {
    const int stageCount = GetStageCount();
    for (int i = 0; i < stageCount; ++i) {
        m_stages[i]->Reset();
    }
}

int dbasic::ShaderSet::GetStageCacheOffset(int index) const {
    const int stageCount = GetStageCount();
    int offset = 0;
    for (int i = 0; i < index; ++i) {
        offset += m_stages[i]->GetObjectDataSize();
    }

    return offset;
}
