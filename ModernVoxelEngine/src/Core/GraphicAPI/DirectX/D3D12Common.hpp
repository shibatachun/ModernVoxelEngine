
#include "src/Core/GraphicAPI/IRenderer.h"

#include <dxgi1_6.h>
#include <d3d12.h>

#include <wrl.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#include "src/Core/GraphicAPI/RHIResource.h"
#include "src/Core/GraphicAPI/RHIGraphicResourceManager.h"
#include "src/Core/GraphicAPI/RHICommandBuffer.h"


namespace d3d12 {
    inline std::string ToString(HRESULT hr) {
        char* msgBuf = nullptr;

        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&msgBuf,
            0,
            nullptr
        );

        std::string msg = msgBuf ? msgBuf : "Unknown HRESULT";
        LocalFree(msgBuf);
        return msg;
    }

    inline void Check(HRESULT hr, const std::string& operation) {
        if (FAILED(hr)) {
            std::cerr << "[D3D12 Error] " << operation
                << " failed: " << ToString(hr) << "\n";
            throw std::runtime_error("D3D12: " + operation + " (" + ToString(hr) + ")");
        }
        else
        {
            std::cout << "[D3D12]::Success: do " << operation << " success!" << std::endl;
        }
    }

    template<typename T>
    constexpr void release(T*& resource) {
        if (resource) {
            resource->Release();
            resource = nullptr;
        }
    }

#ifdef _DEBUG
#define NAME_D3D12_OBJECT(obj, name) obj->SetName(name); OutputDebugString(L"::D3D12 Object Created: "); OutputDebugString(name); OutputDebugString(L"\n");
#else
#define NAME_D3D12_OBJECT(x, name)
#endif
}