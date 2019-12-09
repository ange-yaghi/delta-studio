#ifndef YDS_AUDIO_SYSTEM_OBJECT_H
#define YDS_AUDIO_SYSTEM_OBJECT_H

#include "yds_base.h"

class ysAudioSystemObject : public ysObject
{

public:

	enum AUDIO_SYSTEM_API
	{

		API_UNDEFINED,
		API_DIRECT_SOUND8,

	};

public:

	ysAudioSystemObject();
	ysAudioSystemObject(const char *typeID, AUDIO_SYSTEM_API API);
	virtual ~ysAudioSystemObject();

	AUDIO_SYSTEM_API GetAPI() const { return m_api; }

	bool CheckCompatibility(ysAudioSystemObject *object) const { return (object) ? object->m_api == m_api : true; }

private:

	AUDIO_SYSTEM_API m_api;

};

#endif