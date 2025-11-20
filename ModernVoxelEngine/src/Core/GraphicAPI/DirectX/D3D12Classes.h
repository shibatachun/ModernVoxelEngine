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
		ID3D12Device8*		_main_device{ nullptr };
		IDXGIFactory7*		_dxgi_factory{ nullptr };
	

	private:
		
		IDXGIAdapter4* DetermineMainAdapter();
		D3D_FEATURE_LEVEL GetMaxFeatureLevel(IDXGIAdapter4* adapter);
	};

	class D3D12Command final {
		explicit D3D12Command(ID3D12Device8 *const device, D3D12_COMMAND_LIST_TYPE type) {
			D3D12_COMMAND_QUEUE_DESC desc{};
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 0;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.Type = type;
			Check(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_cmd_queue)),"Create Command Queue");
			NAME_D3D12_OBJECT(_cmd_queue,
				type == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"GFX Command Queue" :
				type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command Queue" : L"Command Queue");

			for (uint32_t i{ 0 }; i < frame_buffer_count; ++i) {
				CommandFrame& frame{ _cmd_frames[i] };
				Check(device->CreateCommandAllocator(type, IID_PPV_ARGS(&frame._cmd_allocator)), "Create Command Allocator");
				NAME_D3D12_OBJECT_INDEXED(frame._cmd_allocator,i,
					type == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"GFX Command Allocator" :
					type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command Allocator" : L"Command Allocator");
			}
			Check(device->CreateCommandList(0, type , _cmd_frames[0]._cmd_allocator, nullptr, IID_PPV_ARGS(&_cmd_list)), "Create Command List");

			Check(_cmd_list->Close(), "Close cmd list");

			NAME_D3D12_OBJECT(_cmd_queue,
				type == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"GFX Command List" :
				type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command List" : L"Command List");
		
		}
	private:
		struct CommandFrame {
			ID3D12CommandAllocator* _cmd_allocator{ nullptr };
			void wait();
			void Release() {
				release(_cmd_allocator);
			}
		};
		void BeginFrame() {
			CommandFrame& frame{ _cmd_frames[_frame_index] };
			frame.wait();
			frame._cmd_allocator->Reset();
			_cmd_list->Reset(frame._cmd_allocator, nullptr);
		}
		void EndFrame() {
			_cmd_list->Close();
			ID3D12CommandList *const cmd_lists[]{ _cmd_list };
			_cmd_queue->ExecuteCommandLists(_countof(cmd_lists), &cmd_lists[0]);
			_frame_index = (_frame_index + 1) % frame_buffer_count;
		}
		void Release();
		ID3D12CommandQueue*				_cmd_queue{ nullptr };
		ID3D12GraphicsCommandList6*		_cmd_list{ nullptr };
		CommandFrame					_cmd_frames[frame_buffer_count]{};
		uint32_t						_frame_index{ 0 };
	};
}


