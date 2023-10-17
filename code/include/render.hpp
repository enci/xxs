#pragma once
#include <string>
#include <cstdint>
#include "handle.hpp"

namespace xxs::render
{	    
    struct image_handle : public handle {};
    struct sprite_handle : public handle {};

	using  uchar = unsigned char;
	struct color
	{ 
		union
		{
			uchar rgba[4];
			struct { uchar a, b, g, r; };
			uint32_t integer_value;
		};
	};

	enum sprite_flags
	{
		bottom		= 1 << 1,
		top			= 1 << 2,
		center_x	= 1 << 3,
		center_y	= 1 << 4,
		flip_x		= 1 << 5,
		flip_y		= 1 << 6,
		overlay		= 1 << 7,
		center		= center_x | center_y
	};

	void initialize(int width, int height, int scale);
	void shutdown();
	void render();
	void clear();
	image_handle load_image(const std::string& image_file);
	int get_image_width(image_handle image_h);
	int get_image_height(image_handle image_h);
    void set_offset(double x, double y);
	sprite_handle create_sprite(
        image_handle image_h,
        double x0,
        double y0,
        double x1,
        double y1);
    sprite_handle create_sprite_pixels(
        image_handle image_h,
        int x0,
        int y0,
        int x1,
        int y1);
	void render_sprite(
		sprite_handle sprite_h,
		double x,
		double y,		
		double sort,
		double size,
		double rotation,
		color color,
		unsigned int flags);
    void render_sprite_int(
            sprite_handle sprite_h,
            double x,
            double y,
            double sort,
            double size,
            double rotation,
            uint32_t color,
            unsigned int flags);
}
