#include "input.hpp"
#include <SDL.h>
#include <unordered_map>
#include "log.hpp"

using namespace xxs;
using namespace std;

namespace xxs::input::internal
{
    unordered_map<size_t, SDL_Gamepad*> gamepads;

    SDL_Gamepad* get_default_gamepad();
}

void input::initialize()
{
    SDL_Init(SDL_INIT_GAMEPAD);
}

void input::shutdown()
{
    for(auto j : internal::gamepads)
        SDL_CloseGamepad(j.second);
    SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
}

void input::process_event(const SDL_Event &event)
{
    // xxs::log::info("Processing event {}", event.type);
}

void input::update(double dt)
{
    int count;
    auto joysticks = SDL_GetJoysticks(&count);
    for(int i = 0; i < count; i++ )
    {
        if(internal::gamepads.find(i) == internal::gamepads.end())
        {
            auto gamepad = SDL_OpenGamepad(joysticks[i]);
            if(gamepad == nullptr)
                xxs::log::error("Failed to open gamepad {}", i);
            else
                internal::gamepads[i] = gamepad;
        }
    }
    SDL_free(joysticks);
}

double input::get_axis(gamepad_axis axis)
{
    auto gamepad = internal::get_default_gamepad();
    if(gamepad == nullptr)
        return 0.0;
    auto value = SDL_GetGamepadAxis(gamepad, (SDL_GamepadAxis)axis);
    return value / 32767.0;
}

bool input::get_button(gamepad_button button)
{
    auto gamepad = internal::get_default_gamepad();
    if(gamepad == nullptr)
        return false;

    auto value = SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)button);
    return value == 1;
}

bool input::get_button_once(gamepad_button button)
{
    return false;
}

bool input::get_key(int key)
{
    return false;
}

bool input::get_key_once(int key)
{
    return false;
}

SDL_Gamepad* input::internal::get_default_gamepad()
{
    if(internal::gamepads.size() > 0)
        return internal::gamepads[0];
    return nullptr;
}