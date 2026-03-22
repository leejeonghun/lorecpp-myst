#ifndef MYSTMENU_H
#define MYSTMENU_H

#include "mystsub.h"

void SPnotEnough();
void HealOne(byte whom);
void CureOne(byte whom);
void ConsciousOne(byte whom);
void RevitalizeOne(byte whom);
void HealAll();
void CureAll();
void ConsciousAll();
void RevitalizeAll();
void CureSpell();
void Wizard_Eye();
bool Teleport();
void PhenominaSpell();
void Inventory();
void ViewParty();
void ViewCharacter();
void QuickView();
void CastSpell();
int ReturnPredict();
void Extrasense();
void SetBackPack();
void Rest();
void Use_Item(int select_num);
void Use_Crystal();
void GameOption();
void SelectMode();

#endif