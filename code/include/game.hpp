#pragma once
#include "render.hpp"
#include <vector>
#include <glm/glm.hpp>

class animation
{
public:
    void update(float dt);
    xxs::render::sprite_handle get_current_sprite();
    std::vector<xxs::render::sprite_handle> sprites;
private:
    int current_frame = 0;
    float frame_time = 0.0f;
    float frame_duration = 1.0f / 10.0f;
};

class character
{
public:
    character();
    void update(float dt);
    void render();
    glm::vec2 get_position () const { return position; }
private:
    enum class state { idle, walking };
    enum class direction { left, right };
    state current_state;
    direction current_direction;
    animation idle_animation;
    animation walking_animation;
    glm::vec2 position {0.0f, 0.0f};
    float speed = 50.0f;
};

struct layers
{
    enum
    {
        ground = 0,
        stuff = 1,
    };
};

class game
{
public:
    game();
    void update(float dt);
    void render();
private:
    xxs::render::sprite_handle ground_sprites [16][16];
    xxs::render::sprite_handle props_sprites [16][16];
    glm::vec2 camera_offset {0.0f, 0.0f};
    character player;
};