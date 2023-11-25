#include "Game.hpp"
#include "Constants.hpp"
#include "Circle.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cstdint>
#include <memory>
#include <iostream>
#include <time.h>
#include <stdlib.h>

Game::Game() : 
	initialized_(false), 
	running_(false), 
	game_ticks_(0)
{
	initialized_ = Initialize();
	InitializeCircles();
}

Game::~Game()
{
	Finalize();
}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not be initialized! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
	{
		printf("%s\n", "Warning: Texture filtering is not enabled!");
	}

	window_ = SDL_CreateWindow(constants::game_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, constants::screen_width, constants::screen_height, SDL_WINDOW_SHOWN);

	if (window_ == nullptr)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);

	if (renderer_ == nullptr)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	constexpr int img_flags = IMG_INIT_PNG;

	if (!(IMG_Init(img_flags) & img_flags))
	{
		printf("SDL_image could not be initialized! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	return true;
}

void Game::Finalize()
{
	SDL_DestroyWindow(window_);
	window_ = nullptr;
	
	SDL_DestroyRenderer(renderer_);
	renderer_ = nullptr;

	SDL_Quit();
	IMG_Quit();
}

void Game::Run()
{
	if (!initialized_)
	{
		return;
	}

	running_ = true;

	constexpr double ms = 1.0 / 60.0;
	std::uint64_t last_time = SDL_GetPerformanceCounter();
	long double delta = 0.0;

	double timer = SDL_GetTicks();

	int frames = 0;
	int ticks = 0;

	while (running_)
	{
		const std::uint64_t now = SDL_GetPerformanceCounter();
		const long double elapsed = static_cast<long double>(now - last_time) / static_cast<long double>(SDL_GetPerformanceFrequency());

		last_time = now;
		delta += elapsed;

		HandleEvents();

		while (delta >= ms)
		{
			Tick();
			delta -= ms;
			++ticks;
		}

		//printf("%Lf\n", delta / ms);
		Render();
		++frames;

		if (SDL_GetTicks() - timer > 1000.0)
		{
			timer += 1000.0;
			//printf("Frames: %d, Ticks: %d\n", frames, ticks);
			frames = 0;
			ticks = 0;
		}
	}
}

void Game::HandleEvents()
{
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			running_ = false;
			return;
		}
	}
}

void Game::Tick()
{
	++game_ticks_;

	for (auto& circle : circles_)
	{
		circle->Tick();
	}
}

void Game::Render()
{
	SDL_RenderSetViewport(renderer_, NULL);
	SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(renderer_);

	for (auto& circle : circles_)
	{
		circle->Render();
	}

	SDL_RenderPresent(renderer_);
}

bool Game::InitializeCircles()
{
	// const SDL_Point center = { constants::screen_width / 2, constants::screen_height / 2 };
	// const SDL_Color color = { 0x00, 0xff, 0x00, 0xff };

	std::srand(std::time(nullptr));
	std::rand();

	const int radius = 50;

	for (int i = 0; i < 100; ++i)
	{
		const SDL_Point center = { std::rand() % constants::screen_width, std::rand() % constants::screen_height };
		Uint8 r = static_cast<Uint8>(std::rand()) % 0xff;
		Uint8 g = static_cast<Uint8>(std::rand()) % 0xff;
		Uint8 b = static_cast<Uint8>(std::rand()) % 0xff;
		const SDL_Color color = { r, g, b, 0xff };
		circles_.emplace_back(std::make_unique<Circle>(renderer_, center, radius, color));
	}

	return true;
}
