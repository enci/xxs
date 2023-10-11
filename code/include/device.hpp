#pragma once
#include <string>

struct SDL_Window;

namespace xxs::device
{
	void initialize(
        int width,
        int height,
        int scale,
        std::string& title);
	void shutdown();
	void swap_buffers();
	void poll_events();
    bool is_running();
	int get_width();
	int get_height();
    SDL_Window* get_window();
}