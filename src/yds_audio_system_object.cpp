#include "../include/yds_audio_system_object.h"

ysAudioSystemObject::ysAudioSystemObject() : ysObject("AUDIO_SYSTEM_OBJECT") {
    m_api = API::Undefined;
}

ysAudioSystemObject::ysAudioSystemObject(const char *typeID, API api) : ysObject(typeID) {
    m_api = api;
}

ysAudioSystemObject::~ysAudioSystemObject() {
    /* void */
}
