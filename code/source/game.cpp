#include "game.hpp"
#include <vector>
#include <cstdlib>
#include "input.hpp"

using namespace xxs;
using namespace std;
using namespace glm;

namespace
{
    // Returns a random integer between min and max (inclusive).
    int random_int(int min, int max) {
        // rand() is not a good random number generator,
        // but it's good enough for this example.
        return min + (rand() % static_cast<int>(max - min + 1));
    }

    render::sprite_handle create_tile_sprite(render::image_handle image, int index, int dx, int dy) {
        auto width = render::get_image_width(image);
        auto height = render::get_image_height(image);

        auto sx = width / dx;

        auto x = index % static_cast<int>(sx);
        auto y = index / static_cast<int>(sx);

        return render::create_sprite(
                image,
                (dx * x) / static_cast<double>(width),
                (dy * y + dy) / static_cast<double>(height),
                (dx * x + dx) / static_cast<double>(width),
                (dy * y) / static_cast<double>(height));
    }

    render::sprite_handle create_tile_sprite(render::image_handle image, int index, int tile_size) {
        return create_tile_sprite(image, index, tile_size, tile_size);
    }

    std::vector<render::sprite_handle> create_tile_sprites_range(
            render::image_handle image, int from_index, int to_index, int tile_size) {
        std::vector<render::sprite_handle> sprites;
        for (int i = from_index; i <= to_index; ++i) {
            sprites.push_back(create_tile_sprite(image, i, tile_size));
        }
        return sprites;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Animation
////////////////////////////////////////////////////////////////////////////////
void animation::update(float dt)
{
    frame_time += dt;
    if(frame_time > frame_duration)
    {
        frame_time = 0.0f;
        current_frame = (current_frame + 1) % sprites.size();
    }
}

render::sprite_handle animation::get_current_sprite()
{
    return sprites[current_frame];
}

////////////////////////////////////////////////////////////////////////////////
// Character
////////////////////////////////////////////////////////////////////////////////
character::character()
{
    current_state = state::idle;
    current_direction = direction::right;

    auto image_h = render::load_image("assets/playerSprites_/fPlayer_ [human].png");
    auto tile_size = 32;

    idle_animation.sprites = create_tile_sprites_range(image_h, 8, 11, tile_size);
    walking_animation.sprites = create_tile_sprites_range(image_h, 16, 23, tile_size);
}


void character::update(float dt)
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
    position.x += (float)dx * speed * dt;
    position.y -= (float)dy * speed * dt;

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

void character::render()
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
            position.x,
            position.y,
            layers::stuff,
            1,
            0,
            render::color{255, 255, 255, 255},
            render::bottom | render::center_x | flip);

}

////////////////////////////////////////////////////////////////////////////////
// Game
////////////////////////////////////////////////////////////////////////////////
game::game()
{
    {   // Create the ground sprites
        auto image_h = render::load_image("assets/forest_/forest_.png");
        int tile_size = 16;

        int ids[] = {30, 31, 32, 33, 52, 53, 54, 55, 76, 77};
        for(int i = 0; i < 16; ++i)
        {
            for(int j = 0; j < 16; ++j)
            {
                auto sprite_idx = random_int(0, sizeof(ids) / sizeof(int) - 1);
                sprite_idx = ids[sprite_idx];
                ground_sprites[i][j] = create_tile_sprite(
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
                if(random_int(0, 100) > 30)
                    continue;

                auto sprite_idx = random_int(0, sizeof(ids) / sizeof(int) - 1);
                sprite_idx = ids[sprite_idx];
                props_sprites[i][j] = create_tile_sprite(
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
                if(random_int(0, 100) > 20)
                    continue;

                auto sprite_idx = random_int(0, sizeof(ids) / sizeof(int) - 1);
                sprite_idx = ids[sprite_idx];
                props_sprites[i][j] = create_tile_sprite(
                        image_h, sprite_idx, 16, 32);
            }
        }
    }
}

void game::update(float dt)
{
    // Move the character.
    auto speed = 100.0f;
    auto dx = input::get_axis(input::gamepad_axis::stick_left_x) * speed * dt;
    auto dy = input::get_axis(input::gamepad_axis::stick_left_y) * speed * dt;
    player.update(dt);

    // Lerp the camera to the character.
    auto player_position = player.get_position();
    vec2 camera_target(player_position.x, player_position.y);
    camera_offset = glm::mix(camera_offset, camera_target, dt);
    render::set_offset(-camera_offset.x, -camera_offset.y);
}

void game::render()
{

    int tile_size = 16;

    // Render the character sprite.
    player.render();

    // Get the player position in tile coordinates.
    auto player_tile_pos = player.get_position() / (float)tile_size;

    // Render the ground sprites.
    for(int i = 0; i < 16; ++i)
    {
        for(int j = 0; j < 16; ++j)
        {
            auto color = render::color{255, 255, 255, 255};

            { // Render the ground

                auto sprite = ground_sprites[i][j];
                render::render_sprite(
                        sprite,
                        i * tile_size - 128,
                        j * tile_size - 128,
                        layers::ground,
                        1,
                        0,
                        color,
                        render::bottom);
            }

            { // Render the props
                auto sprite = props_sprites[i][j];
                render::render_sprite(
                        sprite,
                        i * tile_size - 128,
                        j * tile_size - 128,
                        layers::stuff,
                        1,
                        0,
                        color,
                        render::bottom);
            }
        }
    }
}
