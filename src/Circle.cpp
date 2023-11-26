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
	pixels_(nullptr), 
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
			CreateCircleChordBresenham(radius_ - x, radius_ - y, radius_ - 1 + x, radius_ - y);
			CreateCircleChordBresenham(radius_ - y, radius_ - x, radius_ - 1 + y, radius_ - x);
			CreateCircleChordBresenham(radius_ - y, radius_ - 1 + x, radius_ - 1 + y, radius_ - 1 + x);
			CreateCircleChordBresenham(radius_ - x, radius_ - 1 + y, radius_ - 1 + x, radius_ - 1 + y);
		}
	}

	SDL_UpdateTexture(texture_, nullptr, pixels_, bbox_.w * sizeof(Uint32));
}

void Circle::CreateCircleChordBresenham(int x1, int y1, int x2, int y2)
{
	bool changed = false;
	int x = x1;
	int y = y1;

	int dx = std::abs(x2 - x1);
	int dy = std::abs(y2 - y1);

	int sign_x = 0;

	if (x2 - x1 > 0)
	{
		sign_x = 1;
	}
	else if (x2 - x1 < 0)
	{
		sign_x = -1;
	}
	else
	{
		sign_x = 0;
	}

	int sign_y = 0;

	if (y2 - y1 > 0)
	{
		sign_y = 1;
	}
	else if (y2 - y1 < 0)
	{
		sign_y = -1;
	}
	else
	{
		sign_y = 0;
	}

	if (dy > dx)
	{
		std::swap(dx, dy);
		changed = true;
	}

	float e = 2.0 * static_cast<float>(dy) - static_cast<float>(dx);

	for (int i = 1; i <= dx; ++i)
	{
		pixels_[y * bbox_.w + x] = pixel_color_;

		while (e >= 0)
		{
			if (changed)
			{
				++x;
			}
			else
			{
				++y;
			}

			e = e - 2.0 * dx;
		}

		if (changed)
		{
			y += sign_y;
		}
		else
		{
			x += sign_x;
		}

		e = e + 2.0 * dy;
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
