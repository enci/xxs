#include "game.hpp"
#include <vector>
#include <cstdlib>
#include <glm/glm.hpp>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include "render.hpp"
#include "input.hpp"

using namespace game;
using namespace xxs;
using namespace std;
using namespace glm;

namespace game::internal
{
    render::sprite_handle ground_sprites [16][16];
    render::sprite_handle props_sprites [16][16];

    enum layers
    {
        ground = 0,
        props = 1,
        character = 1
    };

    // Returns a random integer between min and max (inclusive).
    int random_int(int min, int max)
    {
        // rand() is not a good random number generator,
        // but it's good enough for this example.
        return min + (rand() % static_cast<int>(max - min + 1));
    }

    render::sprite_handle create_tile_sprite(render::image_handle image, int index, int dx, int dy)
    {
        auto width = render::get_image_width(image);
        auto height = render::get_image_height(image);

        auto sx = width / dx;;

        auto x = index % static_cast<int>(sx);
        auto y = index / static_cast<int>(sx);

        return render::create_sprite(
                image,
                (dx * x) / static_cast<double>(width),
                (dy * y + dy) / static_cast<double>(height),
                (dx * x + dx) / static_cast<double>(width),
                (dy * y) / static_cast<double>(height));
    }

    render::sprite_handle create_tile_sprite(render::image_handle image, int index, int tile_size)
    {
        return create_tile_sprite(image, index, tile_size, tile_size);
    }

    std::vector<render::sprite_handle> create_tile_sprites_range(
            render::image_handle image, int from_index, int to_index, int tile_size)
    {
        std::vector<render::sprite_handle> sprites;
        for(int i = from_index; i <= to_index; ++i)
        {
            sprites.push_back(create_tile_sprite(image, i, tile_size));
        }
        return sprites;
    }

    class animation
    {
    public:
        void update(float dt)
        {
            frame_time += dt;
            if(frame_time > frame_duration)
            {
                frame_time = 0.0f;
                current_frame = (current_frame + 1) % sprites.size();
            }
        }

        render::sprite_handle get_current_sprite()
        {
            return sprites[current_frame];
        }

        std::vector<render::sprite_handle> sprites;

    private:
        int current_frame = 0;
        float frame_time = 0.0f;
        float frame_duration = 1.0f / 10.0f;
    };

    class character
    {
    public:
        character()
        {
            current_state = state::idle;
            current_direction = direction::right;

            auto image_h = render::load_image("assets/playerSprites_/fPlayer_ [human].png");
            auto tile_size = 32;

            idle_animation.sprites = create_tile_sprites_range(image_h, 8, 11, tile_size);
            walking_animation.sprites = create_tile_sprites_range(image_h, 16, 23, tile_size);
        }

        void update(float dt)
        {
            // Get the input.
            auto dx = input::get_axis(input::gamepad_axis::stick_left_x);
            auto dy = input::get_axis(input::gamepad_axis::stick_left_y);

            // Account for deadzone.
            auto deadzone = 0.2f;
            if(abs(dx) < deadzone)
                dx = 0.0f;
            if(abs(dy) < deadzone)
                dy = 0.0f;

            // Update the position.
            position_x += dx * speed * dt;
            position_y -= dy * speed * dt;

            // Update the state.
            if(dx == 0.0f && dy == 0.0f)
                current_state = state::idle;
            else
                current_state = state::walking;

            // Update the direction.
            if(dx < 0.0f)
                current_direction = direction::left;
            else if(dx > 0.0f)
                current_direction = direction::right;

            // Animate the character.
            switch(current_state)
            {
                case state::idle:
                    idle_animation.update(dt);
                    break;
                case state::walking:
                    walking_animation.update(dt);
                    break;
            }
        }

        void render()
        {
            auto sprite = render::sprite_handle();
            switch(current_state)
            {
                case state::idle:
                    sprite = idle_animation.get_current_sprite();
                    break;
                case state::walking:
                    sprite = walking_animation.get_current_sprite();
                    break;
            }

            int flip = 0;
            if(current_direction == direction::left)
                flip = render::flip_x;

            render::render_sprite(
                    sprite,
                    position_x,
                    position_y,
                    layers::character,
                    1,
                    0,
                    render::color{255, 255, 255, 255},
                    render::bottom | render::center_x | flip);

        }

        enum class state { idle, walking };
        enum class direction { left, right };
        state current_state;
        direction current_direction;
        animation idle_animation;
        animation walking_animation;
        double position_x = 0.0f;
        double position_y = 0.0f;
        double speed = 50.0f;
    };

    class character* player;
    vec2 camera_offset(0.0f, 0.0f);
}

void game::initialize()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    {   // Create the ground sprites
        auto image_h = render::load_image("assets/forest_/forest_.png");
        int tile_size = 16;

        int ids[] = {30, 31, 32, 33, 52, 53, 54, 55, 76, 77};
        for(int i = 0; i < 16; ++i)
        {
            for(int j = 0; j < 16; ++j)
            {
                auto sprite_idx = internal::random_int(0, sizeof(ids) / sizeof(int) - 1);
                sprite_idx = ids[sprite_idx];
                internal::ground_sprites[i][j] = internal::create_tile_sprite(
                        image_h, sprite_idx, tile_size);
            }
        }
    }

    {   // Add small props in the level
        auto image_h = render::load_image("assets/forest_/forest_ [resources].png");

        int ids[] = {6, 7, 8, 9, 20, 30, 23, 33, 24, 34, 25, 35, 26, 36, 27, 37, 28, 38, 29, 39 };
        for(int i = 0; i < 16; ++i)
        {
            for(int j = 0; j < 16; ++j)
            {
                if(internal::random_int(0, 100) > 30)
                    continue;

                auto sprite_idx = internal::random_int(0, sizeof(ids) / sizeof(int) - 1);
                sprite_idx = ids[sprite_idx];
                internal::props_sprites[i][j] = internal::create_tile_sprite(
                        image_h, sprite_idx, 16, 16);
            }
        }
    }

    {   // Add props in the level
        auto image_h = render::load_image("assets/forest_/forest_ [resources].png");

        int ids[] = {0, 1, 5};
        for(int i = 0; i < 16; ++i)
        {
            for(int j = 0; j < 16; ++j)
            {
                if(internal::random_int(0, 100) > 20)
                    continue;

                auto sprite_idx = internal::random_int(0, sizeof(ids) / sizeof(int) - 1);
                sprite_idx = ids[sprite_idx];
                internal::props_sprites[i][j] = internal::create_tile_sprite(
                        image_h, sprite_idx, 16, 32);
            }
        }
    }

    {
        internal::player = new class internal::character();
    }
}

void game::shutdown() {}

void game::update(float dt)
{
    // Move the character.
    auto speed = 100.0f;
    auto dx = input::get_axis(input::gamepad_axis::stick_left_x) * speed * dt;
    auto dy = input::get_axis(input::gamepad_axis::stick_left_y) * speed * dt;
    internal::player->update(dt);

    // Lerp the camera to the character.
    vec2 camera_target(internal::player->position_x, internal::player->position_y);
    internal::camera_offset = glm::mix(internal::camera_offset, camera_target, dt);
    render::set_offset(-internal::camera_offset.x, -internal::camera_offset.y);
}

void game::render()
{

    int tile_size = 16;

    // Render the character sprite.
    internal::player->render();

    // Get the player position in tile coordinates.
    auto player_x = static_cast<int>(internal::player->position_x / tile_size);
    auto player_y = static_cast<int>(internal::player->position_y / tile_size);

    // Render the ground sprites.
    for(int i = 0; i < 16; ++i)
    {
        for(int j = 0; j < 16; ++j)
        {
            auto color = render::color{255, 255, 255, 255};

            { // Render the ground

                auto sprite = internal::ground_sprites[i][j];
                render::render_sprite(
                        sprite,
                        i * tile_size - 128,
                        j * tile_size - 128,
                        internal::ground,
                        1,
                        0,
                        color,
                        render::bottom);
            }

            { // Render the props
                auto sprite = internal::props_sprites[i][j];
                render::render_sprite(
                        sprite,
                        i * tile_size - 128,
                        j * tile_size - 128,
                        internal::props,
                        1,
                        0,
                        color,
                        render::bottom);
            }
        }
    }
}
