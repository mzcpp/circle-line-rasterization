#include "Circle.hpp"

#include "SDL2/SDL.h"

#include <iostream>
#include <cmath>

Circle::Circle(SDL_Renderer* renderer, SDL_Point center, int radius, SDL_Color color) : 
	renderer_(renderer), 
	center_(center), 
	radius_(radius), 
	color_(color), 
	pixel_color_(0), 
	texture_(nullptr)
{
	bbox_.x = center_.x - radius_;
	bbox_.y = center_.y - radius_;
	bbox_.w = 2 * radius_;
	bbox_.h = 2 * radius_;

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	pixel_color_ = (color_.b << 24) + (color_.g << 16) + (color_.r << 8) + 255;
	#else
	pixel_color_ = (255 << 24) + (color_.r << 16) + (color_.g << 8) + color_.b;
	#endif

	pixels_ = new Uint32[bbox_.w * bbox_.h];
	texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, bbox_.w, bbox_.h);
	SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_BLEND);

	//CreateCircleNaive();
	CreateCircleBresenham(std::rand() % 2);
}

Circle::~Circle()
{
	SDL_DestroyTexture(texture_);
	texture_ = nullptr;

	delete[] pixels_;
	pixels_ = nullptr;
}

void Circle::CreateCircleNaive()
{
	memset(pixels_, 0, bbox_.w * bbox_.h * sizeof(Uint32));

	const int cx = center_.x - bbox_.x;
	const int cy = center_.y - bbox_.y;
	const int radius_squared = radius_ * radius_;

	for (int x = 0; x < bbox_.w; ++x)
	{
		for (int y = 0; y < bbox_.h; ++y)
		{
			if ((x - cx) * (x - cx) + (y - cy) * (y - cy) - radius_squared <= 0)
			{
				pixels_[(y * bbox_.w) + x] = pixel_color_;
			}
		}		
	}

	SDL_UpdateTexture(texture_, nullptr, pixels_, bbox_.w * sizeof(Uint32));
}

void Circle::CreateCircleBresenham(bool filled)
{
	memset(pixels_, 0, bbox_.w * bbox_.h * sizeof(Uint32));

	int x = 0;
	int y = radius_;
	int d = 1 - radius_;

	while (x < y)
	{
		if (d < 0)
		{
			d = d + 2 * x + 3;
			++x;
		}
		else
		{
			d = d + 2 * (x - y) + 5;
			++x;
			--y;
		}

		pixels_[((radius_ - y) * bbox_.w) + (radius_ - x)] = pixel_color_;
		pixels_[((radius_ - y) * bbox_.w) + (radius_ - 1 + x)] = pixel_color_;
		pixels_[((radius_ - x) * bbox_.w) + (radius_ - y)] = pixel_color_;
		pixels_[((radius_ - x) * bbox_.w) + (radius_ - 1 + y)] = pixel_color_;
		pixels_[((radius_ - 1 + x) * bbox_.w) + (radius_ - y)] = pixel_color_;
		pixels_[((radius_ - 1 + x) * bbox_.w) + (radius_ - 1 + y)] = pixel_color_;
		pixels_[((radius_ - 1 + y) * bbox_.w) + (radius_ - x)] = pixel_color_;		
		pixels_[((radius_ - 1 + y) * bbox_.w) + (radius_ - 1 + x)] = pixel_color_;

		if (filled)
		{
			CreateCircleChordEFLA(radius_ - x, radius_ - y, radius_ - 1 + x, radius_ - y);
			CreateCircleChordEFLA(radius_ - y, radius_ - x, radius_ - 1 + y, radius_ - x);
			CreateCircleChordEFLA(radius_ - y, radius_ - 1 + x, radius_ - 1 + y, radius_ - 1 + x);
			CreateCircleChordEFLA(radius_ - x, radius_ - 1 + y, radius_ - 1 + x, radius_ - 1 + y);
		}
	}

	SDL_UpdateTexture(texture_, nullptr, pixels_, bbox_.w * sizeof(Uint32));
}

void Circle::CreateCircleChordEFLA(int x1, int y1, int x2, int y2)
{
	bool y_longer = false;
	int short_length = y2 - y1;
	int long_length = x2 - x1;

	if (std::abs(short_length) > std::abs(long_length))
	{
	    std::swap(short_length, long_length);
	    y_longer = true;
	}

	const int decInc = long_length == 0 ? 0 : ((short_length << 16) / long_length);

	if (y_longer) 
	{
	    y1 *= bbox_.w;
	    y2 *= bbox_.w;
	    
	    if (long_length > 0)
	    {
	        for (int j = 0x8000 + (x1 << 16); y1 <= y2; j += decInc)
	        {
	        	pixels_[y1 + (j >> 16)] = pixel_color_;
	        	y1 += bbox_.w;
	        }
	    }
	    else
	    {
	        for (int j = 0x8000 + (x1 << 16); y1 >= y2; j -= decInc)
	        {
	        	pixels_[y1 + (j >> 16)] = pixel_color_;
	        	y1 -= bbox_.w;
	        }
	    }
	}
	else
	{
	    if (long_length > 0)
	    {
	        for (int j = 0x8000 + (y1 << 16); x1 <= x2; j += decInc)
	        {
	        	pixels_[(j >> 16) * bbox_.w + x1] = pixel_color_;
	        	++x1;
	        }
	    }
	    else
	    {
	        for (int j = 0x8000 + (y1 << 16); x1 >= x2; j -= decInc)
	        {
	        	pixels_[(j >> 16) * bbox_.w + x1] = pixel_color_;
	        	--x1;
	        }
	    }
	}
}


void Circle::Tick()
{
}

void Circle::Render()
{
	SDL_SetRenderDrawColor(renderer_, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderCopy(renderer_, texture_, nullptr, &bbox_);

	SDL_Rect bbox_copy = bbox_;
	bbox_copy.x -= 1;
	bbox_copy.y -= 1;
	bbox_copy.w += 2;
	bbox_copy.h += 2;
	//SDL_RenderDrawRect(renderer_, &bbox_copy);
}
