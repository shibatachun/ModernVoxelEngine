#pragma once
#include "src/utils/CommonUtileTemplate.h"

typedef void (*OsMessagesCallBack)(void* os_event, void* user_data);

struct WindowConfiguration {

	uint32_t             width;
	uint32_t             height;
	API					 api;

	const char*			 name;

}; // struct WindowConfiguration
struct Window
{
	void Init(void* configuration_);
	void Shutdown();
	void HandleOSMessages();
	void SetFullScreen(bool value);
	void RegisterOSMessagesCallback(OsMessagesCallBack callback, void* user_data_);
	void UnRegisterOsMessagesCallback(OsMessagesCallBack callback);
	void CenterMouse(bool dragging);
	void UpdateFPS();

	utils::vector<OsMessagesCallBack> os_messages_callbacks;
	utils::vector<void*> os_messages_callbacks_data;
	void* platform_handle = nullptr;
	bool requested_exit = false;
	bool resized = false;
	bool minimized = false;
	uint32_t width = 0;
	uint32_t height = 0;

	double fps_last_time = 0.0;
	double fps_time_accum = 0.0;
	uint32_t fps_frame_counter = 0;
	float fps = 0.0f;

	float dispaly_refresh = 1.0f / 60.0f;

};

