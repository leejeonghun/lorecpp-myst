#ifndef MYSTBATT_H
#define MYSTBATT_H

#include "mystsub.h"

void EncounterEnemy();
void BattleMode(bool assault, bool auto_end);
void DisplayEnemies(bool clean);
bool ExistEnemies();
void AttackOne();
void PlusGold();
void PlusExperience(int p_idx, int e_idx);
bool EndBattle(int& exitcode);

#endif
