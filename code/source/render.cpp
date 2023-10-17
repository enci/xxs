#include "render.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/transform.hpp>
#include <unordered_map>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include "device.hpp"
#include "log.hpp"
#include "tools.hpp"

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
        double sort = 0.0;
        double scale = 1.0;
        double rotation = 0.0;
        xxs::render::color color = {};
        unsigned int flags = bottom;
    };

    // Binary compatible with SDL_Vertex, but we
    // can use vector operations on it
    struct vertex
    {
        vec2 position;
        xxs::render::color color;
        vec2 tex_coord;
    };

    // This is a spatial hash that will be used to "id" the sprites
    size_t spatial_hash(const sprite& sprite);

    unordered_map<size_t, sprite> sprites;
    unordered_map<size_t, image> images;
    vector<sprite_entry> sprite_queue;
    int width = -1;
    int height = -1;
    int scale = -1;
    double offset_x = 0.0;
    double offset_y = 0.0;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* planet_texture = nullptr;
    bool debug_sprites = false;

    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v)
    {
        // Taken from boost::hash_combine
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }

    SDL_Color to_sdl_color(xxs::render::color c)
    {
        SDL_Color color;
        color.r = c.r;
        color.g = c.g;
        color.b = c.b;
        color.a = c.a;
        return color;
    }

    SDL_FPoint to_sdl_fpoint(vec2 v)
    {
        SDL_FPoint point;
        point.x = v.x;
        point.y = v.y;
        return point;
    }

    void transform(vec2& v, mat3x3 m) { v = m * vec3(v, 1.0f); }
}

void render::initialize(int width, int height, int scale)
{
    // Store the width, height and scale
    internal::width = width;
    internal::height = height;
    internal::scale = scale;

    auto window = xxs::device::get_window();
    internal::renderer = SDL_CreateRenderer(window, NULL, 0);
    if (!internal::renderer)
    {
        xxs::log::error("SDL_CreateRenderer() failed: {}!", SDL_GetError());
        return;
    }

    SDL_SetRenderScale(internal::renderer, scale, scale);
}

void render::shutdown()
{
    SDL_DestroyRenderer(internal::renderer);
}

void render::render()
{
    // Get the screen width and height
    auto sw = internal::width / 2;
    auto sh = internal::height / 2;

    // Calculate a view-projection matrix to go from to SDL screen (pixel) coordinates
    mat3x3 vp = mat3x3(
            1.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            sw, sh, 1.0f);


    // Transform the origin
    vec2 origin(0.0f, 0.0f);
    internal::transform(origin, vp);

    // Sort the sprite entries by their sort value (or y if the sort is the same)
    std::stable_sort(internal::sprite_queue.begin(), internal::sprite_queue.end(),
        [](const render::internal::sprite_entry& a, const render::internal::sprite_entry& b)
        {
            if(a.sort == b.sort)
                return a.y > b.y;
            return a.sort < b.sort;
        });

    // Render the entries from the queue with SDL
    for(const auto& se : internal::sprite_queue)
    {
        // Get the sprite from the handle
        auto sprite = internal::sprites[se.sprite_h.id];

        // Get the image from the handle
        const auto& image = internal::images[sprite.image_h.id];

        // Check if the image is valid
        if (!image.texture)
            continue;

        // Vertex data
        std::array<internal::vertex, 4> vertices{};

        // Calculate the size of the sprite
        vec2 from(0.0f, 0.0);
        vec2 to(image.width * (sprite.to.x - sprite.from.x),
                -image.height * (sprite.to.y - sprite.from.y));

        // Check if the sprite is flipped
        if (tools::check_bit_flag_overlap(se.flags, xxs::render::sprite_flags::flip_x))
            std::swap(sprite.from.x, sprite.to.x);
        if (tools::check_bit_flag_overlap(se.flags, xxs::render::sprite_flags::flip_y))
            std::swap(sprite.from.y, sprite.to.y);

        // Calculate the anchor based on the flags
        vec2 anchor(0.0f, 0.0f);
        if (tools::check_bit_flag_overlap(se.flags, xxs::render::sprite_flags::center_x))
            anchor.x = (float)((to.x - from.x) * 0.5f);
        if (tools::check_bit_flag_overlap(se.flags, xxs::render::sprite_flags::center_y))
            anchor.y = (float)((to.y - from.y) * 0.5f);
        else if (tools::check_bit_flag_overlap(se.flags, xxs::render::sprite_flags::top))
            anchor.y = (float)(to.y - from.y);

        // Calculate the offset based on the flags
        vec3 offset = se.flags & xxs::render::sprite_flags::overlay ?
                vec3(0.0f, 0.0f, 0.0f) :
                vec3(-internal::offset_x, -internal::offset_y, 0.0f);
        offset.x = (float)std::round(offset.x * internal::scale) / internal::scale;
        offset.y = (float)std::round(offset.y * internal::scale) / internal::scale;

        // Top left
        vertices[0].position.x = from.x;
        vertices[0].position.y = from.y;
        vertices[0].tex_coord.x = sprite.from.x;
        vertices[0].tex_coord.y = sprite.from.y;

        // Top right
        vertices[1].position.x = to.x;
        vertices[1].position.y = from.y;
        vertices[1].tex_coord.x = sprite.to.x;
        vertices[1].tex_coord.y = sprite.from.y;

        // Bottom right
        vertices[2].position.x = to.x;
        vertices[2].position.y = to.y;
        vertices[2].tex_coord.x = sprite.to.x;
        vertices[2].tex_coord.y = sprite.to.y;

        // Bottom left
        vertices[3].position.x = from.x;
        vertices[3].position.y = to.y;
        vertices[3].tex_coord.x = sprite.from.x;
        vertices[3].tex_coord.y = sprite.to.y;

        // Apply the anchor and color
        for (auto& v : vertices)
        {
            v.position -= anchor;
            v.color = se.color;
        }

        // Calculate the rotation matrix
        mat3x3 rotation = glm::rotate(mat3x3(1.0f), (float)se.rotation);

        // Calculate the scale matrix
        mat3x3 scale = glm::scale(mat3x3(1.0f), vec2((float)se.scale));

        // Calculate the translation matrix
        mat3x3 translation = glm::translate(mat3x3(1.0f),
            vec2((float)se.x - offset.x, (float)se.y - offset.y));

        // Calculate the model matrix
        mat3x3 model = translation * scale * rotation;

        // Calculate the mvp matrix
        mat3x3 mvp = vp * model;

        // Transform the vertices
        for (auto& v : vertices)
            internal::transform(v.position, mvp);

        // Indices
        std::array<int, 6> indices = {0, 1, 2, 2, 3, 0};

        // This only works because internal::vertex is binary compatible with SDL_Vertex
        auto sdl_vertices = reinterpret_cast<SDL_Vertex*>(vertices.data());

        // Render the geometry
        SDL_RenderGeometry(
                internal::renderer,
                image.texture,
                sdl_vertices,
                vertices.size(),
                indices.data(),
                indices.size());

        if(!internal::debug_sprites)
            continue;

        // Render wireframe of the sprite
        SDL_SetRenderDrawColor(internal::renderer, 0, 0, 255, 255);
        SDL_RenderLine(internal::renderer,
                       vertices[0].position.x, vertices[0].position.y,
                       vertices[2].position.x, vertices[2].position.y);

        // Render a dot at the first vertex
        SDL_SetRenderDrawColor(internal::renderer, 255, 0, 0, 255);
        SDL_RenderPoint(internal::renderer, vertices[0].position.x, vertices[0].position.y);

        // Render a dot at the anchor
        SDL_SetRenderDrawColor(internal::renderer, 0, 255, 0, 255);
        auto anchor_point = vec2(0.0f, 0.0f);
        internal::transform(anchor_point, mvp);
        SDL_RenderPoint(internal::renderer, anchor_point.x, anchor_point.y);
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
    Uint32 format;
    SDL_QueryTexture(image.texture, &format, NULL, &image.width, &image.height);

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

void render::set_offset(double x, double y)
{
    internal::offset_x = x;
    internal::offset_y = y;
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
	double sort,
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
    entry.sort = sort;

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

render::sprite_handle render::create_sprite_pixels(
    render::image_handle image_h,
    int x0, int y0, int x1, int y1)
{
    // Get the image from the handle
    auto image = internal::images[image_h.id];

    return render::create_sprite(
        image_h,
        x0 / (double)image.width,
        y0 / (double)image.height,
        x1 / (double)image.width,
        y1 / (double)image.height);
}

void
render::render_sprite_int(
        render::sprite_handle sprite_h,
        double x,
        double y,
        double sort,
        double size,
        double rotation,
        uint32_t color,
        unsigned int flags)
{
    render::color c;
    c.integer_value = color;
    render::render_sprite(sprite_h, x, y, sort, size, rotation, c, flags);
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