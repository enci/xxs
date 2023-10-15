#include "script.hpp"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <string>
#include "log.hpp"
#include "fileio.hpp"

namespace xxs::script
{
    lua_State* L = nullptr;

    void initialize(const std::string& main_script)
    {
        L = luaL_newstate();
        luaL_openlibs(L);
        if(luaL_dofile(L, main_script.c_str()))
        {
            log::error(lua_tostring(L, -1));
        }
    }

    void shutdown()
    {
        lua_close(L);
    }

    void update(double dt)
    {
        return;
        lua_getglobal(L, "update");
        lua_pushnumber(L, dt);
        if(lua_pcall(L, 1, 0, 0))
        {
            log::error(lua_tostring(L, -1));
        }
    }

    void render()
    {
        return;
        lua_getglobal(L, "render");
        if(lua_pcall(L, 0, 0, 0))
        {
            log::error(lua_tostring(L, -1));
        }
    }
}




