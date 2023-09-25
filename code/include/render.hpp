#pragma once
#include <string>
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

	void initialize();
	void shutdown();	
	void render();
	void clear();
	image_handle load_image(const std::string& image_file);
	int get_image_width(image_handle image_h);
	int get_image_height(image_handle image_h);
	sprite_handle create_sprite(
        image_handle image_h,
        double x0,
        double y0,
        double x1,
        double y1);
	void render_sprite(
		sprite_handle sprite_h,
		double x,
		double y,		
		double z,
		double size,
		double rotation,
		color mutiply,
		color add,
		unsigned int flags);
}
