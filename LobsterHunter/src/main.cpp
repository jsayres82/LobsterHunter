/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, vectors, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

//The dimensions of the level
const int LEVEL_WIDTH = 1024;
const int LEVEL_HEIGHT = 768;

//Screen dimension constants
const int SCREEN_WIDTH = LEVEL_WIDTH;
const int SCREEN_HEIGHT = LEVEL_HEIGHT;

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#ifdef _SDL_TTF_H
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The Boat that will move around on the screen
class Boat
{
    public:
		//The dimensions of the Boat
		static const int BOAT_WIDTH = 240;
		static const int BOAT_HEIGHT = 220;

		//Maximum axis velocity of the dot
		static const int BOAT_VEL = 10;

		//Initializes the variables
		Boat();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot
		void move();

		//Shows the dot on the screen relative to the camera
		void render( int camX, int camY );

		//Position accessors
		int getPosX();
		int getPosY();

    private:
		//The X and Y offsets of the dot
		int mPosX, mPosY;

		//The velocity of the dot
		int mVelX, mVelY;
};

//The Lobster that will move around on the screen
class Lobster
{
    public:
		//The dimensions of the Boat
		static const int LOBSTER_WIDTH = 114;
		static const int LOBSTER_HEIGHT = 87;

		//Maximum axis velocity of the dot
		static const int LOBSTER_VEL = 5;

		//Initializes the variables
		Lobster(int x, int y);

		bool caught;
		//Takes key presses and adjusts the dot's velocity
		//void handleEvent( SDL_Event& e );

		//Moves the dot
		void move();

		//Shows the Lobster on the screen relative to the camera
		void render( int camX, int camY );

		//Position accessors
		int getPosX();
		int getPosY();

    private:
		//The X and Y offsets of the Lobster
		int mPosX, mPosY;

		//The velocity of the Lobster
		int mVelX, mVelY;
};

//The Trap that will move around on the screen
class Trap
{
    public:
		//The dimensions of the Boat
		static const int TRAP_WIDTH = 224;
		static const int TRAP_HEIGHT = 103;

		//Maximum axis velocity of the dot
		static const int TRAP_VEL = 5;

		//
		bool released;

		//Initializes the variables
		Trap(int x, int y);

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot
		void move();

		//Shows the Lobster on the screen relative to the camera
		void render( int camX, int camY );

		//Position accessors
		int getPosX();
		int getPosY();

		void setPosX( int x );
		void setPosY( int y );

    private:
		//The X and Y offsets of the Lobster
		int mPosX, mPosY;

		//The velocity of the Lobster
		int mVelX, mVelY;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gBoatTexture;
LTexture gLobsterTexture;
LTexture gTrapTexture;
LTexture gBGTexture;
LTexture gWinTexture;
LTexture gBGCloudsTexture;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "Lobsterman Jordan", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load Boat texture
	if( !gBoatTexture.loadFromFile( "resources/img/boat.bmp" ) )
	{
		printf( "Failed to load Boat texture!\n" );
		success = false;
	}

	//Load Lobster texture
	if( !gLobsterTexture.loadFromFile( "resources/img/lobster.bmp" ) )
	{
		printf( "Failed to load Lobster texture!\n" );
		success = false;
	}

	//Load Trap texture
	if( !gTrapTexture.loadFromFile( "resources/img/trap.bmp" ) )
	{
		printf( "Failed to load Trap texture!\n" );
		success = false;
	}

	//Load You Win texture
	if( !gWinTexture.loadFromFile( "resources/img/win.bmp" ) )
	{
		printf( "Failed to load Win texture!\n" );
		success = false;
	}
	//Load background texture
	if( !gBGCloudsTexture.loadFromFile( "resources/img/clowds_type_1.png" ) )
	{
		printf( "Failed to load background texture!\n" );
		success = false;
	}
	//Load background texture
	if( !gBGTexture.loadFromFile( "resources/img/bg.png" ) )
	{
		printf( "Failed to load background texture!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	gBoatTexture.free();

	gBGTexture.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}



Boat::Boat()
{
    //Initialize the offsets
    mPosX = 0;
    mPosY = 0;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

Lobster::Lobster(int x, int y)
{
    //Initialize the offsets
    mPosX = x;
    mPosY = y;

    //Initialize the velocity
    mVelX = LOBSTER_VEL;
    mVelY = 0;

	caught = false;
}

Trap::Trap(int x, int y)
{
    //Initialize the offsets
    mPosX = x;
    mPosY = y;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

	released = false;
}

void Trap::setPosX(int x)
{
    //Initialize the offsets
    mPosX = x;
}

void Trap::setPosY(int y)
{
    //Initialize the offsets
    mPosY = y;
	mVelY = 0;
}

void Boat::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
           // case SDLK_UP: mVelY -= BOAT_VEL; break;
           // case SDLK_DOWN: mVelY += BOAT_VEL; break;
            case SDLK_LEFT: mVelX -= BOAT_VEL; break;
            case SDLK_RIGHT: mVelX += BOAT_VEL; break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            //case SDLK_UP: mVelY += BOAT_VEL; break;
            //case SDLK_DOWN: mVelY -= BOAT_VEL; break;
            case SDLK_LEFT: mVelX += BOAT_VEL; break;
            case SDLK_RIGHT: mVelX -= BOAT_VEL; break;
        }
    }
}

void Trap::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
           case SDLK_DOWN: 
			   released = true;
			   mVelY -= TRAP_VEL; 
			   break;
        }
    }
}

void Lobster::move()
{
    //Move the Boat left or right
    mPosX += mVelX;

    //If the Boat went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + LOBSTER_WIDTH > LEVEL_WIDTH ) )
    {
        //Move back
        mPosX -= mVelX;
		mVelX = mVelX*-1;
    }	   
}

void Trap::move()
{
    //Move the Boat left or right
    mPosY -= mVelY;

    //If the Boat went too far to the left or right
    if( ( mPosY < 0 ) || ( mPosY + TRAP_HEIGHT > LEVEL_HEIGHT ) )
    {
        //Move back
        mPosY += mVelY;
    }	   
}

void Boat::move()
{
    //Move the Boat left or right
    mPosX += mVelX;

    //If the Boat went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + BOAT_WIDTH > LEVEL_WIDTH ) )
    {
        //Move back
        mPosX -= mVelX;
    }

    //Move the Boat up or down
    mPosY += mVelY;

    //If the Boat went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + BOAT_HEIGHT > LEVEL_HEIGHT ) )
    {
        //Move back
        mPosY -= mVelY;
    }
}

void Boat::render( int camX, int camY )
{
    //Show the Boat relative to the camera
	gBoatTexture.render( mPosX - camX, mPosY - camY );
}

void Lobster::render( int camX, int camY )
{
    //Show the Boat relative to the camera
	if( caught == false )
		gLobsterTexture.render( mPosX - camX, mPosY - camY );
	else
	{
		gWinTexture.render(SCREEN_WIDTH/2 - 244/2, SCREEN_HEIGHT/2 - 244/2);
		gLobsterTexture.render( mPosX - camX, mPosY - camY );
	}
}

void Trap::render( int camX, int camY )
{
    //Show the Boat relative to the camera
	gTrapTexture.render( mPosX - camX, mPosY - camY );
}

int Boat::getPosX()
{
	return mPosX;
}

int Boat::getPosY()
{
	return mPosY;
}

int Lobster::getPosX()
{
	return mPosX;
}

int Lobster::getPosY()
{
	return mPosY;
}

int Trap::getPosX()
{
	return mPosX;
}

int Trap::getPosY()
{
	return mPosY;
}




int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The Boat that will be moving around on the screen
			Boat boat;
			Lobster lobster(SCREEN_WIDTH/2, SCREEN_HEIGHT-Lobster::LOBSTER_HEIGHT);
			Trap trap(boat.getPosX(), boat.getPosY() + Boat::BOAT_HEIGHT );
			
			//The camera area
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for the Boat
					boat.handleEvent( e );
					if( trap.getPosY() ==  boat.getPosY() + Boat::BOAT_HEIGHT )
						trap.handleEvent( e );
						
					
				}

				//Move the dot
				boat.move();
				
				if( lobster.caught == false )
					lobster.move();

				if( trap.released == false )
					trap.setPosX( boat.getPosX() + Boat::BOAT_WIDTH / 2 );
				else
					trap.move();
				
				if( trap.getPosY() + Trap::TRAP_HEIGHT >= LEVEL_HEIGHT - Trap::TRAP_VEL)
					// Lobster completely in trap
					if( lobster.getPosX() >= trap.getPosX() && lobster.getPosX() <= trap.getPosX() + Lobster::LOBSTER_WIDTH )
						lobster.caught = true;
					else
					{
						trap.setPosY(boat.getPosY() + Boat::BOAT_HEIGHT );
						trap.released = false;
					}
				//Center the camera over the Boat
				camera.x = ( boat.getPosX() + Boat::BOAT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
				camera.y = ( boat.getPosY() + Boat::BOAT_HEIGHT / 2 ) - SCREEN_HEIGHT / 2;

				//Keep the camera in bounds
				if( camera.x < 0 )
				{ 
					camera.x = 0;
				}
				if( camera.y < 0 )
				{
					camera.y = 0;
				}
				if( camera.x > LEVEL_WIDTH - camera.w )
				{
					camera.x = LEVEL_WIDTH - camera.w;
				}
				if( camera.y > LEVEL_HEIGHT - camera.h )
				{
					camera.y = LEVEL_HEIGHT - camera.h;
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render background
			    gBGTexture.render( 0, 0, &camera );
				// gBGCloudsTexture.render( 0, 0, &camera );

				//Render objects
				boat.render( camera.x, camera.y );
				lobster.render( camera.x, camera.y );
				if(trap.released == true)
					trap.render( camera.x, camera.y ); 

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}