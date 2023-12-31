#include "device.hpp"
#include <SDL.h>
#include "log.hpp"
#include "input.hpp"

using namespace xxs;
using namespace std;

namespace xxs::device::internal
{
    int width;
    int height;
    string title;
    SDL_Window* window = nullptr;
    bool running = true;
}

void device::initialize(int width, int height, int scale, std::string& title)
{
    internal::width = width * scale;
    internal::height = height * scale;
    internal::title = title;

    auto flags = 0; //SDL_WINDOWPOS_CENTERED_DISPLAY(1);
    SDL_Init(SDL_INIT_VIDEO);
    internal::window = SDL_CreateWindow(
        title.c_str(),       
        internal::width,
        internal::height,
        flags);
}

void device::shutdown()
{
    SDL_Quit();
}

void device::swap_buffers()
{
    SDL_GL_SwapWindow(internal::window);
    SDL_Delay(1);
}

void device::poll_events()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_EVENT_QUIT:
                internal::running = false;
        }

        input::process_event(event);
    }
}

bool device::is_running() { return internal::running; }

int device::get_width() { return internal::width; }

int device::get_height() { return internal::height; } 

SDL_Window* device::get_window() { return internal::window; }
