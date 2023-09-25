#include "render.hpp"
#include <glm/glm.hpp>
#include <SDL3/SDL_render.h>
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
}

void render::shutdown()
{
    SDL_DestroyRenderer(internal::renderer);
}

void render::render()
{
    /* fade between shades of red every 3 seconds, from 0 to 255. */
    static Uint8 r = 0;
    r = (r + 1) % 255;
    SDL_SetRenderDrawColor(internal::renderer, r, 0, 0, 255);

    /* you have to draw the whole window every frame. Clearing it makes sure the whole thing is sane. */
    SDL_RenderClear(internal::renderer);  /* clear whole window to that fade color. */

    /* set the color to white */
    SDL_SetRenderDrawColor(internal::renderer, 255, 255, 255, 255);

    /* draw a square where the mouse cursor currently is. */
    SDL_FRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 200;
    rect.h = 100;
    SDL_RenderFillRect(internal::renderer, &rect);

    /* put everything we drew to the screen. */
    SDL_RenderPresent(internal::renderer);

}

void render::clear()
{
    int temp = 0;
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