#pragma once

union SDL_Event;

namespace xxs::input
{
    void initialize();
    void shutdown();
    void process_event(const SDL_Event& event);
    void update(double dt);

    enum gamepad_button
    {
        BUTTON_SOUTH = 0,
        BUTTON_EAST = 1,
        BUTTON_WEST = 2,
        BUTTON_NORTH = 3,

        SHOULDER_LEFT = 4,
        SHOULDER_RIGHT = 5,

        BUTTON_SELECT = 6,
        BUTTON_START = 7,

        // Button 8 is not used

        STICK_LEFT = 9,
        STICK_RIGHT = 10,

        DPAD_UP = 11,
        DPAD_RIGHT = 12,
        DPAD_DOWN = 13,
        DPAD_LEFT = 14
    };

    enum gamepad_axis
    {
        STICK_LEFT_X = 0,
        STICK_LEFT_Y = 1,
        STICK_RIGHT_X = 2,
        STICK_RIGHT_Y = 3,
        TRIGGER_LEFT = 4,
        TRIGGER_RIGHT = 5
    };

    enum mouse_button {

        MOUSE_BUTTON_LEFT = 0,
        MOUSE_BUTTON_RIGHT = 1,
        MOUSE_BUTTON_MIDDLE = 2
    };

    double get_axis(gamepad_axis axis);
    bool get_button(gamepad_button button);
    bool get_button_once(gamepad_button button);
    bool get_key(int key);
    bool get_key_once(int key);
}
