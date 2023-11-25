#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include "SDL2/SDL.h"

class Circle
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
	Circle(SDL_Renderer* renderer, SDL_Point center, int radius, SDL_Color color);

	~Circle();

	void CreateCircleNaive();

	void CreateCircleBresenham(bool filled);

	void CreateLineBresenham(Uint32* pixels, int x1, int y1, int x2, int y2);

	void Tick();

	void Render();
};

#endif
