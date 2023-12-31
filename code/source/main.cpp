#include "log.hpp"
#include <string>
#include <map>
#include <json.hpp>
#include "fileio.hpp"
#include "device.hpp"
#include "render.hpp"
#include "input.hpp"
#include "game.hpp"
#include "script.hpp"
#include "log.hpp"

using namespace xxs;

struct config
{
    int width;
    int height;
    int scale;
    std::string title;
    std::string main_script;
};

config parse_config(const std::string& config_str)
{
    config cfg;
    nlohmann::json j = nlohmann::json::parse(config_str);
    cfg.width = j["width"];
    cfg.height = j["height"];
    cfg.title = j["title"];
    cfg.scale = j["scale"];
    cfg.main_script = j["script"];
    return cfg;
}

double get_delta_time()
{
    // Gets delta time in seconds, using std::chrono
    static auto start = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
    start = now;
    return delta / 1000000.0;
}

int main()
{
    // Load config
    log::info("xxs starting up");
    auto config = xxs::fileio::read_text_file("config.json");
    auto cfg = parse_config(config);

    // Initialize xxs modules in order
    device::initialize(cfg.width, cfg.height, cfg.scale,cfg.title);
    render::initialize(cfg.width, cfg.height, cfg.scale);
    input::initialize();
    script::initialize(cfg.main_script);
    game game;

    // Main loop
    while(device::is_running())
    {
        auto dt = get_delta_time();
        device::poll_events();
        input::update(dt);
        script::update(dt);
        script::render();
        //game.update((float)dt);
        //game.render();
        render::clear();   
        render::render();
        device::swap_buffers();
    }

    // Shutdown in reverse order
    script::shutdown();
    input::shutdown();
    render::shutdown();
    device::shutdown();

    return 0;
}
