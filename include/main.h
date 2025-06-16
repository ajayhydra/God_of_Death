#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include "character.h"


// #define SCREEN_WIDTH 800
// #define SCREEN_HEIGHT 600


// Declare game states
typedef enum {
    MAIN_MENU,
    GAME_RUNNING,
    GAME_OVER,
    CHARACTER_SELECT,
    BATTLE,
    GAME_PAUSED
} GameState;

// Declare Game struct
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background;
    SDL_Event event;
    TTF_Font* font;
    TTF_Font* headerFont;
    SDL_Texture* battleground;
} Game;

// Declare functions
void game_init(Game* game);
void cleanup(Game game);
int renderCharacterSelect(SDL_Renderer* renderer, TTF_Font* font_title, TTF_Font* font_button);
void renderButton(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color textColor, SDL_Rect buttonRect, SDL_Color buttonColor);
SDL_Texture* loadTexture(const char* file, SDL_Renderer* renderer);
void renderMainMenu(SDL_Renderer* renderer, SDL_Texture* background, TTF_Font* font_title, TTF_Font* font_button);

int renderbattle(SDL_Renderer* renderer,Character* player1, Character* player2, SDL_Texture* background);
void handleKeyPress(SDL_Event event, Character* player1, Character* player2);
void debugCharacterStates(Character* player1, Character* player2) ;
void updateAnimation(Character* character);
int selectcharacter(int x, int y, GameState* gameState, int* character_count);
void cleanupCharacter(Character* character);

#endif
