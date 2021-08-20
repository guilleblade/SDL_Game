#include "enemyClass.h"

Enemy::Enemy(SDL_Renderer* renderer)
{
	SDL_Surface* surface = IMG_Load("./Assets/enemy.png");

	texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);

	int w, h;

	SDL_QueryTexture(texture, NULL, NULL, &w, &h);

	posRect.w = w;
	posRect.h = h;

	Reset();

	xDir = 0;
	yDir = 1;

}

void Enemy::Reset()
{
	speed = rand() % (5) + 1;
	speed *= 100;

	posRect.x = rand() % (1024 - posRect.w) + 1;
	posRect.y = 0 - posRect.h;

	pos_X = posRect.x;
	pos_Y = posRect.y;


}

void Enemy::Update(float deltaTime)
{
	pos_Y += (speed * yDir) * deltaTime;

	posRect.y = (int)(pos_Y + 0.5f);

	if (posRect.y > 768)
	{
		Reset();
	}

}

void Enemy::Draw(SDL_Renderer* renderer)
{
	SDL_RenderCopy(renderer, texture, NULL, &posRect);

}


 Enemy::~Enemy()
{
	 //SDL_DestroyTexure(texture);

}