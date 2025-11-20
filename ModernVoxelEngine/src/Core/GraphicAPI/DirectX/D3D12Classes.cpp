#include "D3D12Classes.h"


namespace d3d12 {

	D3D12Device::D3D12Device() {
		Init();
	}
	D3D12Device::~D3D12Device() {
		shutdown();
	}
	bool D3D12Device::Init() {

		if (_main_device) {
			shutdown();
		}

		uint32_t dxgi_factory_flags{ 0 };

#ifdef _DEBUG
		{
			ComPtr<ID3D12Debug3> debug_interface;
			Check(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface)), "Create Debug interface");
			debug_interface->EnableDebugLayer();
			dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		Check(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&_dxgi_factory)), "Create Dxgi factory");

		ComPtr<IDXGIAdapter4> main_adapter;

		main_adapter.Attach(DetermineMainAdapter());

		if (!main_adapter) {
			throw std::runtime_error("Failed to aquire main adapter ");;
		}

		D3D_FEATURE_LEVEL max_feature_level{ GetMaxFeatureLevel(main_adapter.Get()) };

		assert(max_feature_level >= minimum_feature_level);

		if (max_feature_level < minimum_feature_level)
		{
			shutdown();
		}
		Check(D3D12CreateDevice(main_adapter.Get(), max_feature_level, IID_PPV_ARGS(&_main_device)), "Create main Device");

		NAME_D3D12_OBJECT(_main_device, L"Main D3D12 device");

#ifdef _DEBUG
	{
			ComPtr<ID3D12InfoQueue> info_queue;
			Check(_main_device->QueryInterface(IID_PPV_ARGS(&info_queue)), "Queue Interface");

			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	}
#endif
	}
	//获取最好的adapter,只要到minimum level
	IDXGIAdapter4* D3D12Device::DetermineMainAdapter()
	{
		IDXGIAdapter4* adapter{ nullptr };
		for (uint32_t i{ 0 };
			_dxgi_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
			++i) {
			if (SUCCEEDED(D3D12CreateDevice(adapter, minimum_feature_level, __uuidof(ID3D12Device), nullptr))) {
				return adapter;
			}
			release(adapter);
			
		}
		return nullptr;
	}

	D3D_FEATURE_LEVEL D3D12Device::GetMaxFeatureLevel(IDXGIAdapter4* adapter)
	{
		constexpr D3D_FEATURE_LEVEL feature_levels[4]{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_12_1,
		};
		D3D12_FEATURE_DATA_FEATURE_LEVELS feature_level_info{};
		feature_level_info.NumFeatureLevels = _countof(feature_levels);
		feature_level_info.pFeatureLevelsRequested = feature_levels; 

		ComPtr<ID3D12Device> device;
		Check(D3D12CreateDevice(adapter, minimum_feature_level, IID_PPV_ARGS(&device)), "Create D3D device");
		Check(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &feature_level_info, sizeof(feature_level_info)),"Check for support");
		return feature_level_info.MaxSupportedFeatureLevel;
	}

	void D3D12Device::shutdown() {
		release(_dxgi_factory);

#ifdef _DEBUG
		{
			ComPtr<ID3D12InfoQueue> info_queue;
			Check(_main_device->QueryInterface(IID_PPV_ARGS(&info_queue)), "Queue Interface");

			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);

			ComPtr<ID3D12DebugDevice2> debug_device;
			Check(_main_device->QueryInterface(IID_PPV_ARGS(&debug_device)), "Queue debug interface");
			release(_main_device);
			Check(debug_device->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL),"Report Live Device Objects");

		}
#endif
		release(_main_device);
	}
}