#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>  // use the proper casing on Windows
#include <iostream>
#include <string>

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800

class Sprite {
public:
    // Constructor: loads image file and prepares texture.
    Sprite(SDL_Renderer* renderer, const std::string& filePath, int startx, int starty)
        : renderer(renderer), x_position(startx), y_position(starty) {
        // Load image file into SDL_Surface.
        SDL_Surface* surface = IMG_Load(filePath.c_str());
        if (!surface) {
            std::cerr << "Failed to load " << filePath << ": "
                << IMG_GetError() << std::endl;
        }
        // Create a texture from the surface.
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            std::cerr << "Failed to create texture: "
                << SDL_GetError() << std::endl;
        }
        // Store dimensions for rendering.
        width = surface->w;
        height = surface->h;
        // Free the surface as it’s no longer needed.
        SDL_FreeSurface(surface);
    }

    // Destructor: clean up the texture.
    ~Sprite() {
        SDL_DestroyTexture(texture);
    }

    // Render the sprite at position (x, y).
    void render(const SDL_Rect &source_rect) const {
        SDL_Rect destRect = { x_position, y_position, source_rect.w, source_rect.h };
        SDL_RenderCopy(renderer, texture,&source_rect, &destRect);
        //update the private variables for position
    }
// move the sprite based on user input
    void input_update(const Uint8* keystate) {
        // Handle user input here, e.g., move the sprite based on what key is held down
        if (keystate[SDL_SCANCODE_A] == 1) {
            x_position += -movement_speed; // move left 
        }
        else if (keystate[SDL_SCANCODE_D] == 1) {
            x_position += movement_speed; // move right
        }
	}
    // function to handle moving to next source_rect for sprite animation
   /* void next_idle_frame*/

    // Accessor functions.
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    SDL_Renderer* renderer;  // Our renderer stored from the initializer list.
    SDL_Texture* texture;
    int width;
    int height;
    int x_position;
    int y_position;
	int movement_speed = 5; // Speed of movement, can be adjusted.
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
    source_rect.w = 60;
    source_rect.h = 60;
    // Create a player sprite and NPC sprites
    Sprite Player(renderer,"W_witch_idle.png", 0, WINDOW_HEIGHT - source_rect.h);
    //Sprite NPC1(renderer, "", 0, 0);

    bool game_running = true;
    SDL_Event event;

    // Main game loop.
    while (game_running) {
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

        // Render the player sprite.
        Player.render(source_rect);

        // Present the updated frame.
        SDL_RenderPresent(renderer);
		// Handle user input for the player sprite.
        const Uint8* keystate = SDL_GetKeyboardState(nullptr);
        Player.input_update(keystate);
        //each frame, change the frame of the sprite sheet (idle/)
        SDL_Delay(16);  // Roughly 60 FPS.
    }

    // Clean up.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
