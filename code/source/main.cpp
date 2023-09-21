#include "log.hpp"
#include <string>
#include <map>
#include <json.hpp>
#include <fstream>
#include "fileio.hpp"

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
    xxs::log::info("width: {}, height: {}, title: {}", cfg.width, cfg.height, cfg.title);

    return 0;
}
