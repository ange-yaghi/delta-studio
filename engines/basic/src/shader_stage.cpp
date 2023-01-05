#include "../include/shader_stage.h"

dbasic::ShaderStage::ShaderStage() {
	m_device = nullptr;
	m_shaderProgram = nullptr;
	m_inputLayout = nullptr;
	m_type = Type::FullPass;
	m_name = "";
	m_objectDataSize = 0;
	m_clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_clearTarget = true;
	m_cullMode = ysDevice::CullMode::Back;
	m_enabled = true;
	m_passes = 1;
	m_flagBit = 0;

	for (int i = 0; i < OutputSlot::Count; ++i) {
		m_renderTarget[i] = nullptr;
	}
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
			auto *deleter = m_bufferBindings[i].Deleter;
			assert(deleter != nullptr);
			deleter(m_bufferBindings[i].Memory);
		}
	}

	m_bufferBindings.Destroy();

	return YDS_ERROR_RETURN(ysError::None);
}

bool dbasic::ShaderStage::DependsOn(ysRenderTarget *target) const {
    return false;
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

	const int textureCount = GetTextureInputCount();
	for (int i = 0; i < textureCount; ++i) {
		memcpy(writeBuffer, &m_textureInputs[i].Texture, sizeof(ysTexture *));
		writeBuffer += sizeof(ysTexture *);
	}
}

void dbasic::ShaderStage::ReadObjectData(const void *source) {
	const int bufferCount = GetBufferCount();
	const char *readBuffer = reinterpret_cast<const char *>(source);
	for (int i = 0; i < bufferCount; ++i) {
		if (m_bufferBindings[i].Type == ConstantBufferBinding::BufferType::ObjectData) {
			memcpy(m_bufferBindings[i].Memory, readBuffer, m_bufferBindings[i].Size);
			readBuffer += m_bufferBindings[i].Size;
		}
	}

	const int textureCount = GetTextureInputCount();
	for (int i = 0; i < textureCount; ++i) {
		memcpy(&m_textureInputs[i].Texture, readBuffer, sizeof(ysTexture *));
		readBuffer += sizeof(ysTexture *);
	}
}

ysError dbasic::ShaderStage::AddInput(ysRenderTarget *inputData, int slot) {
	YDS_ERROR_DECLARE("AddInput");

	Input &input = m_inputs.New();
	input.InputData = inputData;
	input.Slot = slot;

	return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderStage::SetInput(ysRenderTarget *renderTarget, int slot) {
	YDS_ERROR_DECLARE("SetInput");

	const int inputCount = m_inputs.GetNumObjects();
	for (int i = 0; i < inputCount; ++i) {
		if (m_inputs[i].Slot == slot) {
			m_inputs[i].InputData = renderTarget;
		}
	}

	return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderStage::AddTextureInput(int slot, TextureHandle *handle) {
	YDS_ERROR_DECLARE("AddTextureInput");

	*handle = m_textureInputs.GetNumObjects();
	TextureInput &input = m_textureInputs.New();
	input.Texture = nullptr;
	input.Slot = slot;

	m_objectDataSize += sizeof(ysTexture *);

	return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderStage::BindTexture(ysTexture *texture, TextureHandle handle) {
	YDS_ERROR_DECLARE("BindTexture");

	m_textureInputs[handle].Texture = texture;

	return YDS_ERROR_RETURN(ysError::None);
}

ysError dbasic::ShaderStage::BindScene() {
	YDS_ERROR_DECLARE("BindScene");
	
	for (int i = 0; i < OutputSlot::Count; ++i) {
		m_device->SetRenderTarget(m_renderTarget[i], i);
	}
	m_device->UseShaderProgram(m_shaderProgram);
	m_device->UseInputLayout(m_inputLayout);

	if (m_clearTarget) {
		m_device->ClearBuffers(m_clearColor.vec);
	}

	const int inputCount = GetInputCount();
	for (int i = 0; i < inputCount; ++i) {
		const Input &input = m_inputs[i];

		ysRenderTarget *inputRenderTarget = input.InputData;
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

	m_device->SetFaceCulling(true);
	m_device->SetFaceCullingMode(m_cullMode);

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

	const int textureCount = GetTextureInputCount();
	for (int i = 0; i < textureCount; ++i) {
		const TextureInput &textureInput = m_textureInputs[i];
		YDS_NESTED_ERROR_CALL(m_device->UseTexture(textureInput.Texture, textureInput.Slot));
	}

	return YDS_ERROR_RETURN(ysError::None);
}

dbasic::StageEnableFlags dbasic::ShaderStage::GetFlags() const {
	return (StageEnableFlags)0x1 << m_flagBit;
}

bool dbasic::ShaderStage::CheckFlags(StageEnableFlags flags) const {
	return (((StageEnableFlags)0x1 << m_flagBit) & flags) > 0;
}

ysVector dbasic::ShaderStage::GetClearColor() const {
	return ysMath::LoadVector(m_clearColor);
}

void dbasic::ShaderStage::SetClearColor(const ysVector &vector) {
	m_clearColor = ysMath::GetVector4(vector);
}
