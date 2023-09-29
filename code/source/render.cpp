#include "render.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
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
        color color = {};
        unsigned int flags = 0;
    };

    // This is a spatial hash that will be used to "id" the sprites
    size_t spatial_hash(const sprite& sprite);

    unordered_map<size_t, sprite> sprites;
    unordered_map<size_t, image> images;
    vector<sprite_entry> sprite_queue;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* planet_texture = nullptr;    

    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v)
    {
        // Taken from boost::hash_combine
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
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

    SDL_SetRenderScale(internal::renderer, 4.0f, 4.0f);
}

void render::shutdown()
{
    SDL_DestroyRenderer(internal::renderer);
}

void render::render()
{
    // Render the entries from the queue with SDL
    for(const auto& se : internal::sprite_queue)
    {
        // Get the sprite from the handle
        auto sprite = internal::sprites[se.sprite_h.id];

        // Get the image from the handle
        auto image = internal::images[sprite.image_h.id];

        // Check if the image is valid
        if (!image.texture)
            continue;

        // Fill in the rectangles
        SDL_FRect src_rect;
        src_rect.x = sprite.from.x;
        src_rect.y = sprite.from.y;
        src_rect.w = sprite.to.x - sprite.from.x;
        src_rect.h = sprite.to.y - sprite.from.y;

        // Fill the destination rectangle from the sprite entry
        SDL_FRect dst_rect;
        dst_rect.x = se.x;
        dst_rect.y = se.y;
        dst_rect.w = src_rect.w * se.scale;
        dst_rect.h = src_rect.h * se.scale;

        SDL_RenderTexture(internal::renderer, image.texture, &src_rect, &dst_rect);
    }

    SDL_RenderPresent(internal::renderer);

    // Clear the queue
    internal::sprite_queue.clear();    
}

void render::clear()
{
    // Clear the screen to black
    SDL_SetRenderDrawColor(internal::renderer, 0, 0, 0, 255);
    SDL_RenderClear(internal::renderer);
}
	
render::image_handle render::load_image(const std::string& image_file)
{
    // Get identifier for the image
    auto string_id = std::hash<std::string>{}(image_file);

    // Check if the image is already loaded
    auto it = internal::images.find(string_id);
    if (it != internal::images.end())
        return render::image_handle{it->second.string_id};
    
    // Create a new image
    render::internal::image image;

    // Load a texture using SDL Image
    image.texture = IMG_LoadTexture(internal::renderer, image_file.c_str());
    if (!image.texture)
    {
        xxs::log::error("IMG_LoadTexture() failed: {}!", SDL_GetError());
        return render::image_handle();
    }

    // Query the texture to get its width and height to use in rendering
    SDL_QueryTexture(image.texture, NULL, NULL, &image.width, &image.height);

    // Store the file name so we can use it to find the image later
    image.file = image_file;
    
    // Store the id
    image.string_id = string_id;

    // Store the image in our list of loaded images
    internal::images[string_id] = image;

    // Return the handle to the image
    return render::image_handle{string_id};
}

int render::get_image_width(image_handle image_h)
{
    // Get the image from the handle
    auto image = internal::images[image_h.id];

    // Check if the image is valid
    if (!image.texture)
        return 0;

    // Return the width
    return image.width;
}

int render::get_image_height(image_handle image_h)
{
    // Get the image from the handle
    auto image = internal::images[image_h.id];

    // Check if the image is valid
    if (!image.texture)
        return 0;

    // Return the height
    return image.height;
}
	
render::sprite_handle render::create_sprite(
    image_handle image_h,
    double x0,
    double y0,
    double x1,
    double y1)
{
    // Get the image from the handle
    auto image = internal::images[image_h.id];

    // Check if the image is valid
    if (!image.texture)
        return render::sprite_handle();

    // Create a new sprite
    render::internal::sprite sprite;

    // Store the image handle
    sprite.image_h = image_h;

    // Store the from and to coordinates
    sprite.from = vec2(x0, y0);
    sprite.to = vec2(x1, y1);

    // Calculate the hash
    auto hash = internal::spatial_hash(sprite);    

    // Store the sprite in our list of loaded sprites
    internal::sprites[hash] = sprite;

    // Return the handle to the sprite
    return render::sprite_handle{hash};
}

void render::render_sprite(
	render::sprite_handle sprite_h,
	double x,
	double y,		
	double z,
	double size,
	double rotation,
	color color,
	unsigned int flags)
{
    // Get the sprite from the handle
    auto sprite = internal::sprites[sprite_h.id];

    // Check if the sprite is valid
    if (!sprite.image_h.id)
        return;

    // Create a new sprite entry
    render::internal::sprite_entry entry;

    // Store the sprite handle
    entry.sprite_h = sprite_h;

    // Store the position
    entry.x = x;
    entry.y = y;
    entry.z = z;

    // Store the scale
    entry.scale = size;

    // Store the rotation
    entry.rotation = rotation;

    // Store the color
    entry.color = color;

    // Store the flags
    entry.flags = flags;

    // Add the sprite to the queue
    internal::sprite_queue.push_back(entry);
}

size_t render::internal::spatial_hash(const render::internal::sprite &sprite)
{
    // This will be the minimal resolution that the spatial hash will differentiate
    double resolution = 1000000.0;

    // Calculate the hash
    size_t hash = 0;
    hash_combine(hash, (int)(sprite.from.x * resolution));
    hash_combine(hash, (int)(sprite.from.y * resolution));
    hash_combine(hash, (int)(sprite.to.x * resolution));
    hash_combine(hash, (int)(sprite.to.y * resolution));
    hash_combine(hash, sprite.image_h.id);

    // Return the hash
    return hash;
}