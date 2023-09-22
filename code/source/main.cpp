#include "log.hpp"
#include <string>
#include <map>
#include <json.hpp>
#include <fstream>
#include "fileio.hpp"
#include "SDL3/SDL.h"

struct config
{
    int width;
    int height;
    std::string title;
};

config parse_config(const std::string& config_str)
{
    config cfg;
    nlohmann::json j = nlohmann::json::parse(config_str);
    cfg.width = j["width"];
    cfg.height = j["height"];
    cfg.title = j["title"];
    return cfg;
}

int main()
{
    xxs::log::info("xxs starting up");
    auto config = xxs::fileio::read_text_file("config.json");
    auto cfg = parse_config(config);
    xxs::log::info("width: {}, height: {}, title: {}",
        cfg.width,
        cfg.height,
        cfg.title);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        cfg.title.c_str(),       
        cfg.width,
        cfg.height,
        SDL_WINDOW_OPENGL);

    bool running = true;
    while(running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_EVENT_QUIT:
                    running = false;
            }
        }
        
        SDL_GL_SwapWindow(window);
        SDL_Delay(1);
    }

    return 0;
}
