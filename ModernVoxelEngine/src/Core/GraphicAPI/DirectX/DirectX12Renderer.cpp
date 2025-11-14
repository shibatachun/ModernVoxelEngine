#include "DirectX12Renderer.h"
namespace d3d12 {
	DirectX12Renderer::DirectX12Renderer() {

	}
	DirectX12Renderer::~DirectX12Renderer()
	{
	}
	bool DirectX12Renderer::Init()
	{
		if (main_device) {
			Cleanup();
		}
		uint32_t dxgi_factory_flags{ 0 };
#ifdef _DEBUG
		dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		Check(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&dxgi_factory)),"Create D3D12 factory");
		return true;
	}
	void DirectX12Renderer::DrawFrame()
	{
	}
	void DirectX12Renderer::Cleanup()
	{
	}
}