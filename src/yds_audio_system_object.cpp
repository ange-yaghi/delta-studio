#include "../include/yds_audio_system_object.h"

ysAudioSystemObject::ysAudioSystemObject() : ysObject("AUDIO_SYSTEM_OBJECT")
{

	m_api = API_UNDEFINED;

}

ysAudioSystemObject::ysAudioSystemObject(const char *typeID, AUDIO_SYSTEM_API API) : ysObject(typeID)
{

	m_api = API;

}

ysAudioSystemObject::~ysAudioSystemObject()
{
}