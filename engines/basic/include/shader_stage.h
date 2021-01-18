#ifndef DELTA_BASIC_SHADER_STAGE_H
#define DELTA_BASIC_SHADER_STAGE_H

#include "delta_core.h"

#include <string>

namespace dbasic {

    class ShaderStage : public ysObject {
    public:
        struct ConstantBufferBinding {
            enum class BufferType {
                ObjectData,
                SceneData
            };

            std::string Name = "";
            ysGPUBuffer *Buffer = nullptr;
            void *Memory = nullptr;
            int Size = 0;
            int Slot = 0;
            BufferType Type = BufferType::SceneData;
            bool Preallocated = false;
        };

        struct Input {
            ShaderStage *Stage;
            int Slot;
        };

        enum class Type {
            FullPass,
            PostProcessing
        };

    public:
        ShaderStage();
        ~ShaderStage();

        ysError Initialize(ysDevice *device, const std::string &name);
        ysError Destroy();

        template <typename T_ConstantData>
        ysError NewConstantBuffer(
            const std::string &name, 
            int slot, 
            ConstantBufferBinding::BufferType type, 
            T_ConstantData *memory = nullptr, 
            int *index = nullptr) 
        {
            YDS_ERROR_DECLARE("NewConstantBuffer");

            ConstantBufferBinding &newBuffer = m_bufferBindings.New();
            YDS_NESTED_ERROR_CALL(
                m_device->CreateConstantBuffer(
                    &newBuffer.Buffer,
                    sizeof(T_ConstantData),
                    nullptr,
                    false));
            newBuffer.Preallocated = (memory != nullptr);
            newBuffer.Memory = (newBuffer.Preallocated)
                ? memory
                : reinterpret_cast<void *>(new T_ConstantData);
            newBuffer.Size = sizeof(T_ConstantData);
            newBuffer.Name = name;
            newBuffer.Type = type;
            newBuffer.Slot = slot;

            if (type == ConstantBufferBinding::BufferType::ObjectData) {
                m_objectDataSize += newBuffer.Size;
            }

            if (index != nullptr) *index = m_bufferBindings.GetNumObjects() - 1;

            return YDS_ERROR_RETURN(ysError::None);
        }

        int GetBufferCount() const { return m_bufferBindings.GetNumObjects(); }

        template <typename T_ConstantData>
        T_ConstantData *MapBufferData(int index) {
            const ConstantBufferBinding &binding = m_bufferBindings[index];
            return (sizeof(T_ConstantData) != binding.Size)
                ? nullptr
                : reinterpret_cast<T_ConstantData *>(m_bufferBindings[index].Memory);
        }

        void CacheObjectData(void *target);
        void ReadObjectData(void *source);

        ysError AddInput(ShaderStage *stage, int slot);
        int GetInputCount() const { return m_inputs.GetNumObjects(); }  

        ysError BindScene();
        ysError BindObject();

        void SetRenderTarget(ysRenderTarget *target) { m_renderTarget = target; }
        ysRenderTarget *GetRenderTarget() const { return m_renderTarget; }

        void SetShaderProgram(ysShaderProgram *shaderProgram) { m_shaderProgram = shaderProgram; }
        ysShaderProgram *GetShaderProgram() const { return m_shaderProgram; }

        void SetInputLayout(ysInputLayout *layout) { m_inputLayout = layout; }
        ysInputLayout *GetInputLayout() const { return m_inputLayout; }

        void SetType(Type type) { m_type = type; }
        Type GetType() const { return m_type; }

        void Reset() { m_complete = false; }
        bool IsComplete() const { return m_complete; }
        void SetComplete() { m_complete = true; }

        int GetObjectDataSize() const { return m_objectDataSize; }

    protected:
        ysDevice *m_device;
        ysRenderTarget *m_renderTarget;

        ysShaderProgram *m_shaderProgram;
        ysInputLayout *m_inputLayout;

        ysExpandingArray<ConstantBufferBinding> m_bufferBindings;
        ysExpandingArray<Input> m_inputs;

        std::string m_name;
        Type m_type;

        int m_objectDataSize;

        bool m_complete;
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_SHADER_STAGE_H */
