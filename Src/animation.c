#include "main.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

int renderbattle(SDL_Renderer* renderer, Character* player1, Character* player2, SDL_Texture* background) {
    SDL_RenderClear(renderer);
    
    if (!background) {
        printf("Failed to load background texture: %s\n", IMG_GetError());
        return -1;
    }
    SDL_RenderCopy(renderer, background, NULL, NULL);
    
    // Make characters face each other during combat
    if (player1->position.x < player2->position.x) {
        // Player1 is to the left of Player2
        if (player1->currentState == ATTACK || player1->currentState == IDLE) {
            player1->direction = 1;  // Face right
        }
        if (player2->currentState == ATTACK || player2->currentState == IDLE) {
            player2->direction = -1; // Face left
        }
    } else {
        // Player1 is to the right of Player2
        if (player1->currentState == ATTACK || player1->currentState == IDLE) {
            player1->direction = -1; // Face left
        }
        if (player2->currentState == ATTACK || player2->currentState == IDLE) {
            player2->direction = 1;  // Face right
        }
    }
    
    // Update positions and animations
    updateCharacterPosition(player1);
    updateCharacterPosition(player2);
    updateAnimation(player1);
    updateAnimation(player2);
    
    // Render both characters
    renderCharacter(renderer, player1);
    renderCharacter(renderer, player2);
    
    return 0;
}



// Simplified key handling - just sets input flags
void handleKeyPress(SDL_Event event, Character* player1, Character* player2) {
    bool key_down = (event.type == SDL_KEYDOWN);
    
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            // Player 1 Controls
            case SDLK_a: // Move left
                player1->move_left = key_down;
                break;
            case SDLK_d: // Move right
                player1->move_right = key_down;
                break;
            case SDLK_w: // Jump
            case SDLK_e: // Alternative jump
                if (key_down) player1->jump_pressed = true;
                break;
            case SDLK_r: // Attack
                if (key_down) player1->attack_pressed = true;
                break;
            case SDLK_s: // Defend
            case SDLK_t: // Alternative defend
                player1->defend_pressed = key_down;
                break;
            
            // Player 2 Controls
            case SDLK_LEFT:
                player2->move_left = key_down;
                break;
            case SDLK_RIGHT:
                player2->move_right = key_down;
                break;
            case SDLK_UP:
            case SDLK_o: // Alternative jump
                if (key_down) player2->jump_pressed = true;
                break;
            case SDLK_p: // Attack
                if (key_down) player2->attack_pressed = true;
                break;
            case SDLK_DOWN:
            case SDLK_LEFTBRACKET: // Alternative defend
                player2->defend_pressed = key_down;
                break;
        }
    }
}
