#include "DirectX12Renderer.h"
namespace d3d12 {
	DirectX12Renderer::DirectX12Renderer(void* window, PRESENTMODE presentmode, asset::AssetManager& assetManager) {
		Init();
	}
	DirectX12Renderer::~DirectX12Renderer()
	{
		Cleanup();
	}

	bool DirectX12Renderer::Init()
	{
		InitDevice();
		
		return true;
	}
	void DirectX12Renderer::DrawFrame()
	{
	}
	void DirectX12Renderer::Cleanup()
	{
	}
	void DirectX12Renderer::InitDevice()
	{
		_d3d_device.reset(new D3D12Device());
		
	}
}