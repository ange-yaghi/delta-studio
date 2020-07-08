#include "../include/yds_d3d11_utilities.h"

#include <d3d11.h>

void D3D11SetDebugName(ID3D11DeviceChild *object, const std::string &name) {
    if (object != nullptr) {
        object->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
    }
}
