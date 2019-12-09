#ifndef YDS_RENDER_GEOMETRY_CHANNEL_H
#define YDS_RENDER_GEOMETRY_CHANNEL_H

#include "yds_base.h"

class ysRenderGeometryChannel : public ysObject
{

	friend class ysRenderGeometryFormat;

public:

	static const int MAX_NAME_LENGTH = 128;

	enum CHANNEL_FORMAT
	{

		CHANNEL_FORMAT_R32G32_FLOAT,
		CHANNEL_FORMAT_R32G32B32_FLOAT,
		CHANNEL_FORMAT_R32G32B32A32_FLOAT,
		CHANNEL_FORMAT_R32G32B32A32_UINT,
		CHANNEL_FORMAT_R32G32B32_UINT,
		CHANNEL_FORMAT_UNDEFINED

	};

public:

	ysRenderGeometryChannel();
	~ysRenderGeometryChannel();

	const char *	GetName()	const { return m_name; }
	int				GetOffset() const { return m_offset; }
	int				GetSize()	const { return GetFormatSize(m_format); }
	int				GetLength() const { return GetFormatLength(m_format); }
	CHANNEL_FORMAT	GetFormat() const { return m_format; }

	static int GetFormatSize(CHANNEL_FORMAT format)
	{

		switch(format)
		{

		case CHANNEL_FORMAT_R32G32_FLOAT:
			return 2 * sizeof(float);

		case CHANNEL_FORMAT_R32G32B32_FLOAT:
			return 3 * sizeof(float);

		case CHANNEL_FORMAT_R32G32B32A32_FLOAT:
			return 4 * sizeof(float);

		case CHANNEL_FORMAT_R32G32B32A32_UINT:
			return 4 * sizeof(unsigned int);

		case CHANNEL_FORMAT_R32G32B32_UINT:
			return 3 * sizeof(unsigned int);

		case CHANNEL_FORMAT_UNDEFINED:
		default:
			return 0;

		}

	}

	static int GetFormatLength(CHANNEL_FORMAT format)
	{

		switch(format)
		{

		case CHANNEL_FORMAT_R32G32_FLOAT:
			return 2;

		case CHANNEL_FORMAT_R32G32B32_FLOAT:
			return 3;

		case CHANNEL_FORMAT_R32G32B32A32_FLOAT:
			return 4;

		case CHANNEL_FORMAT_R32G32B32A32_UINT:
			return 4;

		case CHANNEL_FORMAT_R32G32B32_UINT:
			return 3;

		case CHANNEL_FORMAT_UNDEFINED:
		default:
			return 0;

		}

	}

protected:

	char			m_name[MAX_NAME_LENGTH];
	int				m_offset;
	CHANNEL_FORMAT	m_format;

};

#endif