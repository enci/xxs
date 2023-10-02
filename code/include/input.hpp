#pragma once

union SDL_Event;

namespace xxs::input
{
    void initialize();
    void shutdown();
    void process_event(const SDL_Event& event);
    void update(double dt);

    enum class gamepad_button
    {
        south = 0,
        east = 1,
        west = 2,
        north = 3,
        shoulder_left = 4,
        shoulder_right = 5,
        button_select = 6,
        button_start = 7,
        stick_left = 9,
        stick_right = 10,
        dpad_up = 11,
        dpad_right = 12,
        dpad_down = 13,
        dpad_left = 14
    };

    enum class gamepad_axis
    {
        stick_left_x = 0,
        stick_left_y = 1,
        stick_right_x = 2,
        stick_right_y = 3,
        trigger_left = 4,
        trigger_right = 5
    };

    enum class mouse_button
    {
        left = 0,
        right = 1,
        middle = 2
    };

    double get_axis(gamepad_axis axis);
    bool get_button(gamepad_button button);
    bool get_button_once(gamepad_button button);
    bool get_key(int key);
    bool get_key_once(int key);
}
