#pragma once

#include "D3D12Classes.h"
namespace d3d12 {

	class DirectX12Renderer : public IRenderer
	{
		DirectX12Renderer();
		~DirectX12Renderer();

		virtual bool Init() override final;
		virtual void DrawFrame() override final;
		virtual void Cleanup() override final;
		API getAPI() const override { return API::VULKAN; }

	private:
		ID3D12Device8* main_device{ nullptr };
		IDXGIFactory7* dxgi_factory{ nullptr };

	};
}


