#pragma once

#include "D3D12Classes.h"
namespace d3d12 {

	class DirectX12Renderer : public IRenderer
	{
	public:
		DirectX12Renderer(void* window, PRESENTMODE presentmode, asset::AssetManager& assetManager);
		~DirectX12Renderer();

		virtual bool Init() override final;
		virtual void DrawFrame() override final;
		virtual void Cleanup() override final;
		API getAPI() const override { return API::DIRECTX; }
		void InitDevice();

	private:
		std::unique_ptr<D3D12Device>			_d3d_device;

	

	};
}


