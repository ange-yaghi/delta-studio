#include "../include/shader_set.h"

dbasic::ShaderSet::ShaderSet() {
    m_device = nullptr;
}

dbasic::ShaderSet::~ShaderSet() {
    assert(m_stages.GetNumObjects() == 0);
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

int dbasic::ShaderSet::GetObjectDataSize() const {
    const int stageCount = GetStageCount();
    int totalSize = 0;
    for (int i = 0; i < stageCount; ++i) {
        totalSize += m_stages[i]->GetObjectDataSize();
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

ysError dbasic::ShaderSet::ReadObjectData(const void *memory, int size) {
    YDS_ERROR_DECLARE("ReadObjectData");

    if (GetObjectDataSize() > size) {
        YDS_ERROR_RETURN_MSG(
            ysError::InvalidOperation, 
            "Buffer is not big enough for all object data");
    }

    const char *readBuffer = reinterpret_cast<const char *>(memory);

    const int stageCount = GetStageCount();
    for (int i = 0; i < stageCount; ++i) {
        m_stages[i]->ReadObjectData(readBuffer);
        readBuffer += m_stages[i]->GetObjectDataSize();
    }

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderSet::ReadObjectData(const void *memory, int stageIndex, int size) {
    YDS_ERROR_DECLARE("ReadObjectData");

    ShaderStage *stage = m_stages[stageIndex];
    if (stage->GetObjectDataSize() > size) {
        YDS_ERROR_RETURN_MSG(
            ysError::InvalidOperation,
            "Buffer is not big enough for all object data");
    }

    stage->ReadObjectData(
        reinterpret_cast<const char *>(memory) + GetStageCacheOffset(stageIndex));

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

ysError dbasic::ShaderSet::MergeAndClear(ShaderSet *shaderSet) {
    YDS_ERROR_DECLARE("Merge");

    const int stageCount = shaderSet->GetStageCount();
    for (int i = 0; i < stageCount; ++i) {
        ShaderStage *stage = shaderSet->GetStage(i);
        const int index = GetLargestPossibleIndex(stage);

        m_stages.Insert(index) = stage;
    }

    shaderSet->ShallowDestroy();

    return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderSet::ShallowDestroy() {
    YDS_ERROR_DECLARE("ShallowDestroy");

    m_stages.Clear();

    return YDS_ERROR_RETURN(ysError::None);
}

int dbasic::ShaderSet::GetLargestPossibleIndex(const ShaderStage *stage) const {
    const int stageCount = m_stages.GetNumObjects();
    int largestPossibleIndex = stageCount;
    for (int i = stageCount; i >= 0; --i) {
        if (m_stages[i]->DependsOn(stage->GetRenderTarget())) {
            largestPossibleIndex = i;
        }
    }

    return largestPossibleIndex;
}
