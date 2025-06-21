#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>  // use the proper casing on Windows
#include <iostream>
#include <string>

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800
#define target_fps 60

enum class AnimationState {
    Idle,
    Run
};

class Sprite {
public:
    // Constructor: loads image file and prepares texture.
    Sprite(SDL_Renderer* renderer, const std::string& idlefilePath, const std::string& runfilePath, int startx, int starty)
        : renderer(renderer), x_position(startx), y_position(starty) {
        // load textures from file path for the sprite
        idleTexture = IMG_LoadTexture(renderer, idlefilePath.c_str());
        runTexture = IMG_LoadTexture(renderer, runfilePath.c_str());
    }

    // Destructor: clean up the texture.
  /*  ~Sprite() {
        SDL_DestroyTexture(texture);
    }*/
 
    // Render the sprite at position (x, y).
    void render(SDL_Rect &source_rect) const {
        SDL_Texture* currentTexture = nullptr;
        //check current animation state to get the right rexture
        if (currentState == AnimationState::Idle) {
            currentTexture = idleTexture;
        }
        else if (currentState == AnimationState::Run) {
            currentTexture = runTexture;
        }
		//adjust source rectangle based on frame index
        source_rect.y = 0; // reset before using frame index to move to correct frame
        source_rect.y = source_rect.y + (frameindex*source_rect.h) ;
		printf("source rect y is: %i \n", source_rect.y);
        SDL_Rect destRect = { x_position, y_position, source_rect.w, source_rect.h };
        SDL_RenderCopy(renderer, currentTexture, &source_rect, &destRect);
        printf("current frame index is: %i \n", frameindex);
    }
    //helper function to change the animation state of the sprite in other member functions
    void setAnimationState(AnimationState newState) {
        currentState = newState;
    }
    //updates the frame index if enough time has passed so we can mnove to the next frame for animation
    void updateframeindex(float deltatime) {
        frameTimer += deltatime;
        if (frameTimer > framedelay) {
            frameindex = (frameindex + 1) % 6;
			frameTimer = 0.0f; // reset the timer
        }
    }
// move the sprite based on user input and change animation state accordingly
    void input_update(const Uint8* keystate, float deltatime) {
        // Handle user input here, e.g., move the sprite based on what key is held down
        if (keystate[SDL_SCANCODE_A] == 1) {
            // change to running state
            setAnimationState(AnimationState::Run);
            x_position += -movement_speed * deltatime; // move left 
            
        }
        else if (keystate[SDL_SCANCODE_D] == 1) {
            setAnimationState(AnimationState::Run);
            x_position += movement_speed * deltatime; // move right
            
        }
        else {
            setAnimationState(AnimationState::Idle);
        }
        
	}
    // function to handle moving to next source_rect for sprite animation
   /* void next_idle_frame*/

    // Accessor functions.
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    SDL_Renderer* renderer;  // Our renderer stored from the initializer list.
    SDL_Texture* idleTexture;
    SDL_Texture* runTexture;
    int width;
    int height;
    int x_position;
    int y_position;
	int movement_speed = 250; // Speed of movement, can be adjusted.
    AnimationState currentState;
    int frameindex = 0;
	float frameTimer = 0.0f; // Time per frame in seconds.
	float framedelay = 0.1f; // Delay between frames in seconds.
};

int main(int argc, char* argv[])
{
    // Initialize SDL's video subsystem.
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Initialization error: "
            << SDL_GetError() << std::endl;
        return -1;
    }

    // Initialize SDL_image to support PNG files.
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init error: "
            << IMG_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create an SDL window.
    SDL_Window* window = SDL_CreateWindow("Simple 2D Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window error: "
            << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Create an SDL renderer.
    // Request hardware acceleration and vertical sync.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        std::cerr << "Renderer error: "
            << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }
    //define a source rectangle for the sprite
    SDL_Rect source_rect;
    source_rect.x = 0;
    source_rect.y = 0;
    source_rect.w = 64;
    source_rect.h = 64;
    // Create a player sprite and NPC sprites
    // multiple player sprites for different animations
    Sprite player(renderer,"W_witch_idle.png","W_witch_run.png", 0, WINDOW_HEIGHT - source_rect.h);

    bool game_running = true;
    SDL_Event event;
    int desiredframetime = 1000.0f / target_fps;
    Uint32 lastTime = SDL_GetTicks();


    // Main game loop.
    while (game_running) {
        Uint32 currentTime = SDL_GetTicks(); // in milliseconds
        float deltaTime = (currentTime - lastTime) / 1000.0f; // convert to seconds
        lastTime = currentTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                game_running = false;
            if (event.key.keysym.sym == SDLK_ESCAPE)
                game_running = false;
            break;
        }
	  // Clear the screen (black).
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        // Handle user input for the player sprite.
        const Uint8* keystate = SDL_GetKeyboardState(nullptr);
        player.input_update(keystate, deltaTime);

        // Render the player sprite.
        player.render(source_rect);
       
        // Present the updated frame.
        SDL_RenderPresent(renderer);
        //after the right frame time has passed, change the frame of the sprite sheet
        player.updateframeindex(deltaTime);
		// Limit the frame rate
        if (currentTime - lastTime < desiredframetime) {
            SDL_Delay(desiredframetime - (currentTime - lastTime));
        }
      
    }

    // Clean up.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
