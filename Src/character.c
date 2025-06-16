#include "main.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


int renderCharacterSelect(SDL_Renderer* renderer, TTF_Font* font_title, TTF_Font* font_button) {
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color buttonColor = {0, 0, 0, 255};

    SDL_Surface* titleSurface = TTF_RenderText_Blended(font_title, "Select Your Character", white);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    int titleW = titleSurface->w;
    int titleH = titleSurface->h;
    SDL_Rect titleRect = { (800 - titleW)/2, 50, titleW, titleH };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Load character images
    SDL_Texture* warriorImg = IMG_LoadTexture(renderer, "/mnt/j/Game_fighthing/Assets/ch-1.png");
    SDL_Texture* mageImg = IMG_LoadTexture(renderer, "/mnt/j/Game_fighthing/Assets/ch-2.png");
    SDL_Texture* rogueImg = IMG_LoadTexture(renderer, "/mnt/j/Game_fighthing/Assets/ch-3.png");

    // Render images above buttons
    SDL_Rect warriorRect = { 200, 220, 100, 100 };
    SDL_Rect mageRect    = { 550, 220, 100, 100 };
    SDL_Rect rogueRect   = { 375, 220, 100, 100 };

    SDL_RenderCopy(renderer, warriorImg, NULL, &warriorRect);
    SDL_RenderCopy(renderer, mageImg, NULL, &mageRect);
    SDL_RenderCopy(renderer, rogueImg, NULL, &rogueRect);

    // Buttons
    SDL_Rect char1 = { 150, 340, 150, 60 };
    renderButton(renderer, font_button, "Warrior", white, char1, buttonColor);

    SDL_Rect char2 = { 550, 340, 150, 60 };
    renderButton(renderer, font_button, "Mage", white, char2, buttonColor);

    SDL_Rect char3 = { 350, 340, 150, 60 };
    renderButton(renderer, font_button, "Rogue", white, char3, buttonColor);

    SDL_RenderPresent(renderer);

    // Cleanup
    SDL_DestroyTexture(warriorImg);
    SDL_DestroyTexture(mageImg);
    SDL_DestroyTexture(rogueImg);
    
    return 0;
}
int selectcharacter(int x, int y, GameState* gameState, int* character_count) {
    SDL_Rect warriorButton = { 150, 340, 150, 60 };
    SDL_Rect mageButton = { 550, 340, 150, 60 };
    SDL_Rect rogueButton = { 350, 340, 150, 60 };

    if (x >= warriorButton.x && x <= (warriorButton.x + warriorButton.w) &&
        y >= warriorButton.y && y <= (warriorButton.y + warriorButton.h)) {
        (*character_count)++;
        printf("Warrior selected!\n");
    }
    if (x >= mageButton.x && x <= (mageButton.x + mageButton.w) &&
        y >= mageButton.y && y <= (mageButton.y + mageButton.h)) {
        (*character_count)++;
        printf("Mage selected!\n");
    }
    if (x >= rogueButton.x && x <= (rogueButton.x + rogueButton.w) &&
        y >= rogueButton.y && y <= (rogueButton.y + rogueButton.h)) {
        (*character_count)++;
        printf("Rogue selected!\n");
    }
    if (*character_count > 1) {
        *gameState = BATTLE; // Change game state to BATTLE after character selection
    }
    return 0;
}


void loadAnimation(SDL_Renderer* renderer, Animation* anim, const char* path_file, const char *action, int frameCount) {
    anim->frame_count = frameCount;
    anim->current_frame = 0;
    
    // Fix: Use 0-based indexing for array
    for (int i = 0; i < frameCount; i++) {
        char path[256];
        snprintf(path, sizeof(path), "%s/%s/frame%d.png", path_file, action, i + 1); // File names still start from 1
        SDL_Surface* surface = IMG_Load(path);
        if (!surface) {
            printf("Failed to load frame %d for %s: %s\n", i + 1, action, IMG_GetError());
            anim->frames[i] = NULL;
            continue;
        }
        anim->frames[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void renderCharacter(SDL_Renderer* renderer, Character* character) {
    if (!character) return;
    
    Animation* anim = NULL;
    switch (character->currentState) {
        case IDLE: anim = &character->idle; break;
        case RUN: anim = &character->run; break;
        case JUMP: anim = &character->jump; break;
        case ATTACK: anim = &character->attack; break;
        case DEFEND: anim = &character->protection; break;
    }
    
    if (anim && anim->current_frame < anim->frame_count && anim->frames[anim->current_frame]) {
        // Use SDL_RenderCopyEx for flipping
        SDL_RendererFlip flip = (character->direction == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderCopyEx(renderer, anim->frames[anim->current_frame], NULL, 
                        &character->position, 0.0, NULL, flip);
    }
}

void updateAnimation(Character* character) {
    if (!character) return;  // null check
    Uint32 currentTime = SDL_GetTicks();
    
    if (currentTime - character->lastFrameTime > 100) {  // 100 ms per frame
        Animation* anim = NULL;
        switch (character->currentState) {
            case IDLE: anim = &character->idle; break;
            case RUN: anim = &character->run; break;
            case JUMP: anim = &character->jump; break;
            case ATTACK: anim = &character->attack; break;
            case DEFEND: anim = &character->protection; break;
            default: anim = &character->idle; break;  // fallback to idle
        }

        if (anim && anim->frame_count > 0) {
            anim->current_frame = (anim->current_frame + 1) % anim->frame_count;
            character->lastFrameTime = currentTime;
        }
    }
}


void loadCharacterAnimations(SDL_Renderer* renderer, Character* character, const char* path_prefix) {


    loadAnimation(renderer, &character->idle, path_prefix, "idle", 3);
    loadAnimation(renderer, &character->run, path_prefix, "run", 5);
    loadAnimation(renderer, &character->jump, path_prefix, "jump", 6);
    loadAnimation(renderer, &character->attack, path_prefix, "attack", 3);
    loadAnimation(renderer, &character->protection, path_prefix, "protection", 2);

    character->currentState = IDLE;
    character->lastFrameTime = SDL_GetTicks();
}

// Enhanced character update that processes input flags
void updateCharacterPosition(Character* character) {
    if (!character) return;

    const int MOVE_SPEED = 5;
    const float JUMP_FORCE = -12.0f;
    const float GRAVITY = 0.8f;
    const float MAX_FALL_SPEED = 10.0f;

    // Process input flags when grounded
    if (character->is_grounded) {
        // Handle movement
        if (character->move_left && !character->move_right) {
            character->position.x -= MOVE_SPEED;
            character->direction = -1;
            character->currentState = RUN;
        }
        else if (character->move_right && !character->move_left) {
            character->position.x += MOVE_SPEED;
            character->direction = 1;
            character->currentState = RUN;
        }
        else if (!character->move_left && !character->move_right) {
            // Not moving horizontally
            if (character->currentState == RUN) {
                character->currentState = IDLE;
            }
        }

        // Handle jump
        if (character->jump_pressed) {
            character->currentState = JUMP;
            character->velocity_y = JUMP_FORCE;
            character->is_jumping = true;
            character->is_grounded = false;
            character->jump_pressed = false; // Reset flag
            printf("Player %d started jumping from y=%d\n", character->player_id, character->position.y);
        }

        // Handle attack
        if (character->attack_pressed) {
            character->currentState = ATTACK;
            character->attack_pressed = false; // Reset flag
        }

        // Handle defend
        if (character->defend_pressed) {
            character->currentState = DEFEND;
        }
        else if (character->currentState == DEFEND && !character->defend_pressed) {
            character->currentState = IDLE;
        }
    }

    // Apply gravity and handle air movement
    if (!character->is_grounded) {
        // Allow air control (reduced movement)
        if (character->move_left && !character->move_right) {
            character->position.x -= MOVE_SPEED / 2; // Reduced air control
            character->direction = -1;
        }
        else if (character->move_right && !character->move_left) {
            character->position.x += MOVE_SPEED / 2; // Reduced air control
            character->direction = 1;
        }

        character->velocity_y += GRAVITY;
        if (character->velocity_y > MAX_FALL_SPEED)
            character->velocity_y = MAX_FALL_SPEED;

        character->position.y += (int)character->velocity_y;

        // Check for landing
        if (character->position.y >= character->ground_y) {
            character->position.y = (int)character->ground_y;
            character->velocity_y = 0.0f;
            character->is_jumping = false;
            character->is_grounded = true;
            character->currentState = IDLE;
            printf("Player %d landed at y=%d\n", character->player_id, character->position.y);
        }
    }

    // Keep within screen bounds horizontally
    if (character->position.x < 0) character->position.x = 0;
    if (character->position.x > 800 - character->position.w) 
        character->position.x = 800 - character->position.w;

    // Prevent moving above screen
    if (character->position.y < 0) {
        character->position.y = 0;
        character->velocity_y = 0;
    }
}