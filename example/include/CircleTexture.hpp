#ifndef CIRCLE_TEXTURE_HPP
#define CIRCLE_TEXTURE_HPP

#include <SDL2/SDL.h>

#include <iostream>
#include <cmath>

class CircleTexture
{
private:
	SDL_Renderer* renderer_;
	SDL_Point center_;
	int radius_;
	SDL_Rect bbox_;
	SDL_Color color_;
	Uint32 pixel_color_;
	Uint32* pixels_;
	SDL_Texture* texture_;

public:
	CircleTexture(SDL_Renderer* renderer, const SDL_Point& center, int radius, const SDL_Color& color) : 
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

        CreateCircleBresenham(false);
    }

	~CircleTexture()
    {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;

        delete[] pixels_;
        pixels_ = nullptr;
    }

	void CreateCircleBresenham(bool filled)
    {
	for (int i = 0; i < bbox_.w * bbox_.h; ++i)
	{
		pixels_[i] = 0;
	}

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

    void CreateCircleChordEFLA(int x1, int y1, int x2, int y2)
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

    void MoveTo(const SDL_Point& new_center)
    {
        center_ = new_center;
        bbox_.x = center_.x - radius_;
        bbox_.y = center_.y - radius_;
    }

    void Render()
    {
        bbox_.x = center_.x - radius_;
        bbox_.y = center_.y - radius_;

        SDL_Rect sdl_bbox = { bbox_.x, bbox_.y, bbox_.w, bbox_.h };
        SDL_RenderCopy(renderer_, texture_, nullptr, &sdl_bbox);
    }
};

#endif
