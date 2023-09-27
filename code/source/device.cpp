#include "device.hpp"
#include <SDL.h>
#include <SDL_opengl.h>
#include "log.hpp"

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

void device::initialize(int width, int height, std::string& title)
{
    internal::width = width;
    internal::height = height;
    internal::title = title;
    
    SDL_Init(SDL_INIT_VIDEO);
    internal::window = SDL_CreateWindow(
        title.c_str(),       
        width,
        height,
        NULL);
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

        // TODO: input::process_event(event)
    }
}

bool device::is_running() { return internal::running; }

int device::get_width() { return internal::width; }

int device::get_height() { return internal::height; } 

SDL_Window* device::get_window() { return internal::window; }
