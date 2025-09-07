#define NOMINMAX
#include "yds_d3d11_utilities.h"

#include <d3d11.h>

void D3D11SetDebugName(ID3D11DeviceChild *object, const std::string &name) {
    if (object != nullptr && name.size() < std::numeric_limits<UINT>::max()) {
        const UINT len = static_cast<UINT>(name.size());
        object->SetPrivateData(WKPDID_D3DDebugObjectName, len, name.c_str());
    }
}
