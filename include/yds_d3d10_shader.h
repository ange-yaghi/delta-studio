#if defined(_WIN64)

#ifndef YDS_D3D10_SHADER_H
#define YDS_D3D10_SHADER_H

#include "yds_shader.h"

#include <D3D10.h>

class ysD3D10Shader : public ysShader
{

    friend class ysD3D10Device;

public:

    ysD3D10Shader();
    virtual ~ysD3D10Shader();

protected:
    void *m_shaderByteCode;
    SIZE_T m_shaderSize;

    union
    {

        ID3D10VertexShader *    m_vertexShader;
        ID3D10PixelShader *        m_pixelShader;
        ID3D10GeometryShader *    m_geometryShader;

    };

};

#endif

#endif /* Windows */
