#include "game.hpp"
#include <vector>
#include <cstdlib>
#include <glm/glm.hpp>
#include "render.hpp"
#include "input.hpp"

using namespace game;
using namespace xxs;
using namespace std;
using namespace glm;

namespace game::internal
{
    render::sprite_handle character_sprite;
    vec2 character_position;
    render::sprite_handle ground_sprites [16][16];


    // Returns a random integer between min and max (inclusive).
    int random_int(int min, int max)
    {
        // rand() is not a good random number generator,
        // but it's good enough for this example.
        return min + (rand() % static_cast<int>(max - min + 1));
    }


    render::sprite_handle create_tile_sprite(render::image_handle image, int index, int tile_size)
    {
        auto width = render::get_image_width(image);
        auto height = render::get_image_height(image);

        // The spritesheet is 16x16 tiles.
        auto dx = tile_size;
        auto dy = tile_size;
        auto sx = width / dx;
        auto sy = height / dy;

        auto x = index % static_cast<int>(sx);
        auto y = index / static_cast<int>(sx);

        return render::create_sprite(image, dx * x, dy * y, dx * x + dx, dy * y + dy);
    }
}

void game::initialize()
{
    {   // Create the ground sprites

        auto image_h = render::load_image("assets/forest_/forest_.png");
        auto width = render::get_image_width(image_h);
        auto height = render::get_image_height(image_h);
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

    {
        auto image_h = render::load_image("assets/playerSprites_/fPlayer_ [human].png");
        auto width = render::get_image_width(image_h);
        auto height = render::get_image_height(image_h);

        internal::character_sprite = internal::create_tile_sprite(image_h, 8, 32);
        internal::character_position = vec2(0, 0);
    }
}

void game::shutdown() {}

void game::update(float dt)
{
    // Move the character.
    auto speed = 100.0f;
    auto dx = input::get_axis(input::gamepad_axis::stick_left_x) * speed * dt;
    auto dy = input::get_axis(input::gamepad_axis::stick_left_y) * speed * dt;
    internal::character_position += vec2(dx, dy);
}

void game::render()
{
    // Render the ground sprites.
    int tile_size = 16;
    for(int i = 0; i < 16; ++i)
    {
        for(int j = 0; j < 16; ++j)
        {
            auto sprite = internal::ground_sprites[i][j];
            render::render_sprite(
                sprite,
                i * tile_size,
                j * tile_size,
                0,
                1,
                0,
                render::color{255, 255, 255, 255},
                render::bottom | render::center_x);      
        }
    }

    render::render_sprite(
        internal::character_sprite,
        internal::character_position.x,
        internal::character_position.y,
        0,
        1,
        0,
        render::color{255, 255, 255, 255},
        render::bottom | render::center_x);
}
