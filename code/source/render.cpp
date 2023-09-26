#include "render.hpp"
#include <glm/glm.hpp>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include "device.hpp"
#include "log.hpp"

using namespace xxs;
using namespace std;
using namespace glm;

namespace xxs::render::internal
{
    struct image
    {
        SDL_Texture* texture = {};
		int	width = -1;
		int	height = -1;
		int	channels = -1;
		size_t string_id = 0;
		string file;
    };

    struct sprite
    {
		image_handle image_h = {};
		vec2 from = {};
		vec2 to	= {};
    };

    struct sprite_entry
    {
        sprite_handle sprite_h = {};
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        double scale = 1.0;
        double rotation = 0.0;
        color mul_color = {};
        color add_color = {};
        unsigned int flags = 0;
    };

    vector<sprite> sprites;
    vector<image> images;
    vector<sprite_entry> sprite_queue;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* planet_texture = nullptr;
}

void render::initialize()
{
    auto window = xxs::device::get_window();
    internal::renderer = SDL_CreateRenderer(window, NULL, 0);
    if (!internal::renderer)
    {
        xxs::log::error("SDL_CreateRenderer() failed: {}!", SDL_GetError());
        return;
    }

    // Load a texture using SDL Image
    internal::planet_texture = IMG_LoadTexture(internal::renderer, "content/Planet_02.png");
}

void render::shutdown()
{
    SDL_DestroyRenderer(internal::renderer);
}

void render::render()
{
    // Render the planet texture
    SDL_RenderTexture(internal::renderer, internal::planet_texture, NULL, NULL);
    SDL_RenderPresent(internal::renderer);
}

void render::clear()
{
    // Clear the screen to black
    SDL_SetRenderDrawColor(internal::renderer, 0, 0, 0, 255);
    SDL_RenderClear(internal::renderer);
}
	
render::image_handle render::load_image(const std::string& image_file)
{
    return render::image_handle();
}

int render::get_image_width(image_handle image_h)
{
    return 0;
}

int render::get_image_height(image_handle image_h)
{
    return 0;
}
	
render::sprite_handle render::create_sprite(
    image_handle image_h,
    double x0,
    double y0,
    double x1,
    double y1)
{
    render::sprite_handle handle;
    return handle;
}

void render::render_sprite(
	render::sprite_handle sprite_h,
	double x,
	double y,		
	double z,
	double size,
	double rotation,
	color mutiply,
	color add,
	unsigned int flags)
{

}