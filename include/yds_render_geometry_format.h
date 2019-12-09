#ifndef YDS_RENDER_GEOMETRY_FORMAT_H
#define YDS_RENDER_GEOMETRY_FORMAT_H

#include "yds_render_geometry_channel.h"

class ysRenderGeometryFormat : public ysObject
{

public:

	ysRenderGeometryFormat();
	~ysRenderGeometryFormat();

	int GetChannelCount() const;
	int GetFormatSize() const;

	void AddChannel(const char *name, int offset, ysRenderGeometryChannel::CHANNEL_FORMAT format);

	const ysRenderGeometryChannel *GetChannel(int index) 
	{ 

		ysRenderGeometryChannel *channel = m_channels.Get(index); 
		return (const ysRenderGeometryChannel *)channel;

	}

protected:

	ysDynamicArray<ysRenderGeometryChannel, 16> m_channels;
	int m_formatSize;

};

#endif