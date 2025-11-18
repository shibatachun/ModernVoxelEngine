#include "InputSystem.h"
#include <SDL2/SDL.h>
static constexpr float k_mouse_drag_min_distance = 4.f;
InputBinding& InputBinding::set(BindingType type_, Device device_, DevicePart device_part_, uint16_t button_, uint8_t is_composite_, uint8_t is_part_of_composite_, uint8_t repeat_)
{
    type = type_;
    device = device_;
    device_part = device_part_;
    button = button_;
    is_composite = is_composite_;
    is_part_of_composite = is_part_of_composite_;
    repeat = repeat_;
    return *this;
}
static uint32_t to_sdl_mouse_button(Mousebuttons button) {
    switch (button) {
    case MOUSE_BUTTONS_LEFT:
        return SDL_BUTTON_LEFT;
    case MOUSE_BUTTONS_MIDDLE:
        return SDL_BUTTON_MIDDLE;
    case MOUSE_BUTTONS_RIGHT:
        return SDL_BUTTON_RIGHT;
    }

    return UINT_MAX;
}

InputBinding& InputBinding::set_deadzones(float min_, float max_)
{
    min_deadzone = min_;
    max_deadzone = max_;
    return *this;
}
void InputBackend::init() {

}
void InputBackend::shutdown() {

}
void InputBackend::get_mouse_state(InputVector2& position, uint8_t* buttons, uint32_t num_buttons) {
    int32_t mouse_x, mouse_y;
    uint32_t mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    for (uint32_t i = 0; i < num_buttons; ++i) {
        uint32_t sdl_button = to_sdl_mouse_button((Mousebuttons)i);
        buttons[i] = mouse_buttons & SDL_BUTTON(sdl_button);
    }
    position.x = (float)mouse_x;
    position.y = (float)mouse_y;
}

void InputBackend::on_event(void* event_, uint8_t* keys, uint32_t num_keys, bool& has_focus)
{
    SDL_Event& event = *(SDL_Event*)event_;
    switch (event.type)
    {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
        int32_t key = event.key.keysym.scancode;
        if (key >= 0 && key < (int32_t)num_keys) {
            keys[key] = (event.type == SDL_KEYDOWN);
            
        }
        break;
    }
    case SDL_WINDOWEVENT:
    {
        switch (event.window.event)
        {
            case SDL_WINDOWEVENT_FOCUS_GAINED:
            {
                has_focus = true;
                break;
            }
          
        
            case SDL_WINDOWEVENT_FOCUS_LOST:
            {
                has_focus = false;
                break;
            }

        default:
            break;
        }
    }
    default:
        break;
    }
}

InputBinding& InputBinding::set_handles(InputHandle action_map_, InputHandle action_)
{
    assert(action_map_ < 256);
    assert(action_ < 16636);

    action_map_index = (uint8_t)action_map_;
    action_index = (uint16_t)action_;

    return *this;
    // TODO: insert return statement here
}

bool InputAction::triggered() const
{
    return value.x != 0.0f;
}

float InputAction::read_value_1d() const
{
    return value.x;
}

InputVector2 InputAction::read_value_2d() const
{
    return value;
}


static InputBackend s_input_backend;
static InputSystem s_input_system;

InputSystem* InputSystem::instance() {
    return &s_input_system;
}
void InputSystem::init() {
    action_maps.reserve(16);
    actions.reserve(64);
    bindings.reserve(256);
    memset(Keys, 0, KEY_COUNT);
    memset(previous_keys, 0, KEY_COUNT);
    memset(mouse_button, 0, MOUSE_BUTTONS_COUNT);
    memset(previous_mouse_button, 0, MOUSE_BUTTONS_COUNT);
    s_input_backend.init();
}

void InputSystem::shutdown() {

}

bool InputSystem::IsKeyDown(Key key)
{
    return Keys[key] && has_focus;
}

bool InputSystem::IsKeyJustPressed(Key key, bool repeat)
{
    return false;
}

bool InputSystem::IsKeyJustReleased(Key key)
{
    return false;
}

bool InputSystem::IsMouseDown(Mousebuttons button)
{
    return mouse_button[button];
}

bool InputSystem::IsMouseClicked(Mousebuttons button)
{
    return false;
}

bool InputSystem::IsMouseReleased(Mousebuttons button)
{
    return false;
}

bool InputSystem::IsMouseDragging(Mousebuttons button)
{
    if (!mouse_button[button]) {
        return false;
    }
    return mouse_drag_distance[button] > k_mouse_drag_min_distance;
}

void InputSystem::Update(float delta)
{
    previous_mouse_postion = mouse_position;
    s_input_backend.get_mouse_state(mouse_position, mouse_button, MOUSE_BUTTONS_COUNT);
    for (uint32_t i = 0; i < MOUSE_BUTTONS_COUNT; ++i) {
        if (IsMouseClicked((Mousebuttons)i)) {
            mouse_clicked_position[i] = mouse_position;
        }
        else if (IsMouseDown((Mousebuttons)i)) {
            float delta_x = mouse_position.x - mouse_clicked_position[i].x;
            float delta_y = mouse_position.y - mouse_clicked_position[i].y;
            mouse_drag_distance[i] = sqrtf((delta_x * delta_x) + (delta_y * delta_y));
        }
    }
    for (uint32_t j = 0; j < actions.size(); j++) {
        InputAction& input_action = actions[j];
        input_action.value = { 0,0 };
    }
    //读取所有设备的输入值
    for (uint32_t k = 0; k < bindings.size(); k++) {
        InputBinding& input_binding = bindings[k];
        if (input_binding.is_composite) {
            continue;
        }
        input_binding.value = false;
        switch (input_binding.device)
        {
        case DEVICE_KEYBOARD:
        {
            bool key_value = input_binding.repeat ? IsKeyDown((Key)input_binding.button) : IsKeyJustPressed((Key)input_binding.button, false);
            input_binding.value = key_value ? 1.0f : 0.0f;
            break;
        }
        default:
            break;
        }
    }

    for (uint32_t k = 0; k < bindings.size(); k++) {
        InputBinding& input_binding = bindings[k];
        if (input_binding.is_part_of_composite) {
            continue;
        }
        InputAction& input_action = actions[input_binding.action_index];

        switch (input_binding.type) {
        case BINDING_TYPE_BUTTON:
        {
            input_action.value.x = fmax(input_action.value.x, input_binding.value ? 1.0f : 0.0f);
            break;
        }
        case BINDING_TYPE_AXIS_1D:
        {
            input_action.value.x = input_binding.value != 0.f ? input_binding.value : input_action.value.x;
            break;
        }
        case BINDING_TYPE_AXIS_2D:
        {
            InputBinding& input_binding_x = bindings[++k];
            InputBinding& input_binding_y = bindings[++k];
            input_action.value.x = input_binding_x.value != 0.0f ? input_binding_x.value : input_action.value.x;
            input_action.value.y = input_binding_y.value != 0.0f ? input_binding_y.value : input_action.value.y;
        }
        case BINDING_TYPE_VECTOR_1D:
        {
            // Retrieve following 2 bindings
            InputBinding& input_binding_pos = bindings[++k];
            InputBinding& input_binding_neg = bindings[++k];

            input_action.value.x = input_binding_pos.value ? input_binding_pos.value : input_binding_neg.value ? -input_binding_neg.value : input_action.value.x;
            break;
        }
        case BINDING_TYPE_VECTOR_2D:
        {
            // Retrieve following 4 bindings
            InputBinding& input_binding_up = bindings[++k];
            InputBinding& input_binding_down = bindings[++k];
            InputBinding& input_binding_left = bindings[++k];
            InputBinding& input_binding_right = bindings[++k];

            input_action.value.x = input_binding_right.value ? 1.0f : input_binding_left.value ? -1.0f : input_action.value.x;
            input_action.value.y = input_binding_up.value ? 1.0f : input_binding_down.value ? -1.0f : input_action.value.y;
            break;
        }
        }
        
    
    }
}

void InputSystem::DebugUI()
{
}

void InputSystem::NewFrame()
{
    for (uint32_t i = 0; i < KEY_COUNT; ++i) {
        previous_keys[i] = Keys[i];
    }
    for (uint32_t i = 0; i < MOUSE_BUTTONS_COUNT; ++i) {
        previous_mouse_button[i] = mouse_button[i];
    }
    //TODO::left for game pad design 
}

void InputSystem::OnEvent(void* input_event_)
{
    s_input_backend.on_event(input_event_, Keys, KEY_COUNT, has_focus);
}

bool InputSystem::IsTriggered(InputHandle action) const
{
    assert(action < actions.size());
    return actions[action].triggered();
}

float InputSystem::IsReadValue1D(InputHandle action) const
{
    assert(action < actions.size());
    return actions[action].read_value_1d();
}

InputVector2 InputSystem::IsreadValue2D(InputHandle action) const
{
    assert(action < actions.size());
    return actions[action].read_value_2d();
}
