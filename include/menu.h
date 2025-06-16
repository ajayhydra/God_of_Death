#ifndef MENU_H
#define MENU_H

#include "main.h"
#include "character.h"

void updateCombat(Character* player1, Character* player2);
void handleCombatInput(SDL_Event event, Character* player1, Character* player2);
bool isInAttackRange(Character* attacker, Character* target);

bool checkAttackHit(Character* attacker, Character* target);
void dealDamage(Character* target, int damage);
void initializeCombatStats(Character* character);
void handleFightingCollision(Character* player1, Character* player2);
#endif