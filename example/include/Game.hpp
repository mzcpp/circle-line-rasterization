#ifndef GAME_HPP
#define GAME_HPP

#include "CircleTexture.hpp"

#include <SDL2/SDL.h>

#include <memory>

class Game
{
private:
	bool initialized_;
	bool running_;

	SDL_Point mouse_pos_;
	std::unique_ptr<CircleTexture> circle_texture_;

	SDL_Window* window_;
	SDL_Renderer* renderer_;

public:
	Game();

	~Game();

	bool Initialize();

	void Finalize();

	void Run();

	void HandleEvents();
	
	void Tick();
	
	void Render();
};

#endif