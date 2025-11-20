#pragma once
#include "D3D12Common.hpp"
using namespace Microsoft::WRL;
namespace d3d12
{
	
	class D3D12Device final {
	public:
		D3D12Device();
		~D3D12Device();
		bool Init();
		void shutdown();
	private:
		static constexpr D3D_FEATURE_LEVEL minimum_feature_level{ D3D_FEATURE_LEVEL_11_0 };
		ID3D12Device8* _main_device{ nullptr };
		IDXGIFactory7* _dxgi_factory{ nullptr };
	

	private:
		
		IDXGIAdapter4* DetermineMainAdapter();
		D3D_FEATURE_LEVEL GetMaxFeatureLevel(IDXGIAdapter4* adapter);
	};
}


