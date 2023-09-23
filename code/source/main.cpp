#include "log.hpp"
#include <string>
#include <map>
#include <json.hpp>
#include <fstream>
#include "fileio.hpp"
#include "device.hpp"

using namespace xxs;

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
    log::info("xxs starting up");
    auto config = xxs::fileio::read_text_file("config.json");
    auto cfg = parse_config(config);

    device::initialize(cfg.width, cfg.height, cfg.title);
    while(device::is_running())
    {
       device::poll_events();
       device::swap_buffers();
    }
    device::shutdown();
    
    return 0;
}
