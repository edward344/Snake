#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <sstream>

SDL_Surface* loadImage(std::string filename)
{
	//The image that's loaded
	SDL_Surface* loadedImage = NULL;
	//The optimized image that will be used
	SDL_Surface* optimizedImage = NULL;
	//Load the image using SDL_image
	loadedImage = IMG_Load(filename.c_str());
	// if the image loaded
	if (loadedImage != NULL)
	{
		//create an optmized image
		optimizedImage = SDL_DisplayFormatAlpha(loadedImage);
		//Free the old image
		SDL_FreeSurface(loadedImage);
	}
	//Return the optimized image
	return optimizedImage;
}

class Game
{
	public:
		SDL_Surface* background;
		SDL_Surface* square;
		SDL_Surface* itemImage;
		SDL_Surface* scoreText;
		SDL_Surface* gameOverImage;
		SDL_Surface* winnerImage;
		SDL_Surface* intro;
		TTF_Font* font;
		SDL_Color textColor;
		Mix_Chunk* blip;
		short int score;
		char changeX, changeY;
		char stack[100][2];
		char stackTop[2];
		char item[2];
		short int stackLength;
		bool running;
		bool gameOver;
		bool change;
		//methods:
		Game(); //Constructor.
		void update();
		void displayFrame(SDL_Surface*);
		void cleanUp();
		void addItem(int,int);
		void moveUp();
		void moveDown();
		void moveLeft();
		void moveRight();
		void createItem();
		void startGame();
};

Game::Game()
{
	background = loadImage("background.png");
	square = loadImage("square.png");
	itemImage = loadImage("item.png");
	blip = Mix_LoadWAV("Blip.wav");
	textColor = {0,0,0};
	font = TTF_OpenFont("joystix monospace.ttf",20);
	scoreText = TTF_RenderText_Solid(font,"Score: 0",textColor);
	gameOverImage = TTF_RenderText_Solid(font,"GAME OVER",textColor);
	winnerImage = TTF_RenderText_Solid(font,"You Won!",textColor);
	intro = TTF_RenderText_Solid(font,"Press Enter to start...",textColor);
	stackLength = 0;
	score = 0;
	gameOver = false;
	running = false;
	change = false;
	addItem(1,1);
	addItem(2,1);
	addItem(3,1);
	
	changeX = 1;
	changeY = 0;
	createItem();
}

void Game::addItem(int x,int y)
{
	int arrayLength = sizeof(stack)/sizeof(char);
	if (stackLength < arrayLength)
	{
		stack[stackLength][0] = x;
		stack[stackLength][1] = y;
		stackLength++;
		stackTop[0] = x;
		stackTop[1] = y;
	}

}

void Game::update()
{
	int i;
	change = false;
	if (running)
	{
		for (i=0;i<stackLength -1;i++)
		{
			stack[i][0] = stack[i+1][0];
			stack[i][1] = stack[i+1][1];
		}
		
		stackTop[0] += changeX;
		stackTop[1] += changeY;
		
		
		stack[stackLength -1][0] = stackTop[0];
		stack[stackLength -1][1] = stackTop[1];
		
		if (stackTop[0] < 0)
		{
			running = false;
			gameOver = true;
		}
		else if (stackTop[0] > 29)
		{
			running = false;
			gameOver = true;
		}
		else if (stackTop[1] < 0)
		{
			running = false;
			gameOver = true;
		}
		else if (stackTop[1] > 19)
		{
			running = false;
			gameOver = true;
		}
			
		for (i=0;i<stackLength -1;i++)
		{
			if((stackTop[0] == stack[i][0]) && (stackTop[1] == stack[i][1]))
			{
				running = false;
				gameOver = true;
			}
		}
		
		if ((stackTop[0] == item[0]) && (stackTop[1] == item[1]))
		{
			addItem(item[0],item[1]);
			createItem();
			Mix_PlayChannel(-1,blip,0);
			score++;
		}
		
		if (stackLength == 100)
			running = false;

		if (gameOver)
			running = false;

		std::string text;
		std::ostringstream convert;
		convert << score;
		text = "Score: " + convert.str();
		scoreText = TTF_RenderText_Solid(font,text.c_str(),textColor);
	}
}

void Game::displayFrame(SDL_Surface* screen)
{
	SDL_Rect rect;
	
	SDL_BlitSurface(background,NULL,screen,NULL);
	if (running)
	{
		// draw the Snake:
		for (int i=0; i < stackLength; i++)
		{
			rect.x = stack[i][0] * 20 +20;
			rect.y = stack[i][1] * 20 +20;
			SDL_BlitSurface(square,NULL,screen,&rect);
		}
		rect.x = item[0] * 20 +20;
		rect.y = item[1] * 20 +20;
		SDL_BlitSurface(itemImage,NULL,screen,&rect);
		// Set the coordinates for the score text
		rect.x = 20;
		rect.y = 435;
		// draw the scoreText
		SDL_BlitSurface(scoreText,NULL,screen,&rect);
	}
	else if(gameOver)
	{
		rect.x = 240;
		rect.y = 180;
		SDL_BlitSurface(gameOverImage,NULL,screen,&rect);
		rect.x = 140;
		rect.y = 435;
		SDL_BlitSurface(intro,NULL,screen,&rect);
	}
	else if (stackLength == 100)
	{
		rect.x = 260;
		rect.y = 435;
		SDL_BlitSurface(winnerImage,NULL,screen,&rect);
	}
	else
	{
		rect.x = 140;
		rect.y = 435;
		SDL_BlitSurface(intro,NULL,screen,&rect);
	}
}

void Game::moveUp()
{
	if (changeY != 1 && !change)
	{
		changeX = 0;
		changeY = -1;
		change = true;
	}
}

void Game::moveDown()
{
	if (changeY != -1 && !change)
	{
		changeX = 0;
		changeY = 1;
		change = true;
	}
}

void Game::moveLeft()
{
	if (changeX != 1 && !change)
	{
		changeX = -1;
		changeY = 0;
		change = true;
	}
}

void Game::moveRight()
{
	if (changeX != -1 && !change)
	{
		changeX = 1;
		changeY = 0;
		change = true;
	}
}

void Game::createItem()
{
	bool flag;
	int x,y;
	srand(time(NULL));
	do 
	{
		flag = false;
		x = rand() % 30;
		y = rand() % 20;
		
		for (int i=0; i<stackLength ; i++)
		{
			if (stack[i][0] == x && stack[i][1] == y)
				flag = true;
		}
	} while(flag);
	item[0] = x;
	item[1] = y;
}

void Game::startGame()
{
	running = true;
	if (gameOver)
	{
		stackLength = 0;
		score = 0;
		addItem(1,1);
		addItem(2,1);
		addItem(3,1);
		gameOver = false;
		changeX = 1;
		changeY = 0;
		createItem();
	}
}

void Game::cleanUp()
{
	SDL_FreeSurface(background);
	SDL_FreeSurface(square);
	SDL_FreeSurface(itemImage);
	SDL_FreeSurface(scoreText);
	SDL_FreeSurface(gameOverImage);
	SDL_FreeSurface(winnerImage);
	SDL_FreeSurface(intro);
	TTF_CloseFont(font);
	Mix_FreeChunk(blip);
}


int main( int argc, char* args[] )
{
	//Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );
    //Start SDL_ttf 
    TTF_Init();
    //Initialize SDL_mixer
    Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,4096);
    
    SDL_Surface* screen = NULL;
    
    //Set up screen
    screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );    
    //Set the window caption
    SDL_WM_SetCaption("Snake",NULL);
    //Loop until the user clicks the close button.
    bool done = false;
	//Create game Object:
    Game game;
    //set up the event handler
    SDL_Event event;
    //----------Main Program Loop---------------------------------------
    while (!done)
    {
        // While there's an event to handle
        while (SDL_PollEvent(&event))
        {
            //User did something
            if (event.type == SDL_KEYDOWN)
            {
				switch (event.key.keysym.sym)
				{
					case SDLK_UP: game.moveUp(); break;
					case SDLK_DOWN: game.moveDown(); break;
					case SDLK_LEFT: game.moveLeft(); break;
					case SDLK_RIGHT: game.moveRight(); break;
					case SDLK_RETURN: game.startGame();break;
					case SDLK_ESCAPE: done = true; break;
				}
			}
			
            else if (event.type == SDL_QUIT)
            {
                done = true; // Flag that we are done so we exit this loop
            }
        }
        // -------Game logic shoud go here------------------------------
		game.update();
        // ------------Drawing code should go here:---------------------
        game.displayFrame(screen);
        // ------------Go ahead and update the screen with what we've drawn.
        SDL_Flip(screen);
        // --------Delay 32 milliseconds--------------------------------
        SDL_Delay(150);
    }
    game.cleanUp();
    //quit SDL_ttf
	TTF_Quit();
	//quit SDL_mixer
	Mix_CloseAudio();
    //Quit SDL
    SDL_Quit();

    return 0;
}
