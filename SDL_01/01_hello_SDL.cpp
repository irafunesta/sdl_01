
#include "Dot.h"
#include "LTexture.h"
#include "LTimer.h"

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Key press surfaces constants
enum KeyPressSurfaces
{
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

//Loads individual image
SDL_Surface* loadSurface(std::string path);

//The images that correspond to a keypress
SDL_Texture* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];

//Current displayed image
SDL_Texture* gCurrentTexture = NULL;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

//---------------  Texture rendering --------------------
//Loads individual image as texture
SDL_Texture* loadTexture(std::string path);

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Current displayed texture
SDL_Texture* gTexture = NULL;

//Globally used font
TTF_Font* gFont = NULL;

//Scene textures
LTexture gFPSTextTexture;

//Main loop flag
bool quit = false;

//Event handler
SDL_Event e;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Load fonts
bool loadMediaFonts();

//Frees media and shuts down SDL
void close();


int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}

		if (!loadMediaFonts())
		{
			printf("Failed to load media!\n");
		}

		Dot player;

		//Set default current surface
		gCurrentTexture = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];

		//Set text color as black
		SDL_Color textColor = { 0, 255, 0, 255 };

		//The frames per second timer
		LTimer fpsTimer;

		//In memory text stream
		std::stringstream timeText;

		//Start counting frames per second
		int countedFrames = 0;
		fpsTimer.start();

		//While application is running
		while (!quit)
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}

				player.handleEvent(e);
			}

			//Calculate and correct fps
			float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
			if (avgFPS > 2000000)
			{
				avgFPS = 0;
			}

			//Set text to be rendered
			timeText.str("");
			timeText << "Average Frames Per Second " << avgFPS;

			//Render text
			if (!gFPSTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor, gRenderer, gFont))
			{
				printf("Unable to render FPS texture!\n");
			}

			player.move(SCREEN_WIDTH, SCREEN_HEIGHT);
			
			//Set the color for both the clear and the drawing
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
			//Clear screen
			SDL_RenderClear(gRenderer);

			gFPSTextTexture.render((SCREEN_WIDTH - gFPSTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gFPSTextTexture.getHeight()) / 2, gRenderer);
			
			player.render(gRenderer);
			
			//Update screen
			SDL_RenderPresent(gRenderer);
			++countedFrames;
		}

		close();
	}

	return 0;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
				
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
			//Get window surface
			//gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag

	//OLD CODE

	//bool success = true;

	////Load splash image
	//gHelloWorld = SDL_LoadBMP("images/hello_world.bmp");
	//if (gHelloWorld == NULL)
	//{
	//	printf("Unable to load image %s! SDL Error: %s\n", "02_getting_an_image_on_the_screen/hello_world.bmp", SDL_GetError());
	//	success = false;
	//}

	//return success;

	//---------------- NEW CODE ----------------

	//Loading success flag
	bool success = true;

	//Load default surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadTexture("images/press.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL)
	{
		printf("Failed to load default image!\n");
		success = false;
	}

	//Load up surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadTexture("images/up.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL)
	{
		printf("Failed to load up image!\n");
		success = false;
	}

	//Load down surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadTexture("images/down.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL)
	{
		printf("Failed to load down image!\n");
		success = false;
	}

	//Load left surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadTexture("images/left.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL)
	{
		printf("Failed to load left image!\n");
		success = false;
	}

	//Load right surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadTexture("images/right.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL)
	{
		printf("Failed to load right image!\n");
		success = false;
	}

	return success;
}

bool loadMediaFonts()
{
	//Loading success flag
	bool success = true;

	//Open the font
	gFont = TTF_OpenFont("fonts/lazy.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	gFPSTextTexture.free();

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Free loaded image
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;

	SDL_DestroyTexture(gCurrentTexture);
	gCurrentTexture = NULL;

	//Destroy window    
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
	TTF_Quit();
}

//SDL_Surface* loadSurface(std::string path)
//{
//	//The final optimized image
//	SDL_Surface* optimizedSurface = NULL;
//
//	//Load image at specified path
//	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
//	if (loadedSurface == NULL)
//	{
//		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), IMG_GetError());
//	}
//	else
//	{
//		//Convert surface to screen format
//		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
//		if (optimizedSurface == NULL)
//		{
//			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
//		}
//
//		//Get rid of old loaded surface
//		SDL_FreeSurface(loadedSurface);
//	}
//
//	return optimizedSurface;
//}

SDL_Texture* loadTexture(std::string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}