#include "pickUp.h"

Pickup::Pickup(SDL_Renderer* renderer)
{
	active = false;

	myRenderer = renderer;

	speed = rand() % (5) + 1;
	speed *= 100;

	int temp = rand() % (2) + 1;

	SDL_Surface* surface;

	if (temp == 1)
	{
		pickupType = "bomb";

		surface = IMG_Load("./assets/bombLayer.PNG");

	}
	else
	{
		pickupType = "lives";

		surface = IMG_Load("./assets/livesGraphic.PNG");

	}

	texture = SDL_CreateTextureFromSurface(myRenderer, surface);

	SDL_FreeSurface(surface);

	int w, h;

	SDL_QueryTexture(texture, NULL, NULL, &w, &h);

	posRect.w = w;
	posRect.h = h;

	posRect.x = rand() % (1024 - posRect.w) + 1;
	posRect.y = 0 - posRect.h;

	pos_X = posRect.x;
	pos_Y = posRect.y;

	xDir = 0;
	yDir = 1;

}

void Pickup::DeActivate()
{
	active = false;

	posRect.x = -2000;
	posRect.y = -2000;

	pos_X = posRect.x;
	pos_Y = posRect.y;
}

void Pickup::Reset()
{
	if (!active)
	{

		active = true;

		posRect.x = rand() % (1024 - posRect.w) + 1;
		posRect.y = 0 - posRect.h;

		pos_X = posRect.x;
		pos_Y = posRect.y;

		speed = rand() % (5) + 1;
		speed *= 100;

		int temp = rand() % (2) + 1;

		SDL_Surface* surface;

		if (temp == 1)
		{
			pickupType = "bomb";

			surface = IMG_Load("./assets/bombLayer.PNG");

		}
		else
		{
			pickupType = "lives";

			surface = IMG_Load("./assets/livesGraphic.PNG");

		}

		texture = SDL_CreateTextureFromSurface(myRenderer, surface);

		SDL_FreeSurface(surface);


	}

}

void Pickup::Update(float deltaTime)
{

	if (active)
	{
		pos_Y += (speed * yDir) * deltaTime;

		posRect.y = (int)(pos_Y + 0.5f);

		if (posRect.y > 768)
		{
			DeActivate();
		}

	}
}


void Pickup::Draw(SDL_Renderer* renderer)
{
	SDL_RenderCopy(renderer, texture, NULL, &posRect);

}

Pickup::~Pickup()
{
	//SDL_DestroyTexture(texture);

}