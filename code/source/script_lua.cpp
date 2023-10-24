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
        lua["render"]["set_offset"] = render::set_offset;
        lua["render"]["bottom"] = render::sprite_flags::bottom;
        lua["render"]["top"] = render::sprite_flags::top;
        lua["render"]["center_x"] = render::sprite_flags::center_x;
        lua["render"]["center_y"] = render::sprite_flags::center_y;
        lua["render"]["flip_x"] = render::sprite_flags::flip_x;
        lua["render"]["flip_y"] = render::sprite_flags::flip_y;
        lua["render"]["overlay"] = render::sprite_flags::overlay;
        lua["render"]["center"] = render::sprite_flags::center;

        // Bind the input module
        lua["input"] = lua.create_table();
        lua["input"]["button"] = lua.create_table();
        // Bind gamepad buttons
        lua["input"]["button"]["south"] = input::gamepad_button::south;
        lua["input"]["button"]["east"] = input::gamepad_button::east;
        lua["input"]["button"]["west"] = input::gamepad_button::west;
        lua["input"]["button"]["north"] = input::gamepad_button::north;
        lua["input"]["button"]["shoulder_left"] = input::gamepad_button::shoulder_left;
        lua["input"]["button"]["shoulder_right"] = input::gamepad_button::shoulder_right;
        lua["input"]["button"]["button_select"] = input::gamepad_button::button_select;
        lua["input"]["button"]["button_start"] = input::gamepad_button::button_start;
        lua["input"]["button"]["stick_left"] = input::gamepad_button::stick_left;
        lua["input"]["button"]["stick_right"] = input::gamepad_button::stick_right;
        lua["input"]["button"]["dpad_up"] = input::gamepad_button::dpad_up;
        lua["input"]["button"]["dpad_right"] = input::gamepad_button::dpad_right;
        lua["input"]["button"]["dpad_down"] = input::gamepad_button::dpad_down;
        lua["input"]["button"]["dpad_left"] = input::gamepad_button::dpad_left;
        // Bind gamepad axes
        lua["input"]["axis"] = lua.create_table();
        lua["input"]["axis"]["stick_left_x"] = input::gamepad_axis::stick_left_x;
        lua["input"]["axis"]["stick_left_y"] = input::gamepad_axis::stick_left_y;
        lua["input"]["axis"]["stick_right_x"] = input::gamepad_axis::stick_right_x;
        lua["input"]["axis"]["stick_right_y"] = input::gamepad_axis::stick_right_y;
        lua["input"]["axis"]["trigger_left"] = input::gamepad_axis::trigger_left;
        lua["input"]["axis"]["trigger_right"] = input::gamepad_axis::trigger_right;

        lua["input"]["get_axis"] = input::get_axis;
        lua["input"]["get_button"] = input::get_button;
        lua["input"]["get_button_once"] = input::get_button_once;
        lua["input"]["get_key"] = input::get_key;
        lua["input"]["get_key_once"] = input::get_key_once;
    }

    void initialize(const std::string& main_script)
    {
        lua.open_libraries(
                sol::lib::base,
                sol::lib::io,
                sol::lib::table,
                sol::lib::string,
                sol::lib::coroutine,
                sol::lib::debug,
                sol::lib::os,
                sol::lib::math,
                sol::lib::package);

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




