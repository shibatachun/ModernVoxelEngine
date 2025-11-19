#pragma once
#include "src/utils/CommonUtileTemplate.h"
#include "src/utils/Key.h"

enum Mousebuttons {
	MOUSE_BUTTONS_NONE = -1,
	MOUSE_BUTTONS_LEFT = 0,
	MOUSE_BUTTONS_RIGHT,
	MOUSE_BUTTONS_MIDDLE,
	MOUSE_BUTTONS_COUNT
};

enum Device : uint8_t{
	DEVICE_KEYBOARD,
	DEVICE_MOUSE,
	DEVICE_GAMEPAD
};
enum DevicePart : uint8_t {
	DEVICE_PART_KEYBOARD,
	DEVICE_PART_MOUSE,
	DEVICE_PART_GAMEPAD_AXIS,
	DEVICE_PART_GAMEPAD_BUTTONS
};
enum BindingType : uint8_t {
	BINDING_TYPE_BUTTON,
	BINDING_TYPE_AXIS_1D,
	BINDING_TYPE_AXIS_2D,
	BINDING_TYPE_VECTOR_1D,
	BINDING_TYPE_VECTOR_2D,
	BINDING_TYPE_BUTTON_ONE_MOD,
	BINDING_TYPE_BUTTON_TWO_MOD
};

struct InputVector2 {
	float x;
	float y;
	
};
//empty for game pad future implement
struct Gamepad {

};

typedef uint32_t InputHandle;

struct InputBinding {
	BindingType		type;
	Device device;
	DevicePart device_part;
	uint8_t	action_map_index;

	uint16_t action_index;
	uint16_t button;

	float value = 0.0f;
	uint8_t is_composite;
	uint8_t is_part_of_composite;
	uint8_t repeat;
	float min_deadzone = 0.1f;
	float max_deadzone = 0.95f;

	InputBinding& set(BindingType type_, Device device_, DevicePart device_part_, uint16_t button_, uint8_t is_composite, uint8_t is_part_of_composite, uint8_t repeat);
	InputBinding& set_deadzones(float min, float max);
	InputBinding& set_handles(InputHandle action_map, InputHandle action);
};

struct InputAction {
	bool triggered() const;
	float read_value_1d() const;
	InputVector2 read_value_2d() const;
	InputVector2 value;
	InputHandle action_map;
	const char* name;
};
struct InputActionMap {
	const char* name;
	bool active;
};
struct  InputActionMapCreation
{
	const char* name;
	bool active;
};

struct InputActionCreation {
	InputHandle action;
};

struct InputBackend {
	void init();
	void shutdown();
	void get_mouse_state(InputVector2& position, uint8_t* buttons, uint32_t num_buttons);
	void on_event(void* event_, uint8_t* keys, uint32_t num_keys, bool& has_focus);
};

struct InputSystem
{
	static InputSystem* instance();
	void init();
	void shutdown();

	bool IsKeyDown(Key key);
	bool IsKeyJustPressed(Key key, bool repeat = false);
	bool IsKeyJustReleased(Key key);

	bool IsMouseDown(Mousebuttons button);
	bool IsMouseClicked(Mousebuttons button);
	bool IsMouseReleased(Mousebuttons button);
	bool IsMouseDragging(Mousebuttons button);

	void Update(float delta);
	void DebugUI();
	void NewFrame();
	void OnEvent(void* input_event_);
	bool IsTriggered(InputHandle action) const;
	float IsReadValue1D(InputHandle action) const;
	InputVector2 IsreadValue2D(InputHandle action) const;

	utils::vector<InputActionMap> action_maps;
	utils::vector<InputAction> actions;
	utils::vector<InputBinding> bindings;

	uint8_t Keys[KEY_COUNT];
	uint8_t previous_keys[KEY_COUNT];

	InputVector2 mouse_position;
	InputVector2 previous_mouse_postion;
	InputVector2 mouse_clicked_position[MOUSE_BUTTONS_COUNT];
	uint8_t	mouse_button[MOUSE_BUTTONS_COUNT];
	uint8_t previous_mouse_button[MOUSE_BUTTONS_COUNT];
	float mouse_drag_distance[MOUSE_BUTTONS_COUNT];

	bool has_focus;



};



