#include "../include/shader_stage.h"

dbasic::ShaderStage::ShaderStage() {
	m_device = nullptr;
	m_renderTarget = nullptr;
	m_shaderProgram = nullptr;
	m_inputLayout = nullptr;
	m_type = Type::FullPass;
	m_name = "";
	m_objectDataSize = 0;
}

dbasic::ShaderStage::~ShaderStage() {
	assert(GetBufferCount() == 0);
}

ysError dbasic::ShaderStage::Initialize(ysDevice *device, const std::string &name) {
	YDS_ERROR_DECLARE("Initialize");

	m_device = device;
	m_name = name;

	return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderStage::Destroy() {
	YDS_ERROR_DECLARE("Destroy");

	const int bufferCount = GetBufferCount();
	for (int i = 0; i < bufferCount; ++i) {
		YDS_NESTED_ERROR_CALL(m_device->DestroyGPUBuffer(m_bufferBindings[i].Buffer));
		if (!m_bufferBindings[i].Preallocated) {
			delete m_bufferBindings[i].Memory;
		}
	}

	m_bufferBindings.Destroy();

	return YDS_ERROR_RETURN(ysError::None);
}

void dbasic::ShaderStage::CacheObjectData(void *target) {
	const int bufferCount = GetBufferCount();
	char *writeBuffer = reinterpret_cast<char *>(target);
	for (int i = 0; i < bufferCount; ++i) {
		if (m_bufferBindings[i].Type == ConstantBufferBinding::BufferType::ObjectData) {
			memcpy(writeBuffer, m_bufferBindings[i].Memory, m_bufferBindings[i].Size);
			writeBuffer += m_bufferBindings[i].Size;
		}
	}
}

void dbasic::ShaderStage::ReadObjectData(void *source) {
	const int bufferCount = GetBufferCount();
	char *readBuffer = reinterpret_cast<char *>(source);
	for (int i = 0; i < bufferCount; ++i) {
		if (m_bufferBindings[i].Type == ConstantBufferBinding::BufferType::ObjectData) {
			memcpy(m_bufferBindings[i].Memory, readBuffer, m_bufferBindings[i].Size);
			readBuffer += m_bufferBindings[i].Size;
		}
	}
}

ysError dbasic::ShaderStage::AddInput(ShaderStage *stage, int slot) {
	YDS_ERROR_DECLARE("AddInput");

	Input &input = m_inputs.New();
	input.Stage = stage;
	input.Slot = slot;

	return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderStage::BindScene() {
	YDS_ERROR_DECLARE("BindScene");

	m_device->SetRenderTarget(m_renderTarget);
	m_device->UseShaderProgram(m_shaderProgram);
	m_device->UseInputLayout(m_inputLayout);

	const int inputCount = GetInputCount();
	for (int i = 0; i < inputCount; ++i) {
		const Input &input = m_inputs[i];

		if (!input.Stage->IsComplete()) {
			YDS_ERROR_RETURN_MSG(ysError::InvalidOperation, "Shader stage binding before dependency has rendered");
		}

		ysRenderTarget *inputRenderTarget = input.Stage->GetRenderTarget();
		if (inputRenderTarget->GetType() != ysRenderTarget::Type::OffScreen) {
			YDS_ERROR_RETURN_MSG(ysError::InvalidOperation, "Attempting to use a non-offscreen render target as an input to another stage");
		}

		m_device->UseRenderTargetAsTexture(inputRenderTarget, input.Slot);
	}

	const int bufferCount = GetBufferCount();
	for (int i = 0; i < bufferCount; ++i) {
		const ConstantBufferBinding &binding = m_bufferBindings[i];
		
		if (binding.Type == ConstantBufferBinding::BufferType::SceneData) {
			YDS_NESTED_ERROR_CALL(m_device->EditBufferData(binding.Buffer, reinterpret_cast<char *>(binding.Memory)));
			YDS_NESTED_ERROR_CALL(m_device->UseConstantBuffer(binding.Buffer, binding.Slot));
		}
	}

	return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderStage::BindObject() {
	YDS_ERROR_DECLARE("BindObject");

	const int bufferCount = GetBufferCount();
	for (int i = 0; i < bufferCount; ++i) {
		const ConstantBufferBinding &binding = m_bufferBindings[i];

		if (binding.Type == ConstantBufferBinding::BufferType::ObjectData) {
			YDS_NESTED_ERROR_CALL(m_device->EditBufferData(binding.Buffer, reinterpret_cast<char *>(binding.Memory)));
			YDS_NESTED_ERROR_CALL(m_device->UseConstantBuffer(binding.Buffer, binding.Slot));
		}
	}

	return YDS_ERROR_RETURN(ysError::None);
}
