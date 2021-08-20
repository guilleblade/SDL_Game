#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

//boolean to maintain program loop
bool quit = false;

//deltaTime init() - for frame rate ind.
float deltaTime = 0.0;
int thisTime = 0;
int lastTime = 0;

//set player speed 
float playerSpeed = 500.0f;

//movement in the x axis - left or right
float xDir;

//variables use to control integer 
float pos_X, pos_Y;

//enemy stuff
#include "enemyClass.h";

#include <vector>

//enemy list variable
vector<Enemy> enemyList;

//Bullet info
#include "bullet.h"
vector<Bullet> bulletList;

//create rectangles for the player
SDL_Rect playerPos;

//declare sounds
Mix_Chunk* laser;
Mix_Chunk* explosion;
Mix_Music* bgm;

void CreateBullet()
{
	for (int i = 0; i < bulletList.size(); i++)
	{
		if (bulletList[i].active == false)
		{
			//play laser sound
			Mix_PlayChannel(-1, laser, 0);

			bulletList[i].active = true;

			bulletList[i].posRect.x = (pos_X + (playerPos.w / 2));

			bulletList[i].posRect.x = (bulletList[i].posRect.x - (bulletList[i].posRect.w / 2));

			bulletList[i].posRect.y = playerPos.y;

			bulletList[i].pos_X = pos_X;

			bulletList[i].pos_Y = playerPos.y;

			break;
		}

	}
}

//variables for score and lives
//score, lives and fonts
int playerScore, oldScore, playerLives, oldLives;

//delcare font
TTF_Font* font;

//font color
SDL_Color colorP1 = { 0 , 225, 0, 225 };

//create surfaces
SDL_Surface* scoreSurface, * livesSurface;

//create textures
SDL_Texture* scoreTexture, * livesTexture;

SDL_Rect scorePos, livesPos;

string tempScore, tempLives;

void UpdateScore(SDL_Renderer* renderer)
{
	//creating the text
	tempScore = "Player Score: " + std::to_string(playerScore);

	//create render text
	scoreSurface = TTF_RenderText_Solid(font, tempScore.c_str(), colorP1);

	//creating texture
	scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);

	SDL_QueryTexture(scoreTexture, NULL, NULL, &scorePos.w, &scorePos.h);

	SDL_FreeSurface(scoreSurface);

	oldScore = playerScore;

}

void UpdateLives(SDL_Renderer* renderer)
{
	tempLives = "Player Lives: " + std::to_string(playerLives);

	//create render text
	livesSurface = TTF_RenderText_Solid(font, tempLives.c_str(), colorP1);


	//create texture
	livesTexture = SDL_CreateTextureFromSurface(renderer, livesSurface);

	SDL_QueryTexture(livesTexture, NULL, NULL, &livesPos.w, &livesPos.h);

	SDL_FreeSurface(livesSurface);

	oldLives = playerLives;
}

//setup pickup
#include "pickUp.h"
vector<Pickup> pickupList;

int main(int argc, char* argv[]) {

	SDL_Window* window;				//declare a pointer

	//create a renderer variable
	SDL_Renderer* renderer = NULL;

	SDL_Init(SDL_INIT_EVERYTHING);			//Initialize SDL2

	//create an application window of the following settings
	window = SDL_CreateWindow(
		"Space Game",			//window title
		SDL_WINDOWPOS_UNDEFINED,	//initial x position
		SDL_WINDOWPOS_UNDEFINED,	//initial y position
		1024,						//width in pixels
		768,						//height in pixels
		SDL_WINDOW_OPENGL			//flags - see below
	);


	//check that the window was successfully created
	if (window == NULL)
	{
		//in the case that the window could not be made
		printf("Could not create window %s\n", SDL_GetError());
		return 1;
	}

	//create renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//background image - CREATE

	//create a SDL surface
	SDL_Surface* surface = IMG_Load("./Assets/bkgd.png");

	//create bkd texture
	SDL_Texture* bkgd;

	//place surface into the texture
	bkgd = SDL_CreateTextureFromSurface(renderer, surface);

	// free the surface
	SDL_FreeSurface(surface);

	//create the rectangles for the new graphics
	SDL_Rect bkgdPos;

	//set bkgdPos x, y, width and height
	bkgdPos.x = 0;
	bkgdPos.y = 0;
	bkgdPos.w = 1024;
	bkgdPos.h = 768;

	//background image - CREATE END

		//player image -- CREATE

	//create SDL Surface
	surface = IMG_Load("./Assets/player.png");

	//create bkgd texture 
	SDL_Texture* player;

	//place surface into the texture
	player = SDL_CreateTextureFromSurface(renderer, surface);

	//free the surface
	SDL_FreeSurface(surface);



	//set bkgdPos x, y, width and height
	playerPos.x = 500;
	playerPos.y = 700;
	playerPos.w = 220;
	playerPos.h = 44;



	//player image -- CREATE END

		//SDL event to handle input
	SDL_Event event;

	//create bullets and place in list
	for (int i = 0; i < 10; i++)
	{
		Bullet tempBullet(renderer, i + 5, i + 5);

		bulletList.push_back(tempBullet);
	}

	//clear enemy list
	enemyList.clear();

	//fill enemy list
	for (int i = 0; i < 6; i++)
	{
		Enemy tempEnemy(renderer);

		enemyList.push_back(tempEnemy);
	}

	//initialize audio playback
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	//load laser sound
	laser = Mix_LoadWAV("./assets/retrolaser.wav");

	//load explosion sound
	explosion = Mix_LoadWAV("./assets/retroExplosion.wav");

	//load and loop background music
	bgm = Mix_LoadMUS("./assets/retroMusic.wav");

	//if the music is not playing, play it and loop
	if (!Mix_PlayingMusic())
	{
		Mix_PlayMusic(bgm, -1);
	}

	//initialize player score lives and font
	oldScore = 0;
	playerScore = 0;
	oldLives = 0;
	playerLives = 3;

	TTF_Init();
	font = TTF_OpenFont("./assets/Retronoid.ttf", 20);

	//set up rectangles
	scorePos.x = scorePos.y = 10;
	livesPos.x = 10;
	livesPos.y = 40;


	//set up our enum to handle all game states
	enum GameState { MENU, INSTRUCTIONS, GAME, WIN, LOSE, LEVELTWO };

	//variable to track where we are in the game
	GameState gameState = MENU;

	//bool value to allow movement through the individual states
	bool menu, instructions, game, win, lose, levelTwo;

	//clear pickup list and create pickup list
	pickupList.clear();

	Pickup tempPickup(renderer);

	pickupList.push_back(tempPickup);


	//basic program loop
	while (!quit)
	{
		switch (gameState)
		{
		case MENU:
			menu = true;

			cout << "The Game State is MENU" << endl;
			cout << "Press the I key for Instructions" << endl;
			cout << "Press the S key to Start the game" << endl;
			cout << "Press the Q key to Quit Game" << endl;
			cout << endl;

			//load instructions background
			//create a SDL surface
			surface = IMG_Load("./assets/MainMenu.png");

			//place surface into the texture
			bkgd = SDL_CreateTextureFromSurface(renderer, surface);

			//free the surface
			SDL_FreeSurface(surface);

			//background image -- CREATE END



			while (menu)
			{
				//create deltaTime
				thisTime = SDL_GetTicks();
				deltaTime = (float)(thisTime - lastTime) / 1000;
				lastTime = thisTime;

				//check for input
				if (SDL_PollEvent(&event))
				{

					//close window by the windows x button
					if (event.type == SDL_QUIT)
					{
						quit = true;
						menu = false;
						break;
					}

					switch (event.type)
					{
						//look for a key press
					case SDL_KEYUP:

						//check the SDLKEy values and move the coords
						switch (event.key.keysym.sym)
						{
						case SDLK_i:
							menu = false;
							gameState = INSTRUCTIONS;
							break;
						case SDLK_s:
							menu = false;
							gameState = GAME;
							break;
						case SDLK_q:
							quit = true;
							menu = false;
							break;
						default:
							break;
						}


					}


				}

				//DRAW SECTION
				//clear old buffer
				SDL_RenderClear(renderer);

				//prepare to draw bkgd1
				SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);

				//draw info into the screen
				SDL_RenderPresent(renderer);

				//END DRAW*********

			}//ends menu screen

			break;

		case INSTRUCTIONS:
			instructions = true;

			cout << "The Game State is MENU" << endl;
			cout << "Press the I key for Instructions" << endl;
			cout << "Press the S key to Start the game" << endl;
			cout << "Press the Q key to Quit Game" << endl;
			cout << endl;

			//load instruction background
			//create SDL surface
			surface = IMG_Load("./assets/instructionsTwo.png");

			bkgd = SDL_CreateTextureFromSurface(renderer, surface);

			//free the surface
			SDL_FreeSurface(surface);

			//background image -- CREATE END

			while (instructions)
			{
				//create deltaTime
				thisTime = SDL_GetTicks();
				deltaTime = (float)(thisTime - lastTime) / 1000;
				lastTime = thisTime;

				//check for input
				if (SDL_PollEvent(&event))
				{
					//close window by the windows x button
					if (event.type == SDL_QUIT)
					{
						quit = true;
						instructions = false;
						break;
					}


					switch (event.type)
					{
						//look for a key press
					case SDL_KEYUP:

						//check the SDLKEy values and move the coords
						switch (event.key.keysym.sym)
						{
						case SDLK_m:
							instructions = false;
							gameState = MENU;
							break;
						case SDLK_s:
							instructions = false;
							gameState = GAME;
							break;
						case SDLK_q:
							quit = true;
							instructions = false;
							break;
						default:
							break;
						}


					}





				}
				//DRAW******
						//clear old buffer
				SDL_RenderClear(renderer);

				//prepare to draw bkgd1
				SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);

				//draw info into the screen
				SDL_RenderPresent(renderer);

				//END DRAW****


			} //ends instructions screen



			break;


		case GAME:
		{
			game = true;

			cout << "The game state is GAME" << endl;

			surface = IMG_Load("./assets/bkgd.png");

			bkgd = SDL_CreateTextureFromSurface(renderer, surface);

			//free the surface
			SDL_FreeSurface(surface);

			//start the game fresh
			enemyList.clear();

			//fill enemy list
			for (int i = 0; i < 6; i++)
			{
				Enemy tempEnemy(renderer);

				enemyList.push_back(tempEnemy);

			}
			//clear pick up list
			pickupList.clear();

			Pickup tempPickup(renderer);

			pickupList.push_back(tempPickup);

			//reset players score and lives
			playerScore = 0;
			playerLives = 3;




			while (game)
			{
				//create deltaTime
				thisTime = SDL_GetTicks();
				deltaTime = (float)(thisTime - lastTime) / 1000;
				lastTime = thisTime;

				//check for input
				if (SDL_PollEvent(&event))
				{
					//close window by the windows x button
					if (event.type == SDL_QUIT)
					{
						quit = true;
						game = false;
						break;
					}

					switch (event.type)
					{
						// look for a keypress
					case SDL_KEYUP:

						//check the space bar
						switch (event.key.keysym.sym)
						{
						case SDLK_SPACE:
							CreateBullet();
							pickupList[0].Reset();
							break;
						default:
							break;
						}

					}

				}

				//player movement
			//get the keyboard state
				const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

				//check with arrows are pressed
				if (currentKeyStates[SDL_SCANCODE_LEFT])
				{
					xDir = -1;
				}
				else if (currentKeyStates[SDL_SCANCODE_RIGHT])
				{
					xDir = 1;
				}
				else
				{
					xDir = 0;
				}


				//UPDATE
			  //get new position to move player using direction and deltaTime
				pos_X += (playerSpeed * xDir) * deltaTime;

				//move player
				playerPos.x = (int)(pos_X + 0.5f);

				//keep player on screen
				//moving left
				if (playerPos.x < 0)
				{
					playerPos.x = 0;
					pos_X = 0;

				}

				//moving right
				if (playerPos.x > 1024 - playerPos.w)
				{
					playerPos.x = 1024 - playerPos.w;
					pos_X = 1024 - playerPos.w;

				}
				cout << pos_X << endl;

				//Update Bullets
				for (int i = 0; i < bulletList.size(); i++)
				{
					if (bulletList[i].active == true)
					{
						bulletList[i].Update(deltaTime);
					}
				}

				//Update pickup
				pickupList[0].Update(deltaTime);

				//update enemy list
				for (int i = 0; i < 6; i++)
				{
					enemyList[i].Update(deltaTime);
				}

				//check for collision of bullet with enemies
				//for loop to scroll through all the players bullets
				for (int i = 0; i < bulletList.size(); i++)
				{
					//check to see if this bullet is active in the world
					if (bulletList[i].active == true)
					{
						//check all enemies against active bullet
						for (int j = 0; j < enemyList.size(); j++)
						{
							//see if there is a collision between this bullet and this enemy using SDL
							if (SDL_HasIntersection(&bulletList[i].posRect, &enemyList[j].posRect))
							{
								//play explosion sound
								Mix_PlayChannel(-1, explosion, 0);

								//reset the enemy
								enemyList[j].Reset();

								//reset the bullet
								bulletList[i].Reset();

								//give player points 
								playerScore += 10;

								//check to see if the player is able to go to level two
								if (playerScore >= 500)
								{
									game = false;
									gameState = LEVELTWO;
								}
							}
						}
					}

				}

				//check for collision with the player and all enemies
				//check all enemies against active bullet
				for (int i = 0; i < enemyList.size(); i++)
				{
					//see if there is a collision between this bullet and this enemy using SDL
					if (SDL_HasIntersection(&playerPos, &enemyList[i].posRect))
					{
						//playing sound
						Mix_PlayChannel(-1, explosion, 0);

						//reset the enemy
						enemyList[i].Reset();

						//deduct lives
						playerLives -= 1;

						//check to see if the player loses
						if (playerLives <= 0)
						{
							game = false;
							gameState = LOSE;
						}
					}

				}

				//check for collision with the player and pickups
				for (int i = 0; i < pickupList.size(); i++)
				{
					//see if there is a collision between the player and pickup using sdl
					if (SDL_HasIntersection(&playerPos, &pickupList[i].posRect))
					{
						//if so deactivate the pickup and remove a players life
						if (pickupList[i].pickupType == "lives")
						{
							//deactivate the enemy
							pickupList[i].DeActivate();

							//play explosion sound
							Mix_PlayChannel(-1, explosion, 0);

							//take player lives
							playerLives -= 1;

							//check to see if the player loses
							if (playerLives <= 0)
							{
								game = false;
								gameState = LOSE;
							}

						}
						else if (pickupList[i].pickupType == "bomb")
						{
							//check to see if this is the destroy all enemies
							//if so cycle through enemies and reset then give player points for each

							//deactivate the pickup
							pickupList[i].DeActivate();

							//play explosion sound
							Mix_PlayChannel(-1, laser, 0);

							//cycle through all enemies
							for (int i = 0; i < enemyList.size(); i++)
							{
								//reset the enemy
								enemyList[i].Reset();

								//give the player points
								playerScore += 10;
							}

							//check to see if the player can go to level 2
							if (playerScore >= 500)
							{
								game = false;
								gameState = LEVELTWO;
							}

						}
					}
				}

				//update score and lives on screen text
				if (playerScore != oldScore);
				{
					UpdateScore(renderer);
				}

				if (playerLives != oldLives);
				{
					UpdateLives(renderer);
				}

				//DRAW 
				 //clear old buffer
				SDL_RenderClear(renderer);


				//prepare to draw bkgd
				SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);

				//prepare to draw player
				SDL_RenderCopy(renderer, player, NULL, &playerPos);

				//Draw Bullets
				for (int i = 0; i < bulletList.size(); i++)
				{
					if (bulletList[i].active == true)
					{
						bulletList[i].Draw(renderer);
					}
				}
				//draw enemy list
				for (int i = 0; i < 6; i++)
				{
					enemyList[i].Draw(renderer);
				}

				//draw pickup
				pickupList[0].Draw(renderer);

				//draw score texture and lives texture
				SDL_RenderCopy(renderer, scoreTexture, NULL, &scorePos);

				SDL_RenderCopy(renderer, livesTexture, NULL, &livesPos);


				//draw new info to the screen
				SDL_RenderPresent(renderer);

				//END DRAW




			}//ends game screen

			break;
		}

					case LEVELTWO:
						levelTwo = true;

						cout << "The game state is Level 2" << endl;		

						//start the game fresh
						enemyList.clear();

						//fill enemy list
						for (int i = 0; i < 6; i++)
						{
							Enemy tempEnemy(renderer);

							enemyList.push_back(tempEnemy);

						}



						while (levelTwo)
						{
							//create deltaTime
							thisTime = SDL_GetTicks();
							deltaTime = (float)(thisTime - lastTime) / 1000;
							lastTime = thisTime;

							//check for input
							if (SDL_PollEvent(&event))
							{
								//close window by the windows x button
								if (event.type == SDL_QUIT)
								{
									quit = true;
									levelTwo = false;
									break;
								}

								switch (event.type)
								{
									// look for a keypress
								case SDL_KEYUP:

									//check the space bar
									switch (event.key.keysym.sym)
									{
									case SDLK_SPACE:
										CreateBullet();
										break;
									default:
										break;
									}

								}

							}

							//player movement
						//get the keyboard state
							const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

							//check with arrows are pressed
							if (currentKeyStates[SDL_SCANCODE_LEFT])
							{
								xDir = -1;
							}
							else if (currentKeyStates[SDL_SCANCODE_RIGHT])
							{
								xDir = 1;
							}
							else
							{
								xDir = 0;
							}


							//UPDATE
						  //get new position to move player using direction and deltaTime
							pos_X += (playerSpeed * xDir) * deltaTime;

							//move player
							playerPos.x = (int)(pos_X + 0.5f);

							//keep player on screen
							//moving left
							if (playerPos.x < 0)
							{
								playerPos.x = 0;
								pos_X = 0;

							}

							//moving right
							if (playerPos.x > 1024 - playerPos.w)
							{
								playerPos.x = 1024 - playerPos.w;
								pos_X = 1024 - playerPos.w;

							}

							//Update Bullets
							for (int i = 0; i < bulletList.size(); i++)
							{
								if (bulletList[i].active == true)
								{
									bulletList[i].Update(deltaTime);
								}
							}

							//update enemy list
							for (int i = 0; i < 6; i++)
							{
								enemyList[i].Update(deltaTime);
							}

							//check for collision of bullet with enemies
							//for loop to scroll through all the players bullets
							for (int i = 0; i < bulletList.size(); i++)
							{
								//check to see if this bullet is active in the world
								if (bulletList[i].active == true)
								{
									//check all enemies against active bullet
									for (int j = 0; j < enemyList.size(); j++)
									{
										//see if there is a collision between this bullet and this enemy using SDL
										if (SDL_HasIntersection(&bulletList[i].posRect, &enemyList[j].posRect))
										{
											//play explosion sound
											Mix_PlayChannel(-1, explosion, 0);

											//reset the enemy
											enemyList[j].Reset();

											//reset the bullet
											bulletList[i].Reset();

											//give player points 
											playerScore += 50;

											//check to see if the player wins
											if (playerScore >= 1000)
											{
												levelTwo = false;
												gameState = WIN;
											}
										}
									}
								}

							}

							//check for collision with the player and all enemies
							//check all enemies against active bullet
							for (int i = 0; i < enemyList.size(); i++)
							{
								//see if there is a collision between this bullet and this enemy using SDL
								if (SDL_HasIntersection(&playerPos, &enemyList[i].posRect))
								{
									//playing sound
									Mix_PlayChannel(-1, explosion, 0);

									//reset the enemy
									enemyList[i].Reset();

									//deduct lives
									playerLives -= 1;

									//check to see if the player loses
									if (playerLives <= 0)
									{
										levelTwo = false;
										gameState = LOSE;
									}
								}

							}
							//update score and lives on screen text
							if (playerScore != oldScore);
							{
								UpdateScore(renderer);
							}

							if (playerLives != oldLives);
							{
								UpdateLives(renderer);
							}

							//DRAW 
							 //clear old buffer
							SDL_RenderClear(renderer);


							//prepare to draw bkgd
							SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);

							//prepare to draw player
							SDL_RenderCopy(renderer, player, NULL, &playerPos);

							//Draw Bullets
							for (int i = 0; i < bulletList.size(); i++)
							{
								if (bulletList[i].active == true)
								{
									bulletList[i].Draw(renderer);
								}
							}
							//draw enemy list
							for (int i = 0; i < 6; i++)
							{
								enemyList[i].Draw(renderer);
							}

							//draw score texture and lives texture
							SDL_RenderCopy(renderer, scoreTexture, NULL, &scorePos);

							SDL_RenderCopy(renderer, livesTexture, NULL, &livesPos);


							//draw new info to the screen
							SDL_RenderPresent(renderer);

							//END DRAW




						}//ends game screen

						break;

			case WIN:
				win = true;

				cout << "The Game State is WIN" << endl;
				cout << "Press the M key for Main Menu" << endl;
				cout << "Press the S key to Start the game" << endl;
				cout << "Press the Q key to Quit Game" << endl;
				cout << endl;

				surface = IMG_Load("./assets/Win.png");

				bkgd = SDL_CreateTextureFromSurface(renderer, surface);

				//free the surface
				SDL_FreeSurface(surface);

				//background image -- CREATE END

				while (win)
				{
					//create deltaTime
					thisTime = SDL_GetTicks();
					deltaTime = (float)(thisTime - lastTime) / 1000;
					lastTime = thisTime;

					//check for input
					if (SDL_PollEvent(&event))
					{
						//close window by the windows x button
						if (event.type == SDL_QUIT)
						{
							quit = true;
							win = false;
							break;
						}


						switch (event.type)
						{
							//look for a key press
						case SDL_KEYUP:

							//check the SDLKEy values and move the coords
							switch (event.key.keysym.sym)
							{
							case SDLK_m:
								win = false;
								gameState = MENU;
								break;
							case SDLK_s:
								win = false;
								gameState = GAME;
								break;
							case SDLK_q:
								quit = true;
								win = false;
								break;
							default:
								break;
							}


						}


					}

					//DRAW******
				//clear old buffer
					SDL_RenderClear(renderer);

					//prepare to draw bkgd1
					SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);

					//draw info into the screen
					SDL_RenderPresent(renderer);

					//END DRAW****



				}//ends win screen


				break;

			case LOSE: 
				lose = true;

				cout << "The Game State is WIN" << endl;
				cout << "Press the M key for Main Menu" << endl;
				cout << "Press the S key to Start the game" << endl;
				cout << "Press the Q key to Quit Game" << endl;
				cout << endl;

				surface = IMG_Load("./assets/Lose.png");

				bkgd = SDL_CreateTextureFromSurface(renderer, surface);

				//free the surface
				SDL_FreeSurface(surface);

				//background image -- CREATE END

				while (lose)
				{
					//create deltaTime
					thisTime = SDL_GetTicks();
					deltaTime = (float)(thisTime - lastTime) / 1000;
					lastTime = thisTime;

					//check for input
					if (SDL_PollEvent(&event))
					{
						//close window by the windows x button
						if (event.type == SDL_QUIT)
						{
							quit = true;
							lose = false;
							break;
						}


						switch (event.type)
						{
							//look for a key press
						case SDL_KEYUP:

							//check the SDLKEy values and move the coords
							switch (event.key.keysym.sym)
							{
							case SDLK_m:
								lose = false;
								gameState = MENU;
								break;
							case SDLK_s:
								lose = false;
								gameState = GAME;
								break;
							case SDLK_q:
								quit = true;
								lose = false;
								break;
							default:
								break;
							}


						}


					}

					//DRAW******
				//clear old buffer
					SDL_RenderClear(renderer);

					//prepare to draw bkgd1
					SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);

					//draw info into the screen
					SDL_RenderPresent(renderer);

					//END DRAW****

				}//ends lose screen

		}//ends switch for game loop


	}//ends game loop


	//close and destroy the window
	SDL_DestroyWindow(window);

	//clean up
	SDL_Quit();

	return 0;

}