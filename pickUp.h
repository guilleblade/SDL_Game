#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
using namespace std;

class Pickup
{

public:

	bool active;

	string pickupType;

	SDL_Renderer* myRenderer;

	SDL_Texture* texture;

	SDL_Rect posRect;
	
	float xDir, yDir;

	float speed;

	float pos_X, pos_Y;

	Pickup(SDL_Renderer* renderer);

	void Update(float deltaTime);

	void Draw(SDL_Renderer* renderer);

	void DeActivate();

	void Reset();

	~Pickup();








};
