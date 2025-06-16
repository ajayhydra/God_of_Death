
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "main.h"
#include "character.h"
#include "menu.h"

int character_count = 0;
const int SCREEN_WIDTH = 800;     // Adjust to your screen width
const int SCREEN_HEIGHT = 600;    // Adjust to your screen height
const int GROUND_LEVEL = 450;     // Adjust based on your battleground image
const int CHARACTER_WIDTH = 150;  // Smaller, more appropriate size
const int CHARACTER_HEIGHT = 170; // Smaller, more appropriate size
Mix_Music *bgMusic = NULL;
void game_init(Game *game)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize: %s\n", SDL_GetError());
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("SDL_image could not initialize: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }
     if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        exit(1);
    }

    if (TTF_Init() == -1)
    {
        printf("SDL_ttf could not initialize: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    game->window = SDL_CreateWindow("Samurai Game",
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    SCREEN_WIDTH, SCREEN_HEIGHT,
                                    SDL_WINDOW_SHOWN);
    if (!game->window)
    {
        printf("Window could not be created: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    if (!game->renderer)
    {
        printf("Renderer could not be created: %s\n", SDL_GetError());
        SDL_DestroyWindow(game->window);
        IMG_Quit();
        SDL_Quit();
        return;
    }

    game->font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 20);
    game->headerFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 50);

    if (!game->font || !game->headerFont)
    {
        printf("Failed to load font: %s\n", TTF_GetError());
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }
}

SDL_Texture *loadTexture(const char *file, SDL_Renderer *renderer)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, file);
    if (!texture)
    {
        printf("Failed to load texture: %s\n", IMG_GetError());
    }
    return texture;
}

// Render text inside a rectangle button
void renderButton(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color textColor, SDL_Rect buttonRect, SDL_Color buttonColor)
{
    SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(renderer, &buttonRect);

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textW = textSurface->w;
    int textH = textSurface->h;
    SDL_Rect textRect = {
        buttonRect.x + (buttonRect.w - textW) / 2,
        buttonRect.y + (buttonRect.h - textH) / 2,
        textW,
        textH};

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

// Render full main menu
void renderMainMenu(SDL_Renderer *renderer, SDL_Texture *background, TTF_Font *font_title, TTF_Font *font_button)
{
    SDL_RenderClear(renderer);

    // Render background
    SDL_RenderCopy(renderer, background, NULL, NULL);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color buttonColor = {50, 50, 200, 255};

    // Title text
    SDL_Surface *titleSurface = TTF_RenderText_Blended(font_title, "Samurai Fight!", white);
    SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    int titleW = titleSurface->w;
    int titleH = titleSurface->h;
    SDL_Rect titleRect = {(SCREEN_WIDTH - titleW) / 2, 50, titleW, titleH};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Start button
    // SDL_Rect startButton = {300, 250, 200, 60};
    SDL_Rect startButton = {620, 300, 150, 40};

    renderButton(renderer, font_button, "Start Game", white, startButton, buttonColor);

    // Settings button
    SDL_Rect settingsButton = {620, 350, 150, 40};
    renderButton(renderer, font_button, "Settings", white, settingsButton, buttonColor);

    // Exit button
    SDL_Rect exitButton = {620, 400, 150, 40};
    renderButton(renderer, font_button, "Exit", white, exitButton, buttonColor);

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    Game game;
    GameState gameState = MAIN_MENU;
    game_init(&game);

    game.background = loadTexture("/mnt/j/Game_fighthing/Assets/Background.jpg", game.renderer);
    game.battleground = loadTexture("/mnt/j/Game_fighthing/Assets/Battleground2.png", game.renderer);
    

    bgMusic = Mix_LoadMUS("/mnt/j/Game_fighthing/Assets/backgroundmusic.mp3");
    if (bgMusic == NULL)
    {
        printf("Failed to load background music: %s\n", Mix_GetError());
    }
    else
    {
        Mix_PlayMusic(bgMusic, -1); // -1 means loop forever
    }

    if (!game.background)
    {
        printf("Failed to load background!\n");
        return -1;
    }

    game.event = (SDL_Event){0};
    bool running = true;
    Character player1, player2;

    // Initialize character positions with proper ground level
    player1.position = (SDL_Rect){150, GROUND_LEVEL - CHARACTER_HEIGHT, CHARACTER_WIDTH, CHARACTER_HEIGHT};
    player2.position = (SDL_Rect){SCREEN_WIDTH - 270, GROUND_LEVEL - CHARACTER_HEIGHT, CHARACTER_WIDTH, CHARACTER_HEIGHT};

    // Set ground reference for physics
    player1.ground_y = GROUND_LEVEL - CHARACTER_HEIGHT;
    player2.ground_y = GROUND_LEVEL - CHARACTER_HEIGHT;

    // Initialize physics properties
    player1.is_grounded = true;
    player1.is_jumping = false;
    player1.velocity_y = 0.0f;
    player1.direction = 1; // Facing right
    player1.currentState = IDLE;
    player1.player_id = 1;

    player2.is_grounded = true;
    player2.is_jumping = false;
    player2.velocity_y = 0.0f;
    player2.direction = -1; // Facing left
    player2.currentState = IDLE;
    player2.player_id = 2;

    // Initialize input flags (if using the improved input system)
    player1.move_left = false;
    player1.move_right = false;
    player1.jump_pressed = false;
    player1.attack_pressed = false;
    player1.defend_pressed = false;

    player2.move_left = false;
    player2.move_right = false;
    player2.jump_pressed = false;
    player2.attack_pressed = false;
    player2.defend_pressed = false;

    // Load character animations
    loadCharacterAnimations(game.renderer, &player1, "/mnt/j/Game_fighthing/Assets/samurai1");
    printf("Player1: idle frames loaded: %d\n", player1.idle.frame_count);
    loadCharacterAnimations(game.renderer, &player2, "/mnt/j/Game_fighthing/Assets/samurai2");
    printf("Player2: idle frames loaded: %d\n", player2.idle.frame_count);
    initializeCombatStats(&player1);
    initializeCombatStats(&player2);

    // Debug: Print initial positions
    printf("Player1 initial position: x=%d, y=%d (ground_y=%d)\n",
           player1.position.x, player1.position.y, (int)player1.ground_y);
    printf("Player2 initial position: x=%d, y=%d (ground_y=%d)\n",
           player2.position.x, player2.position.y, (int)player2.ground_y);

    while (running)
    {
        // Handle events
        while (SDL_PollEvent(&game.event))
        {
            if (game.event.type == SDL_QUIT)
            {
                running = false;
            }

            if (gameState == MAIN_MENU)
            {
                if (game.event.type == SDL_MOUSEBUTTONDOWN && game.event.button.button == SDL_BUTTON_LEFT)
                {
                    int x = game.event.button.x;
                    int y = game.event.button.y;

                    // Check Start Game button
                    if (x >= 620 && x <= 770 && y >= 300 && y <= 340)
                    {
                        gameState = CHARACTER_SELECT;
                    }
                    // Check Settings button
                    else if (x >= 620 && x <= 770 && y >= 350 && y <= 390)
                    {
                        printf("Settings button clicked\n");
                    }
                    // Check Exit button
                    else if (x >= 620 && x <= 770 && y >= 400 && y <= 440)
                    {
                        running = false;
                    }
                }
            }
            else if (gameState == CHARACTER_SELECT)
            {
                if (game.event.type == SDL_MOUSEBUTTONDOWN && game.event.button.button == SDL_BUTTON_LEFT)
                {
                    int x = game.event.button.x;
                    int y = game.event.button.y;
                    selectcharacter(x, y, &gameState, &character_count);
                }
            }
            else if (gameState == BATTLE)
            {
                // Handle key presses for battle
                handleCombatInput(game.event, &player1, &player2);

                // Debug: Print positions when keys are pressed
                if (game.event.type == SDL_KEYDOWN)
                {
                    printf("After keypress - Player1: x=%d, y=%d, HP=%d | Player2: x=%d, y=%d, HP=%d\n",
                           player1.position.x, player1.position.y, player1.health,
                           player2.position.x, player2.position.y, player2.health);
                }
            }
            else if (gameState == GAME_RUNNING)
            {
                // Handle game running events
            }
            else if (gameState == GAME_PAUSED)
            {
                // Handle pause events
            }
            else if (gameState == GAME_OVER)
            {
                if (game.event.type == SDL_MOUSEBUTTONDOWN && game.event.button.button == SDL_BUTTON_LEFT)
                {
                    int x = game.event.button.x;
                    int y = game.event.button.y;

                    // Check Restart button
                    if (x >= 300 && x <= 500 && y >= 400 && y <= 450)
                    {
                        gameState = MAIN_MENU; // Restart to main menu
                        character_count = 0;   // Reset character count
                        printf("Game restarted!\n");
                    }
                    // Check Exit button
                    else if (x >= 300 && x <= 500 && y >= 460 && y <= 510)
                    {
                        running = false; // Exit the game
                        printf("Exiting game...\n");
                    }
                }
            }
        }
        if (gameState == BATTLE)
        {
            // Update combat system
            updateCombat(&player1, &player2);

            // Update animations every frame
            updateAnimation(&player1);
            updateAnimation(&player2);

            // Update positions based on current states
            updateCharacterPosition(&player1);
            updateCharacterPosition(&player2);

            // Handle collision (allows close combat)
          //  handleFightingCollision(&player1, &player2);

            // Check for game over
            if (player1.health <= 0)
            {
                printf("Player 2 Wins!\n");
                gameState = GAME_OVER; // Uncomment when you add game over state
            }
            else if (player2.health <= 0)
            {
                printf("Player 1 Wins!\n");
                gameState = GAME_OVER; // Uncomment when you add game over state
            }
        }

        // Render according to game state
        if (gameState == MAIN_MENU)
        {
            renderMainMenu(game.renderer, game.background, game.headerFont, game.font);
        }
        else if (gameState == CHARACTER_SELECT)
        {
            renderCharacterSelect(game.renderer, game.headerFont, game.font);
        }
        else if (gameState == BATTLE)
        {
            // Render the battle scene
            if (renderbattle(game.renderer, &player1, &player2, game.battleground) != 0)
            {
                printf("Error rendering battle scene\n");
            }
        }
        else if (gameState == GAME_RUNNING)
        {
            // Render game running logic
        }
        else if (gameState == GAME_PAUSED)
        {
            // Render pause logic
        }
        else if (gameState == GAME_OVER)
        {
            Mix_PauseMusic(); // Pause music on game over
            SDL_RenderClear(game.renderer);
            // Render game over screen
            SDL_Color white = {255, 255, 255, 255};
            SDL_Color buttonColor = {50, 50, 200, 255};

            SDL_Rect restartButton = {300, 400, 200, 50};
            renderButton(game.renderer, game.font, "Restart", white, restartButton, buttonColor);

            SDL_Rect exitButton = {300, 460, 200, 50};
            renderButton(game.renderer, game.font, "Exit", white, exitButton, buttonColor);
        }

        SDL_RenderPresent(game.renderer);
        SDL_Delay(16); // ~60 FPS
    }

    cleanup(game);
    return 0;
}

// Additional helper function you might want to add
void debugCharacterStates(Character *player1, Character *player2)
{
    const char *stateNames[] = {"IDLE", "RUN", "JUMP", "ATTACK", "DEFEND"};
    printf("Player1 State: %s, Frame: %d | Player2 State: %s, Frame: %d\n",
           stateNames[player1->currentState], player1->idle.current_frame,
           stateNames[player2->currentState], player2->idle.current_frame);
}

// Add cleanup functions for proper memory management
void cleanupCharacter(Character *character)
{
    if (!character)
        return;

    // Clean up idle animation frames
    for (int i = 0; i < character->idle.frame_count; i++)
    {
        if (character->idle.frames[i])
        {
            SDL_DestroyTexture(character->idle.frames[i]);
            character->idle.frames[i] = NULL;
        }
    }

    // Clean up run animation frames
    for (int i = 0; i < character->run.frame_count; i++)
    {
        if (character->run.frames[i])
        {
            SDL_DestroyTexture(character->run.frames[i]);
            character->run.frames[i] = NULL;
        }
    }

    // Clean up jump animation frames
    for (int i = 0; i < character->jump.frame_count; i++)
    {
        if (character->jump.frames[i])
        {
            SDL_DestroyTexture(character->jump.frames[i]);
            character->jump.frames[i] = NULL;
        }
    }

    // Clean up attack animation frames
    for (int i = 0; i < character->attack.frame_count; i++)
    {
        if (character->attack.frames[i])
        {
            SDL_DestroyTexture(character->attack.frames[i]);
            character->attack.frames[i] = NULL;
        }
    }

    // Clean up protection animation frames
    for (int i = 0; i < character->protection.frame_count; i++)
    {
        if (character->protection.frames[i])
        {
            SDL_DestroyTexture(character->protection.frames[i]);
            character->protection.frames[i] = NULL;
        }
    }

    printf("Character cleanup completed\n");
}

void cleanup(Game game)
{
    // Clean up game textures
    if (game.background)
    {
        SDL_DestroyTexture(game.background);
        game.background = NULL;
        printf("Background texture destroyed\n");
    }

    if (game.battleground)
    {
        SDL_DestroyTexture(game.battleground);
        game.battleground = NULL;
        printf("Battleground texture destroyed\n");
    }

    // Clean up fonts if they exist
    if (game.font)
    {
        TTF_CloseFont(game.font);
        game.font = NULL;
        printf("Font cleaned up\n");
    }

    if (game.headerFont)
    {
        TTF_CloseFont(game.headerFont);
        game.headerFont = NULL;
        printf("Header font cleaned up\n");
    }

    // Clean up renderer
    if (game.renderer)
    {
        SDL_DestroyRenderer(game.renderer);
        game.renderer = NULL;
        printf("Renderer destroyed\n");
    }

    // Clean up window
    if (game.window)
    {
        SDL_DestroyWindow(game.window);
        game.window = NULL;
        printf("Window destroyed\n");
    }
    Mix_FreeMusic(bgMusic);
    Mix_CloseAudio();
    bgMusic = NULL;
    // Quit SDL subsystems
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    printf("SDL cleanup completed\n");
}
