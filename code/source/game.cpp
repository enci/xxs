#include "game.hpp"
#include <vector>
#include <cstdlib>
#include <glm/glm.hpp>
#include "render.hpp"

using namespace game;
using namespace xxs;
using namespace std;
using namespace glm;

namespace game::internal
{
    vector<render::sprite_handle> ground_sprites;    
    render::sprite_handle character_sprite;
    vec2 character_position;

    const int tile_size = 16;

    // Returns a random integer between min and max (inclusive).
    int random_int(int min, int max)
    {
        // rand() is not a good random number generator,
        // but it's good enough for this example.
        return min + (rand() % static_cast<int>(max - min + 1));
    }


    render::sprite_handle create_tile_sprite(render::image_handle image, int index)
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
    auto image_h = render::load_image("assets/forest_/forest_.png");
    auto width = render::get_image_width(image_h);
    auto height = render::get_image_height(image_h);

    // The spritesheet is 16x16 tiles.
    auto dx = 16.0;
    auto dy = 16.0;
    auto sx = width / dx;
    auto sy = height / dy;

    // Create a sprite for somes tiles in the spritesheet.
    internal::ground_sprites.push_back(
        internal::create_tile_sprite(image_h, 30));

    internal::ground_sprites.push_back(
        internal::create_tile_sprite(image_h, 31));

    internal::ground_sprites.push_back(
        internal::create_tile_sprite(image_h, 32));

    internal::ground_sprites.push_back(
        internal::create_tile_sprite(image_h, 33));
    

    /*
    auto sprite_h = render::create_sprite(image_h, 0, 0, width, height);
    internal::ground_sprites.push_back(sprite_h);    

    internal::character_sprite = render::create_sprite(image_h, 0, 0, width, height);
    internal::character_position = vec2(0, 0);
    */
}

void game::shutdown() {}

void game::update(float dt)
{
}

void game::render()
{
    // Render the ground sprites.
    for(int i = 0; i < 10; ++i)
    {
        for(int j = 0; j < 1; ++j) 
        {
            auto sprite_idx = (i + j) % internal::ground_sprites.size();
            render::render_sprite(
                internal::ground_sprites[sprite_idx],
                i * internal::tile_size,
                j * internal::tile_size,
                0,
                1,
                0,
                render::color{255, 255, 255, 255},
                render::bottom | render::center_x);      
        }
    }

    /*
    render::render_sprite(
        internal::character_sprite,
        internal::character_position.x,
        internal::character_position.y,
        0,
        1,
        0,
        render::color{255, 255, 255, 255},
        render::bottom | render::center_x);
    */
}
