#include "script.hpp"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <string>
#include "log.hpp"
#include "fileio.hpp"
#include "device.hpp"
#include "render.hpp"
#include "input.hpp"


#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace xxs::script
{
    sol::state lua;
    sol::function update_function;
    sol::function draw_function;

    void log_info(const std::string& message) { log::info(message); }

    void sol_check(sol::protected_function_result result)
    {
        if(!result.valid())
        {
            sol::error err = result;
            log::error(err.what());
        }
    }

    void bind()
    {
        // We only bind the log info function for now
        lua.set_function("log", log_info);

        // Bind the device module
        lua["device"] = lua.create_table();

        // Bind the render module
        lua["render"] =  lua.create_table();
        lua["render"]["load_image"] = render::load_image;
        lua["render"]["get_image_width"] = render::get_image_width;
        lua["render"]["get_image_height"] = render::get_image_height;
        lua["render"]["create_sprite"] = render::create_sprite;
        lua["render"]["create_sprite_pixels"] = render::create_sprite_pixels;
        lua["render"]["render_sprite"] = render::render_sprite_int;

        // Bind the input module
        lua["input"] = lua.create_table();
    }

    void initialize(const std::string& main_script)
    {
        lua.open_libraries(sol::lib::base, sol::lib::package);

        // Bind xxs API to lua
        bind();

        // Open some common libraries
        sol_check( lua.script_file(main_script) );

        // Call init function
        sol::function init_function = lua["init"];
        sol_check( init_function() );

        // Get update and render functions for later
        update_function = lua["update"];
        draw_function = lua["draw"];
    }

    void shutdown()
    {
        // Call shutdown function
        sol::function shutdown_function = lua["shutdown"];
        sol_check( shutdown_function() );
    }

    void update(double dt)
    {
        sol_check( update_function(dt) );

    }

    void render()
    {
        sol_check(draw_function() );
    }
}




