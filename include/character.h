#ifndef CHARACTER_H
#define CHARACTER_H

#include "main.h"
#include <stdbool.h>

#define MAX_FRAMES 10

typedef enum {
    IDLE,
    RUN,
    JUMP,
    ATTACK,
    DEFEND
} ActionState;

typedef struct {
    SDL_Texture* frames[MAX_FRAMES];
    int frame_count;
    int current_frame;
    int frame_delay; // milliseconds
} Animation;

typedef struct {
    Animation idle;
    Animation run;
    Animation jump;
    Animation attack;
    Animation protection;
    ActionState currentState;
    SDL_Rect position;
    Uint32 lastFrameTime;
    int direction; // 1 for right, -1 for left
    int player_id; // 1 or 2 to identify which player
    
    // Jump physics variables
    float velocity_y;     // Vertical velocity
    float ground_y;       // Ground position
    bool is_jumping;      // Jump state flag
    bool is_grounded;   // Grounded state flag
     bool move_left;
    bool move_right;
    bool jump_pressed;
    bool attack_pressed;
    bool defend_pressed;
     int health;
    int max_health;
    bool is_attacking;
    bool is_defending;
    int attack_timer;
    int attack_duration;
    int attack_cooldown;
    int attack_damage;
    int defense_timer;
    bool was_hit;
    int hit_stun_timer;
    int knockback_x;
    bool is_stunned;
} Character;



void loadAnimation(SDL_Renderer* renderer, Animation* anim, const char* path_prefix, const char* action_name, int num_frames);
void renderCharacter(SDL_Renderer* renderer, Character* character);
void updateAnimation(Character* character);
void loadCharacterAnimations(SDL_Renderer* renderer, Character* character, const char* path_prefix);

void updateCharacterPosition(Character* character);
#endif 