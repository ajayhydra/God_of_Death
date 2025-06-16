#include "menu.h"
#include "main.h"
#include "character.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


// Combat system functions
bool isInAttackRange(Character* attacker, Character* target) {
    const int ATTACK_RANGE = 80;
    int distance;
    
    if (attacker->direction > 0) { // Facing right
        distance = target->position.x - (attacker->position.x + attacker->position.w);
    } else { // Facing left
        distance = attacker->position.x - (target->position.x + target->position.w);
    }
    
    return (distance >= 0 && distance <= ATTACK_RANGE);
}

bool checkAttackHit(Character* attacker, Character* target) {
    // Must be in attack state and in range
    if (!attacker->is_attacking || attacker->attack_timer < 5 || attacker->attack_timer > 15) {
        return false; // Attack only hits during specific frames
    }
    
    if (!isInAttackRange(attacker, target)) {
        return false;
    }
    
    // Check if target is defending
    if (target->is_defending && target->defense_timer > 0) {
        printf("Attack blocked!\n");
        return false; // Attack blocked
    }
    
    return true;
}

void dealDamage(Character* target, int damage) {
    target->health -= damage;
    target->was_hit = true;
    target->hit_stun_timer = 20; // 20 frames of hit stun
    target->is_stunned = true;
    
    // Knockback effect
    target->knockback_x = (target->position.x < 700) ? 30 : -30; // Push away from center
    
    printf("Hit! Health: %d/%d\n", target->health, target->max_health);
    
    if (target->health <= 0) {
        target->health = 0;
        printf("Player defeated!\n");
    }
}

void updateCombat(Character* player1, Character* player2) {
    // Update attack timers
    if (player1->is_attacking) {
        player1->attack_timer++;
        if (player1->attack_timer >= player1->attack_duration) {
            player1->is_attacking = false;
            player1->attack_timer = 0;
            player1->currentState = IDLE;
            player1->attack_cooldown = 30; // 30 frames cooldown
        }
    }
    
    if (player2->is_attacking) {
        player2->attack_timer++;
        if (player2->attack_timer >= player2->attack_duration) {
            player2->is_attacking = false;
            player2->attack_timer = 0;
            player2->currentState = IDLE;
            player2->attack_cooldown = 30;
        }
    }
    
    // Update cooldowns
    if (player1->attack_cooldown > 0) player1->attack_cooldown--;
    if (player2->attack_cooldown > 0) player2->attack_cooldown--;
    
    // Update defense timers
    if (player1->defense_timer > 0) player1->defense_timer--;
    if (player2->defense_timer > 0) player2->defense_timer--;
    
    // Update hit stun
    if (player1->hit_stun_timer > 0) {
        player1->hit_stun_timer--;
        if (player1->hit_stun_timer <= 0) {
            player1->is_stunned = false;
            player1->was_hit = false;
        }
    }
    
    if (player2->hit_stun_timer > 0) {
        player2->hit_stun_timer--;
        if (player2->hit_stun_timer <= 0) {
            player2->is_stunned = false;
            player2->was_hit = false;
        }
    }
    
    // Apply knockback
    if (player1->knockback_x != 0) {
        player1->position.x += (player1->knockback_x > 0) ? 2 : -2;
        player1->knockback_x += (player1->knockback_x > 0) ? -2 : 2;
        if (abs(player1->knockback_x) <= 2) player1->knockback_x = 0;
    }
    
    if (player2->knockback_x != 0) {
        player2->position.x += (player2->knockback_x > 0) ? 2 : -2;
        player2->knockback_x += (player2->knockback_x > 0) ? -2 : 2;
        if (abs(player2->knockback_x) <= 2) player2->knockback_x = 0;
    }
    
    // Check for hits
    if (checkAttackHit(player1, player2)) {
        dealDamage(player2, player1->attack_damage);
    }
    
    if (checkAttackHit(player2, player1)) {
        dealDamage(player1, player2->attack_damage);
    }
}

// Updated input handling for combat
void handleCombatInput(SDL_Event event, Character* player1, Character* player2) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            // Player 1 Controls
            case SDLK_a: // Move left
                if (!player1->is_stunned) player1->move_left = true;
                break;
            case SDLK_d: // Move right
                if (!player1->is_stunned) player1->move_right = true;
                break;
            case SDLK_w: // Jump
                if (!player1->is_stunned && player1->is_grounded) {
                    player1->jump_pressed = true;
                }
                break;
            case SDLK_r: // Attack
                if (!player1->is_stunned && !player1->is_attacking && 
                    player1->attack_cooldown <= 0 && player1->is_grounded) {
                    player1->is_attacking = true;
                    player1->attack_timer = 0;
                    player1->currentState = ATTACK;
                    printf("Player 1 attacks!\n");
                }
                break;
            case SDLK_t: // Defend
                if (!player1->is_stunned && player1->is_grounded) {
                    player1->is_defending = true;
                    player1->defense_timer = 60; // 1 second of defense
                    player1->currentState = DEFEND;
                }
                break;
            
            // Player 2 Controls
            case SDLK_LEFT:
                if (!player2->is_stunned) player2->move_left = true;
                break;
            case SDLK_RIGHT:
                if (!player2->is_stunned) player2->move_right = true;
                break;
            case SDLK_UP:
                if (!player2->is_stunned && player2->is_grounded) {
                    player2->jump_pressed = true;
                }
                break;
            case SDLK_p: // Attack
                if (!player2->is_stunned && !player2->is_attacking && 
                    player2->attack_cooldown <= 0 && player2->is_grounded) {
                    player2->is_attacking = true;
                    player2->attack_timer = 0;
                    player2->currentState = ATTACK;
                    printf("Player 2 attacks!\n");
                }
                break;
            case SDLK_o: // Defend
                if (!player2->is_stunned && player2->is_grounded) {
                    player2->is_defending = true;
                    player2->defense_timer = 60;
                    player2->currentState = DEFEND;
                }
                break;
        }
    }
    else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            // Player 1
            case SDLK_a: player1->move_left = false; break;
            case SDLK_d: player1->move_right = false; break;
            case SDLK_t: 
                player1->is_defending = false;
                if (player1->currentState == DEFEND) player1->currentState = IDLE;
                break;
            
            // Player 2
            case SDLK_LEFT: player2->move_left = false; break;
            case SDLK_RIGHT: player2->move_right = false; break;
            case SDLK_o:
                player2->is_defending = false;
                if (player2->currentState == DEFEND) player2->currentState = IDLE;
                break;
        }
    }
}

// Initialize combat stats (add this to your character initialization)
void initializeCombatStats(Character* character) {
    character->health = 100;
    character->max_health = 100;
    character->is_attacking = false;
    character->is_defending = false;
    character->attack_timer = 0;
    character->attack_duration = 20; // 20 frames for attack
    character->attack_cooldown = 0;
    character->attack_damage = 15;
    character->defense_timer = 0;
    character->was_hit = false;
    character->hit_stun_timer = 0;
    character->knockback_x = 0;
    character->is_stunned = false;
}

// Simple collision for fighting (allows close combat)
void handleFightingCollision(Character* player1, Character* player2) {
    const int SCREEN_WIDTH = 1400;
    
    // Only prevent complete overlap, allow close contact
    if (player1->position.x + player1->position.w > player2->position.x + 5 &&
        player1->position.x < player2->position.x + player2->position.w - 5) {
        
        // Minimal separation - just enough to not overlap
        int overlap = (player1->position.x + player1->position.w) - player2->position.x;
        if (overlap > 0) {
            player1->position.x -= (overlap + 2) / 2;
            player2->position.x += overlap / 2;
        }
        
        // Keep within bounds
        if (player1->position.x < 0) {
            player2->position.x += abs(player1->position.x);
            player1->position.x = 0;
        }
        if (player2->position.x + player2->position.w > SCREEN_WIDTH) {
            player1->position.x -= (player2->position.x + player2->position.w) - SCREEN_WIDTH;
            player2->position.x = SCREEN_WIDTH - player2->position.w;
        }
    }
}