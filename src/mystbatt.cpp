#include <cstring>
// --- START OF FILE MystBatt.cpp ---

#include "mystbatt.h"
#include "mystsub.h"
#include "uhanx.h"
#include "voice.h"
#include "mystmenu.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cmath>

// Externs for shared game state (from MystSub.cpp and globals)
extern byte battle[7][4];
extern byte enemynumber;
extern EnemyData2 enemy[9];
extern Lore player[8];
extern LorePlayer party;
extern std::string mapname;
extern std::string sexdata;
extern std::string s_val;
extern std::string m[11];
extern int person;
extern longint long_val;
extern bool SoundOn;
extern bool Cruel;

// Forward declarations for functions defined in MystSub/MystMenu or elsewhere
extern bool getbit(int bit);
extern void join(byte num, byte partynum);
extern void joinenemy(byte num, byte j_idx);
extern void turn_mind(byte j_idx, byte enemy_num);
extern std::string ReturnSJosa(std::string s);
extern std::string ReturnBJosa(std::string s);
extern std::string ReturnOJosa(std::string s);
extern std::string FromPas(const char* buf, uint8_t len);
extern std::string ReturnMessage(int who, int how, int what, int whom);
extern std::string ReturnMagic(int _class, int magic_idx);
extern std::string ReturnWeapon(int number);
extern void Print(int color, std::string s1);
extern void message(int color, std::string s_msg);
extern void cPrint(int color1, int color2, std::string s1, std::string s2, std::string s3);
extern void AuxPrint(int color, std::string s1, bool newline);
extern void Clear();
extern void PressAnyKey();
extern void Scroll(bool character);
extern void SimpleDisCond();
extern void Display_Condition();
extern void GameOver();
extern int Select(int yinit, int maxsum, int total, bool clean, bool lastclean);
extern int SelectEnemy();
extern bool exist(int p_idx);
extern void ReturnSexData();
extern void Use_Item(int val);
extern void CureSpell();

// Audio stubs - replace with actual sound calls if Voice.h implements them


bool autobattle = false;

// Helper function to set player names inside Summon
void SetPlayerName(int p_idx, const std::string& name) {
    player[p_idx].name_len = std::min((int)name.length(), 16);
    std::memcpy(player[p_idx].name, name.c_str(), player[p_idx].name_len);
}

void PlusExperience(int person_idx, int foe_idx) {
    long plus = enemy[foe_idx].level * 2;
    plus = (plus * plus * plus) / 8;
    if (plus <= 0) plus = 1;
    
    if (!enemy[foe_idx].unconscious) {
        std::string name = FromPas(player[person_idx].name, player[person_idx].name_len);
        cPrint(14, 11, name + ReturnSJosa(name) + " ", std::to_string(plus), "만큼 경험치를 얻었다 !");
        player[person_idx].experience += plus;
    } else {
        for (int i_idx = 1; i_idx <= 6; i_idx++) {
            if (exist(i_idx)) player[i_idx].experience += plus;
        }
    }
}

void PlusGold() {
    if (party.etc[6] == 2) return;
    long total_gold = 0;
    for (int i = 1; i <= enemynumber; i++) {
        if (enemy[i].E_number > 1) {
            int j = enemy[i].level * 2;
            if (j >= 75) j = 74; // 배열 Out of Bounds 방지
            int k_ac = EnemyData_Const[j].ac;
            if (k_ac == 0) k_ac = 1;
            long plus = EnemyData_Const[j].level;
            plus = plus * EnemyData_Const[j].level;
            plus = plus * EnemyData_Const[j].level;
            plus = plus * k_ac;
            total_gold += plus;
        }
    }
    party.gold += total_gold;
    message(15, "일행은 " + std::to_string(total_gold) + "개의 금을 얻었다.");
}

void DisplayEnemies(bool clean) {
    if (clean) {
        // 현재 텍스트 색상을 백업 (옵션이지만 안전한 상태 관리를 위해)
        int oldColor = GetColor();

        // 원본 파스칼의 SetFillStyle(1,0); 에 해당 (배경을 검은색으로)
        SetColor(0);
        Bar(24, 24, 25, 25);
        Bar(24, 24, 239, 287);

        // 원본 파스칼의 SetFillStyle(1,8); 에 해당 (채우기 색상을 다크 그레이로 변경)
        // C++ 포팅에서는 이 직후 for문 안에서 SetColor를 덮어쓰므로 
        // 텍스트 출력 자체에는 영향이 없지만 원본 상태 머신과 동일하게 맞춥니다.
        SetColor(8);
    }

    for (int i = 1; i <= enemynumber; i++) {
        EnemyData2& e = enemy[i];
        if (e.hp == 0) SetColor(8);
        else if (e.hp < 200) SetColor(12);
        else if (e.hp < 500) SetColor(4);
        else if (e.hp < 1000) SetColor(6);
        else if (e.hp < 2000) SetColor(14);
        else if (e.hp < 4000) SetColor(2);
        else SetColor(10);

        if (e.E_number == 0 || (e.specialcastlevel & 0x80) > 0) SetColor(10);
        if (e.unconscious) SetColor(8);
        if (e.dead) SetColor(0);

        OutHPrintXY(70, 20 * i + 50, e.name.c_str(), false);
    }
}

bool ExistEnemies() {
    for (int i = 1; i <= enemynumber; i++) {
        if (!enemy[i].dead) return true;
    }
    return false;
}

void AttackOne() {
    ReturnSexData();
    int k = battle[person][3];
    
    if ((enemy[k].unconscious && !Cruel) || enemy[k].dead) {
        if (!ExistEnemies()) return;
        battle[person][3]++;
        k++;
        if (battle[person][3] <= enemynumber) { AttackOne(); return; }
        return;
    }
    
    std::string s = ReturnMessage(person, battle[person][1], person, battle[person][3]);
    Print(15, s);
    
    std::string p_name = FromPas(player[person].name, player[person].name_len);
    
    if (enemy[k].unconscious && !enemy[k].dead) {
        switch (std::rand() % 4) {
            case 0: Print(12, p_name + "의 무기가 " + enemy[k].name + "의 심장을 꿰뚫었다"); break;
            case 1: Print(12, enemy[k].name + "의 머리는 " + p_name + "의 공격으로 산산 조각이 났다"); break;
            case 2: Print(12, "적의 피가 사방에 뿌려졌다"); break;
            case 3: Print(12, "적은 비명과 함께 찢겨 나갔다"); break;
        }
        if (SoundOn) scream2();
        PlusExperience(person, k);
        enemy[k].hp = 0;
        enemy[k].dead = true;
        return;
    }
    
    if ((std::rand() % 20) > player[person].accuracy) {
        Print(7, p_name + "의 공격은 빗나갔다 ....");
        return;
    }
    
    long _power = 0;
    int weapon_type = (player[person].weapon + 6) / 7;
    
    switch (weapon_type) {
        case 0: _power = player[person].raw_skills[5]; break; // fist_skill
        case 1: _power = player[person].raw_skills[0]; break; // sword_skill
        case 2: _power = player[person].raw_skills[1]; break; // axe_skill
        case 3: _power = player[person].raw_skills[2]; break; // spear_skill
        case 4: _power = player[person].raw_skills[3]; break; // bow_skill
        default: _power = player[person].level * 5; break;
    }
    
    if (player[person].classtype == sword && (player[person]._class == 5 || player[person]._class == 6)) {
        long_val = std::round(player[person].strength * _power * 5);
    } else {
        long_val = std::round(player[person].strength * player[person].wea_power * _power / 10.0);
    }
    
    long_val = long_val - (long_val * (std::rand() % 50) / 100);
    
    if ((std::rand() % 100) < enemy[k].resistance[0]) {
        Print(7, "적은 " + p_name + "의 공격을 저지했다");
        return;
    }
    
    long _ac = std::round(enemy[k].ac * enemy[k].level * ((std::rand() % 10) + 1));
    long_val -= _ac;
    
    if (long_val <= 0) {
        Print(7, "그러나, 적은 " + p_name + "의 공격을 막았다");
        return;
    }
    
    enemy[k].hp -= long_val;
    if (enemy[k].hp <= 0) {
        enemy[k].hp = 0;
        enemy[k].unconscious = false; enemy[k].dead = false;
        Print(12, "적은 " + p_name + "의 공격으로 의식불명이 되었다");
        if (SoundOn) hit();
        PlusExperience(person, k);
        enemy[k].unconscious = true;
    } else {
        cPrint(7, 15, "적은 ", std::to_string(long_val), "만큼의 피해를 입었다");
        if (SoundOn) hit();
    }
    DisplayEnemies(false);
}

void CastOne() {
    ReturnSexData();
    int j = battle[person][2];
    int k = battle[person][3];
    std::string p_name = FromPas(player[person].name, player[person].name_len);
    
    if ((enemy[k].unconscious && !Cruel) || enemy[k].dead) {
        if (!ExistEnemies()) return;
        battle[person][3]++;
        k++;
        if (battle[person][3] <= enemynumber) { CastOne(); return; }
        return;
    }
    
    std::string s;
    if (j < 11) {
        s = ReturnMessage(person, battle[person][1], battle[person][2], battle[person][3]);
    } else if (battle[person][1] == 2) {
        s = p_name + ReturnSJosa(p_name) + " 화염의 크리스탈을 사용했고, 거기서 생성된 화염폭풍은 " + enemy[k].name + "에게 명중했다.";
    } else {
        s = p_name + ReturnBJosa(p_name) + " 크리스탈로 생성시킨 한파의 회오리는 " + enemy[k].name + ReturnOJosa(enemy[k].name) + " 강타했다.";
    }
    Print(15, s);
    
    if (enemy[k].unconscious) {
        Print(12, p_name + "의 마법은 적을 완전히 제거해 버렸다");
        if (SoundOn) scream1();
        PlusExperience(person, k);
        enemy[k].hp = 0;
        enemy[k].dead = true;
        DisplayEnemies(false);
        return;
    }
    
    long i_dmg = 0;
    if (j < 11) {
        int attack_magic = player[person].raw_skills[0]; // mapped from attack_magic
        i_dmg = std::round(attack_magic * j * j / 10.0);
        if (player[person].sp < i_dmg) {
            Print(7, "마법 지수가 부족했다");
            return;
        }
        player[person].sp -= i_dmg;
        if ((std::rand() % 20) >= player[person].accuracy) {
            Print(7, "그러나, " + enemy[k].name + ReturnOJosa(enemy[k].name) + " 빗나갔다");
            return;
        }
        i_dmg = std::round(j * j * attack_magic * 3.0);
    } else if (battle[person][1] == 2) {
        i_dmg = 32500;
    } else {
        i_dmg = 20000;
    }
    
    if ((std::rand() % 100) < enemy[k].resistance[1]) {
        Print(7, enemy[k].name + ReturnSJosa(enemy[k].name) + " " + p_name + "의 마법을 저지했다");
        return;
    }
    
    long j_ac = std::round(enemy[k].ac * enemy[k].level * ((std::rand() % 10) + 1));
    i_dmg -= j_ac;
    
    if (i_dmg <= 0) {
        Print(7, "그러나, " + enemy[k].name + ReturnSJosa(enemy[k].name) + " " + p_name + "의 마법 공격을 막았다");
        return;
    }
    
    enemy[k].hp -= i_dmg;
    if (enemy[k].hp <= 0) {
        Print(12, enemy[k].name + ReturnSJosa(enemy[k].name) + " " + p_name + "의 마법에 의해 의식불능이 되었다");
        if (SoundOn) hit();
        PlusExperience(person, k);
        enemy[k].hp = 0;
        enemy[k].unconscious = true;
    } else {
        cPrint(7, 15, enemy[k].name + ReturnSJosa(enemy[k].name) + " ", std::to_string(i_dmg), "만큼의 피해를 입었다");
        if (SoundOn) hit();
    }
    DisplayEnemies(false);
}

void CastAll() {
    battle[person][3] = 1;
    while (battle[person][3] <= enemynumber) {
        CastOne();
        battle[person][3]++;
    }
}

void CastSpecial() {
    const int skill_point = 200;
    const int defence_point = 50;
    const int ability_point = 100;
    const int magic_point = 150;
    const int superman_point = 400;

    auto Remove_Skill = [&]() {
        EnemyData2& target = enemy[battle[person][3]];
        if (player[person].sp < skill_point) { Print(7, "마법 지수가 부족했다"); return; }
        player[person].sp -= skill_point;
        if ((std::rand() % 100) < target.resistance[1]) { Print(7, "기술 무력화 공격은 저지 당했다"); return; }
        if ((std::rand() % 60) > player[person].accuracy) { Print(7, "기술 무력화 공격은 빗나갔다"); return; }
        Print(4, target.name + "의 특수 공격 능력이 제거되었다");
        target.special = 0;
    };

    auto Remove_Defence = [&]() {
        EnemyData2& target = enemy[battle[person][3]];
        if (player[person].sp < defence_point) { Print(7, "마법 지수가 부족했다"); return; }
        player[person].sp -= defence_point;
        if ((std::rand() % 100) < target.resistance[0]) { Print(7, "방어 무력화 공격은 저지 당했다"); return; }
        int j = (target.ac < 5) ? 40 : 25;
        if ((std::rand() % j) > player[person].accuracy) { Print(7, "방어 무력화 공격은 빗나갔다"); return; }
        Print(4, target.name + "의 방어 능력이 저하되었다");
        if ((target.resistance[0] < 31) || (std::rand() % 2 == 0)) if(target.ac > 0) target.ac--;
        if (target.resistance[0] > 10) target.resistance[0] -= 10; else target.resistance[0] = 0;
        if (target.resistance[1] > 10) target.resistance[1] -= 10; else target.resistance[1] = 0;
    };

    auto Remove_Ability = [&]() {
        EnemyData2& target = enemy[battle[person][3]];
        if (player[person].sp < ability_point) { Print(7, "마법 지수가 부족했다"); return; }
        player[person].sp -= ability_point;
        if ((std::rand() % 200) < target.resistance[1]) { Print(7, "능력 저하 공격은 저지 당했다"); return; }
        if ((std::rand() % 30) > player[person].accuracy) { Print(7, "능력 저하 공격은 빗나갔다"); return; }
        Print(4, target.name + "의 전체적인 능력이 저하되었다");
        if (target.level > 1) target.level--;
        if (target.resistance[0] > 10) target.resistance[0] -= 10; else target.resistance[0] = 0;
        if (target.resistance[1] > 10) target.resistance[1] -= 10; else target.resistance[1] = 0;
    };

    auto Remove_Magic = [&]() {
        EnemyData2& target = enemy[battle[person][3]];
        if (player[person].sp < magic_point) { Print(7, "마법 지수가 부족했다"); return; }
        player[person].sp -= magic_point;
        if ((std::rand() % 100) < target.resistance[1]) { Print(7, "마법 불능 공격은 저지 당했다"); return; }
        if ((std::rand() % 100) > player[person].accuracy) { Print(7, "마법 불능 공격은 빗나갔다"); return; }
        if (target.castlevel > 1) Print(4, target.name + "의 마법 능력이 저하되었다");
        else Print(4, target.name + "의 마법 능력은 사라졌다");
        if (target.castlevel > 0) target.castlevel--;
    };

    auto Remove_Superman = [&]() {
        EnemyData2& target = enemy[battle[person][3]];
        if (player[person].sp < superman_point) { Print(7, "마법 지수가 부족했다"); return; }
        player[person].sp -= superman_point;
        if ((std::rand() % 100) < target.resistance[1]) { Print(7, "탈 초인화 공격은 저지 당했다"); return; }
        if ((std::rand() % 100) > player[person].accuracy) { Print(7, "탈 초인화 공격은 빗나갔다"); return; }
        if (target.specialcastlevel > 1) Print(4, target.name + "의 초자연적 능력이 저하되었다");
        else Print(4, target.name + "의 초자연적 능력은 사라졌다");
        if (target.specialcastlevel > 0) {
            int idx = 0;
            while (((target.specialcastlevel & (1 << idx)) > 0) || (idx == 8)) idx++;
            if (idx < 8) target.specialcastlevel ^= (1 << idx);
        }
    };

    switch (battle[person][2]) {
        case 1: Remove_Skill(); break;
        case 2: Remove_Defence(); break;
        case 3: Remove_Ability(); break;
        case 4: Remove_Magic(); break;
        case 5: Remove_Superman(); break;
        case 6: for (int i = 1; i <= enemynumber; i++) { battle[person][3] = i; if (player[person].sp >= skill_point) Remove_Skill(); } break;
        case 7: for (int i = 1; i <= enemynumber; i++) { battle[person][3] = i; if (player[person].sp >= defence_point) Remove_Defence(); } break;
        case 8: for (int i = 1; i <= enemynumber; i++) { battle[person][3] = i; if (player[person].sp >= ability_point) Remove_Ability(); } break;
        case 9: for (int i = 1; i <= enemynumber; i++) { battle[person][3] = i; if (player[person].sp >= magic_point) Remove_Magic(); } break;
        case 10:for (int i = 1; i <= enemynumber; i++) { battle[person][3] = i; if (player[person].sp >= superman_point) Remove_Superman(); } break;
    }
}

void BattleESP() {
    int esp_magic = player[person].raw_skills[4];
    ReturnSexData();
    std::string p_name = FromPas(player[person].name, player[person].name_len);
    
    if (battle[person][2] == 1 || battle[person][2] == 2 || battle[person][2] == 4) {
        Print(7, ReturnMagic(5, battle[person][2]) + Mokjuk + " 전투모드에서는 사용할 수가 없습니다.");
        return;
    }
    
    if (battle[person][2] == 3) {
        EnemyData2& target = enemy[battle[person][3]];
        if (esp_magic < 40) { Print(7, "독심을 사용하려 하였지만 아직 능력이 부족했다"); return; }
        if (player[person].sp < 15) { Print(7, "마법 지수가 부족했다"); return; }
        player[person].sp -= 15;
        
        bool valid_target = (target.E_number >= 6 && target.E_number <= 9) || 
                            (target.E_number >= 11 && target.E_number <= 16) || 
                            target.E_number == 18 || target.E_number == 19 || 
                            target.E_number == 25 || target.E_number == 26 || 
                            target.E_number == 47 || target.E_number == 64;
                            
        if (!valid_target) { Print(7, "독심술은 전혀 통하지 않았다"); return; }
        
        int lv = target.level;
        if ((lv > player[person].level) && (std::rand() % 2 == 0)) { Print(7, "적의 마음을 끌어들이기에는 아직 능력이 부족했다"); return; }
        if ((std::rand() % 60) > (player[person].level - lv) * 2 + player[person].concentration) { Print(7, "적의 마음은 흔들리지 않았다"); return; }
        
        Print(11, "적은 우리의 편이 되었다");
        join(target.E_number, 6);
        target.dead = true; target.unconscious = true; target.hp = 0; target.level = 0;
        Display_Condition();
    } else {
        switch (battle[person][2]) {
            case 6: {
                if (esp_magic < 20) { Print(7, p_name + "는 주위의 원소들을 이용하려 했으나 역부족이었다"); return; }
                if (player[person].sp < 100) { Print(7, "초감각 지수가 부족했다"); return; }
                player[person].sp -= 100;
                if (std::rand() % 2 == 0) {
                    Print(7, "갑자기 땅속의 우라늄이 핵분열을 일으켜 고온");
                    Print(7, "의 열기가 적의 주위를 감싸기 시작한다");
                } else {
                    Print(7, "공기중의 수소가 돌연히 핵융합을 일으켜 질량");
                    Print(7, "결손의 에너지를 적들에게 방출하기 시작한다");
                }
                for (int i = 1; i <= enemynumber; i++) {
                    long j_hp = enemy[i].hp;
                    if (j_hp < player[person].concentration * player[person].level) j_hp = 0;
                    else j_hp -= player[person].concentration * player[person].level;
                    enemy[i].hp = j_hp;
                    
                    if (enemy[i].unconscious && !enemy[i].dead) {
                        enemy[i].dead = true;
                        PlusExperience(person, i);
                    }
                    if (j_hp == 0 && !enemy[i].unconscious) {
                        enemy[i].unconscious = true;
                        PlusExperience(person, i);
                    }
                }
                break;
            }
            case 7: {
                Print(7, p_name + "는 적에게 공포심을 불어 넣었다");
                if (esp_magic < 30) { Print(7, p_name + "는 아직 능력이 부족 했다"); return; }
                if (player[person].sp < 100) { Print(7, "마법 지수가 부족했다"); return; }
                player[person].sp -= 100;
                int j = battle[person][3];
                if ((std::rand() % 40) < enemy[j].resistance[1]) {
                    if (enemy[j].resistance[0] < 5) enemy[j].resistance[0] = 0; else enemy[j].resistance[0] -= 5;
                    if (enemy[j].resistance[1] < 5) enemy[j].resistance[1] = 0; else enemy[j].resistance[1] -= 5;
                    return;
                }
                if ((std::rand() % 60) > player[person].concentration) {
                    if (enemy[j].endurance < 5) enemy[j].endurance = 0; else enemy[j].endurance -= 5;
                    return;
                }
                enemy[j].dead = true;
                Print(10, enemy[j].name + "는 겁을 먹고는 도망 가버렸다");
                PlusExperience(person, j);
                break;
            }
            case 8: {
                Print(7, p_name + "는 적을 환상속에 빠지게 하려한다");
                if (esp_magic < 80) { Print(7, p_name + "는 아직 능력이 부족 했다"); return; }
                if (player[person].sp < 300) { Print(7, "마법 지수가 부족했다"); return; }
                player[person].sp -= 300;
                int j = battle[person][3];
                if ((std::rand() % 30) < player[person].concentration) {
                    for (int i = 0; i < 2; i++) {
                        if (enemy[j].accuracy[i] < 4) enemy[j].accuracy[i] = 0; else enemy[j].accuracy[i] -= 4;
                    }
                }
                if ((std::rand() % 40) < enemy[j].resistance[1]) {
                    if (enemy[j].agility < 5) enemy[j].agility = 0; else enemy[j].agility -= 5;
                    return;
                }
                break;
            }
            case 9: {
                AuxPrint(7, p_name + "는 적의 신진 대사를 조절하여 적의 ", true);
                AuxPrint(7, "체력을 점차 약화 시키려 한다", false);
                if (esp_magic < 90) { Print(7, p_name + "는 아직 능력이 부족 했다"); return; }
                if (player[person].sp < 500) { Print(7, "마법 지수가 부족했다"); return; }
                player[person].sp -= 500;
                if ((std::rand() % 40) > player[person].concentration) return;
                
                EnemyData2& target = enemy[battle[person][3]];
                if (target.poison) {
                    if (target.hp > 5000) target.hp -= 5000;
                    else {
                        target.hp = 0;
                        target.unconscious = true;
                        PlusExperience(person, battle[person][3]);
                    }
                } else target.poison = true;
                break;
            }
            case 10: {
                Print(7, p_name + "는 염력으로 적의 영혼을 분리 시키려 한다");
                if (esp_magic < 100) { Print(7, p_name + "는 아직 능력이 부족 했다"); return; }
                if (player[person].sp < 1000) { Print(7, "마법 지수가 부족했다"); return; }
                player[person].sp -= 1000;
                int j = battle[person][3];
                if ((std::rand() % 40) < enemy[j].resistance[1]) {
                    if (enemy[j].resistance[0] < 20) enemy[j].resistance[0] = 0; else enemy[j].resistance[0] -= 20;
                    if (enemy[j].resistance[1] < 20) enemy[j].resistance[1] = 0; else enemy[j].resistance[1] -= 20;
                    return;
                }
                if ((std::rand() % 80) > player[person].concentration) {
                    if (enemy[j].hp < 5000) {
                        enemy[j].hp = 0;
                        enemy[j].unconscious = true;
                        PlusExperience(person, j);
                    } else enemy[j].hp -= 5000;
                    return;
                }
                enemy[j].unconscious = true;
                break;
            }
        }
        DisplayEnemies(false);
    }
}

int r_seed(int seed) {
    return (std::rand() % (seed * 2 + 1)) - seed;
}

void Summon() {
    int summon_magic = player[person].raw_skills[5];
    Lore& p6 = player[6];
    
    switch (battle[person][2]) {
        case 1: {
            switch (std::rand() % 4) {
                case 0: SetPlayerName(6, "불의 정령"); break;
                case 1: SetPlayerName(6, "사라만다"); break;
                case 2: SetPlayerName(6, "아저"); break;
                case 3: SetPlayerName(6, "이프리트"); break;
            }
            p6.sex = male; p6._class = 0; p6.classtype = elemental;
            p6.level = summon_magic / 5;
            p6.strength = 10 + r_seed(5); p6.mentality = 10 + r_seed(5); p6.concentration = 10 + r_seed(5);
            p6.endurance = 10 + r_seed(5); p6.resistance = 10 + r_seed(5); p6.agility = 0;
            p6.accuracy = 10 + r_seed(5); p6.luck = 10 + r_seed(5);
            p6.poison = 0; p6.unconscious = 0; p6.dead = 0;
            p6.hp = p6.endurance * p6.level * 10; p6.sp = 0;
            p6.experience = 0; p6.potential_experience = 0;
            p6.weapon = 29; p6.wea_power = summon_magic * 3;
            p6.shield = 0; p6.shi_power = 0; p6.armor = 0; p6.ac = 0; p6.potential_ac = 0;
            std::memset(p6.raw_skills, 0, sizeof(p6.raw_skills));
            Display_Condition();
            break;
        }
        case 2: {
            switch (std::rand() % 9) {
                case 0: SetPlayerName(6, "님프"); break;
                case 1: SetPlayerName(6, "드리어드스"); break;
                case 2: SetPlayerName(6, "네레이드스"); break;
                case 3: SetPlayerName(6, "나이어드스"); break;
                case 4: SetPlayerName(6, "나파이어스"); break;
                case 5: SetPlayerName(6, "오레이어드스"); break;
                case 6: SetPlayerName(6, "알세이드스"); break;
                case 7: SetPlayerName(6, "마리드"); break;
                case 8: SetPlayerName(6, "켈피"); break;
            }
            p6.sex = female; p6._class = 0; p6.classtype = elemental;
            p6.level = summon_magic / 5;
            p6.strength = 10 + r_seed(5); p6.mentality = 10 + r_seed(5); p6.concentration = 10 + r_seed(5);
            p6.endurance = 8 + r_seed(5); p6.resistance = 12 + r_seed(5); p6.agility = 0;
            p6.accuracy = 10 + r_seed(5); p6.luck = 10 + r_seed(5);
            p6.poison = 0; p6.unconscious = 0; p6.dead = 0;
            p6.hp = p6.endurance * p6.level * 10; p6.sp = 0;
            p6.experience = 0; p6.potential_experience = 0;
            p6.weapon = 30; p6.wea_power = summon_magic * 2;
            p6.shield = 0; p6.shi_power = 0; p6.armor = 0; p6.ac = 1; p6.potential_ac = 1;
            std::memset(p6.raw_skills, 0, sizeof(p6.raw_skills));
            Display_Condition();
            break;
        }
        case 3: {
            switch (std::rand() % 5) {
                case 0: SetPlayerName(6, "공기의 정령"); break;
                case 1: SetPlayerName(6, "실프"); break;
                case 2: SetPlayerName(6, "실피드"); break;
                case 3: SetPlayerName(6, "디지니"); break;
                case 4: SetPlayerName(6, "인비저블 스토커"); break;
            }
            p6.sex = female; p6._class = 0; p6.classtype = elemental;
            p6.level = summon_magic / 5;
            p6.strength = 10 + r_seed(5); p6.mentality = 10 + r_seed(5); p6.concentration = 10 + r_seed(5);
            p6.endurance = 6 + r_seed(5); p6.resistance = 14 + r_seed(5); p6.agility = 0;
            p6.accuracy = 13 + r_seed(5); p6.luck = 10 + r_seed(5);
            p6.poison = 0; p6.unconscious = 0; p6.dead = 0;
            p6.hp = p6.endurance * p6.level * 10; p6.sp = 0;
            p6.experience = 0; p6.potential_experience = 0;
            p6.weapon = 31; p6.wea_power = summon_magic;
            p6.shield = 0; p6.shi_power = 0; p6.armor = 0; p6.ac = 1; p6.potential_ac = 1;
            std::memset(p6.raw_skills, 0, sizeof(p6.raw_skills));
            Display_Condition();
            break;
        }
        case 4: {
            switch (std::rand() % 3) {
                case 0: SetPlayerName(6, "땅의 정령"); break;
                case 1: SetPlayerName(6, "놈"); break;
                case 2: SetPlayerName(6, "다오"); break;
            }
            p6.sex = male; p6._class = 0; p6.classtype = elemental;
            p6.level = summon_magic / 5;
            p6.strength = 10 + r_seed(5); p6.mentality = 10 + r_seed(5); p6.concentration = 10 + r_seed(5);
            p6.endurance = 14 + r_seed(5); p6.resistance = 10 + r_seed(5); p6.agility = 0;
            p6.accuracy = 6 + r_seed(5); p6.luck = 10 + r_seed(5);
            p6.hp = p6.endurance * p6.level * 10; p6.weapon = 32; p6.wea_power = summon_magic * 4;
            p6.ac = 3; p6.potential_ac = 3;
            std::memset(p6.raw_skills, 0, sizeof(p6.raw_skills));
            Display_Condition();
            break;
        }
        case 5: {
            if (std::rand() % 2 == 0) {
                SetPlayerName(6, "에인션트 나이트");
                p6.sex = male; p6._class = 2; p6.classtype = sword;
                p6.strength = 12 + r_seed(5); p6.mentality = 6 + r_seed(5); p6.endurance = 15 + r_seed(5);
                p6.resistance = 10 + r_seed(5); p6.accuracy = 13 + r_seed(5); p6.sp = 0;
                p6.weapon = 6; p6.wea_power = 60 + r_seed(10);
                p6.shield = 2; p6.shi_power = 2; p6.armor = 3 + r_seed(1);
                p6.potential_ac = 2; p6.ac = p6.armor + p6.potential_ac;
                std::memset(p6.raw_skills, 0, sizeof(p6.raw_skills));
                p6.raw_skills[0] = summon_magic; // sword_skill
                p6.raw_skills[4] = summon_magic; // shield_skill
            } else {
                SetPlayerName(6, "에인션트 메이지");
                p6.sex = female; p6._class = 1; p6.classtype = magic;
                p6.strength = 7 + r_seed(5); p6.mentality = 12 + r_seed(5); p6.endurance = 7 + r_seed(5);
                p6.resistance = 10 + r_seed(5); p6.accuracy = 13 + r_seed(5);
                p6.weapon = 29; p6.wea_power = 2; p6.shield = 0; p6.shi_power = 0; p6.armor = 0; p6.potential_ac = 0; p6.ac = 0;
                p6.sp = p6.mentality * p6.level;
                std::memset(p6.raw_skills, 0, sizeof(p6.raw_skills));
            }
            p6.level = summon_magic / 5;
            p6.concentration = 10 + r_seed(5); p6.agility = 0; p6.luck = 10 + r_seed(5);
            p6.poison = 0; p6.unconscious = 0; p6.dead = 0;
            p6.hp = p6.endurance * p6.level * 10;
            Display_Condition();
            break;
        }
        case 6: {
            switch (std::rand() % 8) {
                case 0: SetPlayerName(6, "밴더스내치"); p6.endurance=15+r_seed(5); p6.resistance=8+r_seed(5); p6.accuracy=12+r_seed(5); p6.weapon=33; p6.wea_power=summon_magic*3; p6.potential_ac=3; p6.ac=3; break;
                case 1: SetPlayerName(6, "캐리온 크롤러"); p6.endurance=20+r_seed(5); p6.resistance=14+r_seed(5); p6.accuracy=13+r_seed(5); p6.weapon=34; p6.wea_power=summon_magic*1; p6.potential_ac=3; p6.ac=3; break;
                case 2: SetPlayerName(6, "켄타우루스"); p6.endurance=17+r_seed(5); p6.resistance=12+r_seed(5); p6.accuracy=18+r_seed(2); p6.weapon=35; p6.wea_power=std::round(summon_magic*1.5); p6.potential_ac=2; p6.ac=2; break;
                case 3: SetPlayerName(6, "데모고르곤"); p6.endurance=18+r_seed(5); p6.resistance=5+r_seed(5); p6.accuracy=17+r_seed(3); p6.weapon=36; p6.wea_power=summon_magic*4; p6.potential_ac=4; p6.ac=4; break;
                case 4: SetPlayerName(6, "듈라한"); p6.endurance=10+r_seed(5); p6.resistance=20; p6.accuracy=17; p6.weapon=16; p6.wea_power=summon_magic*1; p6.potential_ac=3; p6.ac=3; break;
                case 5: SetPlayerName(6, "에틴"); p6.endurance=10+r_seed(5); p6.resistance=10; p6.accuracy=10+r_seed(9); p6.weapon=8; p6.wea_power=std::round(summon_magic*0.8); p6.potential_ac=1; p6.ac=1; break;
                case 6: SetPlayerName(6, "헬하운드"); p6.endurance=14+r_seed(5); p6.resistance=9+r_seed(5); p6.accuracy=11+r_seed(5); p6.weapon=33; p6.wea_power=summon_magic*3; p6.potential_ac=2; p6.ac=2; break;
                case 7: SetPlayerName(6, "미노타우루스"); p6.endurance=13+r_seed(5); p6.resistance=11+r_seed(5); p6.accuracy=14+r_seed(5); p6.weapon=9; p6.wea_power=summon_magic*3; p6.potential_ac=2; p6.ac=2; break;
            }
            p6.sex = neutral; p6._class = 0; p6.classtype = unknown; p6.level = summon_magic / 5;
            p6.strength = 10+r_seed(5); p6.mentality = 10+r_seed(5); p6.concentration = 10+r_seed(5);
            p6.hp = p6.endurance * p6.level * 10;
            std::memset(p6.raw_skills, 0, sizeof(p6.raw_skills));
            Display_Condition();
            break;
        }
        case 7: {
            switch (std::rand() % 6) {
                case 0: SetPlayerName(6, "클라우드자이언트"); p6.endurance=20+r_seed(5); p6.resistance=15+r_seed(5); p6.accuracy=10+r_seed(5); p6.weapon=37; p6.wea_power=std::round(summon_magic*2.5); p6.potential_ac=2; p6.ac=2; break;
                case 1: SetPlayerName(6, "파이어 자이언트"); p6.endurance=25+r_seed(5); p6.resistance=5+r_seed(5); p6.accuracy=12+r_seed(5); p6.weapon=38; p6.wea_power=summon_magic*4; p6.potential_ac=2; p6.ac=2; break;
                case 2: SetPlayerName(6, "프로스트자이언트"); p6.endurance=30+r_seed(5); p6.resistance=8+r_seed(5); p6.accuracy=8+r_seed(2); p6.weapon=2; p6.wea_power=summon_magic*2; p6.potential_ac=2; p6.ac=2; break;
                case 3: SetPlayerName(6, "힐 자이언트"); p6.endurance=40+r_seed(5); p6.resistance=5+r_seed(5); p6.accuracy=7+r_seed(3); p6.weapon=39; p6.wea_power=std::round(summon_magic*1.5); p6.potential_ac=2; p6.ac=2; break;
                case 4: SetPlayerName(6, "스톤 자이언트"); p6.endurance=20+r_seed(5); p6.resistance=10+r_seed(5); p6.accuracy=11+r_seed(5); p6.weapon=37; p6.wea_power=std::round(summon_magic*2.5); p6.potential_ac=4; p6.ac=4; break;
                case 5: SetPlayerName(6, "스토엄 자이언트"); p6.endurance=20+r_seed(5); p6.resistance=10+r_seed(5); p6.accuracy=15+r_seed(9); p6.weapon=40; p6.wea_power=summon_magic*6; p6.potential_ac=1; p6.ac=1; break;
            }
            p6.sex = male; p6._class = 0; p6.classtype = giant; p6.level = summon_magic / 5;
            p6.strength = 10+r_seed(5); p6.hp = p6.endurance * p6.level * 10;
            std::memset(p6.raw_skills, 0, sizeof(p6.raw_skills));
            Display_Condition();
            break;
        }
        case 8: {
            switch (std::rand() % 4) {
                case 0: SetPlayerName(6, "글레이 고렘"); p6.endurance=30+r_seed(5); p6.resistance=15+r_seed(5); p6.accuracy=13+r_seed(5); p6.weapon=41; p6.wea_power=std::round(summon_magic*0.5); p6.potential_ac=3; p6.ac=3; break;
                case 1: SetPlayerName(6, "프레쉬 고렘"); p6.endurance=20+r_seed(5); p6.resistance=10+r_seed(5); p6.accuracy=12+r_seed(5); p6.weapon=0; p6.wea_power=summon_magic*1; p6.potential_ac=1; p6.ac=1; break;
                case 2: SetPlayerName(6, "아이언 고렘"); p6.endurance=20+r_seed(5); p6.resistance=5+r_seed(5); p6.accuracy=10+r_seed(2); p6.weapon=42; p6.wea_power=summon_magic*4; p6.potential_ac=5; p6.ac=5; break;
                case 3: SetPlayerName(6, "스톤 고렘"); p6.endurance=25+r_seed(5); p6.resistance=10+r_seed(5); p6.accuracy=13+r_seed(3); p6.weapon=0; p6.wea_power=summon_magic*2; p6.potential_ac=4; p6.ac=4; break;
            }
            p6.classtype = golem; p6.level = summon_magic / 5; p6.hp = p6.endurance * p6.level * 10;
            std::memset(p6.raw_skills, 0, sizeof(p6.raw_skills));
            Display_Condition();
            break;
        }
        case 9: {
            switch (std::rand() % 12) {
                case 0: SetPlayerName(6, "블랙 드래곤"); p6.weapon=43; break;
                case 1: SetPlayerName(6, "블루 드래곤"); p6.weapon=44; break;
                case 2: SetPlayerName(6, "블래스 드래곤"); p6.weapon=45; break;
                case 3: SetPlayerName(6, "브론즈 드래곤"); p6.weapon=44; break;
                case 4: SetPlayerName(6, "크로매틱 드래곤"); p6.weapon=46; break;
                case 5: SetPlayerName(6, "코퍼 드래곤"); p6.weapon=43; break;
                case 6: SetPlayerName(6, "골드 드래곤"); p6.weapon=46; break;
                case 7: SetPlayerName(6, "그린 드래곤"); p6.weapon=47; break;
                case 8: SetPlayerName(6, "플래티움 드래곤"); p6.weapon=48; break;
                case 9: SetPlayerName(6, "레드 드래곤"); p6.weapon=46; break;
                case 10: SetPlayerName(6, "실버 드래곤"); p6.weapon=49; break;
                case 11: SetPlayerName(6, "화이트 드래곤"); p6.weapon=49; break;
            }
            p6.endurance = 30+r_seed(10); p6.resistance = 10+r_seed(10); p6.accuracy = 15+r_seed(5);
            p6.wea_power = std::round(summon_magic * ((std::rand()%5)+1)); p6.potential_ac = 3; p6.ac = 3;
            p6.classtype = dragon; p6.level = summon_magic / 5; p6.hp = p6.endurance * p6.level * 10;
            std::memset(p6.raw_skills, 0, sizeof(p6.raw_skills));
            Display_Condition();
            break;
        }
        case 10: {
            if (std::rand() % 2 == 0) {
                SetPlayerName(6, "늑대 인간"); p6.classtype = unknown; p6.endurance=25; p6.resistance=15; p6.accuracy=18; p6.weapon=36; p6.wea_power=summon_magic*3; p6.potential_ac=2; p6.ac=2;
            } else {
                SetPlayerName(6, "드래곤 뉴트"); p6.classtype = dragon; p6.endurance=30; p6.resistance=18; p6.accuracy=19; p6.weapon=6; p6.wea_power=std::round(summon_magic*4.5); p6.potential_ac=4; p6.ac=4;
            }
            p6.level = summon_magic / 5; p6.hp = p6.endurance * p6.level * 10;
            std::memset(p6.raw_skills, 0, sizeof(p6.raw_skills));
            Display_Condition();
            break;
        }
        case 11: {
            int rnd = std::rand() % 3;
            if (rnd == 0) {
                SetPlayerName(6, "크리스탈 드래곤"); p6.classtype = dragon; p6.strength=25; p6.mentality=20; p6.concentration=20; p6.endurance=30; p6.resistance=20; p6.agility=0; p6.accuracy=20; p6.luck=20; p6.weapon=49; p6.wea_power=255; p6.potential_ac=4;
            } else {
                SetPlayerName(6, "크리스탈 고렘"); p6.classtype = golem; p6.strength=20; p6.mentality=0; p6.concentration=0; p6.endurance=40; p6.resistance=25; p6.agility=0; p6.accuracy=13; p6.luck=0; p6.weapon=0; p6.wea_power=150; p6.potential_ac=5;
            }
            p6.level = 30; p6.ac = p6.potential_ac; p6.hp = p6.endurance * p6.level * 10;
            std::memset(p6.raw_skills, 0, sizeof(p6.raw_skills));
            Display_Condition();
            break;
        }
    }
}

void Assist_Attack_One() {
    int k = battle[person][3];
    if ((enemy[k].unconscious && !Cruel) || enemy[k].dead) {
        if (!ExistEnemies()) return;
        battle[person][3]++;
        k++;
        if (battle[person][3] <= enemynumber) { Assist_Attack_One(); return; }
        return;
    }
    
    std::string s = ReturnMessage(person, battle[person][1], person, battle[person][3]);
    Print(15, s);
    std::string p_name = FromPas(player[person].name, player[person].name_len);
    
    if (enemy[k].unconscious && !enemy[k].dead) {
        switch(std::rand() % 4) {
            case 0: Print(12, p_name + "의 무기가 " + enemy[k].name + "의 심장을 꿰뚫었다"); break;
            case 1: Print(12, enemy[k].name + "의 머리는 " + p_name + "의 공격으로 산산 조각이 났다"); break;
            case 2: Print(12, "적의 피가 사방에 뿌려졌다"); break;
            case 3: Print(12, "적은 비명과 함께 찢겨 나갔다"); break;
        }
        if (SoundOn) scream2();
        PlusExperience(person, k);
        enemy[k].hp = 0; enemy[k].dead = true;
        return;
    }
    
    if ((std::rand() % 20) > player[person].accuracy) {
        Print(7, p_name + "의 공격은 빗나갔다 ...."); return;
    }
    
    long i_dmg = std::round(player[person].strength * player[person].wea_power * player[person].level / 2.0);
    i_dmg -= i_dmg * (std::rand() % 50) / 100;
    
    if ((std::rand() % 100) < enemy[k].resistance[0]) {
        Print(7, "적은 " + p_name + "의 공격을 저지했다"); return;
    }
    
    long j_ac = std::round(enemy[k].ac * enemy[k].level * ((std::rand() % 10) + 1));
    i_dmg -= j_ac;
    
    if (i_dmg <= 0) {
        Print(7, "그러나, 적은 " + p_name + "의 공격을 막았다"); return;
    }
    
    enemy[k].hp -= i_dmg;
    if (enemy[k].hp <= 0) {
        enemy[k].hp = 0;
        enemy[k].unconscious = false; enemy[k].dead = false;
        Print(12, "적은 " + p_name + "의 공격으로 의식불명이 되었다");
        if (SoundOn) hit();
        PlusExperience(person, k);
        enemy[k].unconscious = true;
    } else {
        cPrint(7, 15, "적은 ", std::to_string(i_dmg), "만큼의 피해를 입었다");
        if (SoundOn) hit();
    }
    DisplayEnemies(false);
}

void Assist_Attack_All() {
    battle[person][3] = 1;
    while (battle[person][3] <= enemynumber) {
        Assist_Attack_One();
        battle[person][3]++;
    }
}

void Assist_Attack() {
    person = 6;
    if (!exist(person)) return;
    switch (player[person].classtype) {
        case sword: Assist_Attack_One(); break;
        case magic: Assist_Attack_All(); break;
        case elemental: if (player[person].weapon == 29) Assist_Attack_One(); else Assist_Attack_All(); break;
        case unknown: Assist_Attack_One(); break;
        case giant: Assist_Attack_One(); break;
        case golem: Assist_Attack_All(); break;
        case dragon: Assist_Attack_One(); break;
    }
}

bool RunAway() {
    if ((std::rand() % 50) > player[person].agility) {
        Print(7, "그러나, 일행은 성공하지 못했다");
        return false;
    } else {
        Print(11, "성공적으로 도망을 갔다");
        return true;
    }
}

void WeaponAttack() {
    EnemyData2& e = enemy[person];
    if ((std::rand() % 20) >= e.accuracy[0]) {
        Print(7, e.name + ReturnSJosa(e.name) + " 빗맞추었다"); return;
    }
    
    int k_cnt = 0; for (int i = 1; i <= 6; i++) if (exist(i)) k_cnt++;
    if (k_cnt == 0) return;
    int h = (std::rand() % k_cnt) + 1;
    k_cnt = 0; int j = 0;
    for (int i = 1; i <= 6; i++) {
        if (exist(i)) { k_cnt++; if (k_cnt == h) j = i; }
    }
    if (j == 0) j = (std::rand() % 6) + 1;
    if (player[j].name_len == 0) j = (std::rand() % 5) + 1;
    
    long i_dmg = e.strength * e.level * ((std::rand() % 10) + 1) / 5;
    
    std::string p_name = FromPas(player[j].name, player[j].name_len);
    
    if (exist(j) && player[j].classtype == sword && player[j].shield > 0) {
        if ((std::rand() % 550) < player[j].raw_skills[4] * player[j].shi_power) {
            Print(13, e.name + ReturnSJosa(e.name) + " " + p_name + ReturnOJosa(p_name) + " 공격했다");
            Print(7, "그러나, " + p_name + ReturnSJosa(p_name) + " 방패로 적의 공격을 막았다");
            return;
        }
    }
    
    if (exist(j)) {
        i_dmg -= (player[j].ac * player[j].level * ((std::rand() % 10) + 1) / 10);
    }
    
    if (i_dmg <= 0) {
        Print(13, e.name + ReturnSJosa(e.name) + " " + p_name + ReturnOJosa(p_name) + " 공격했다");
        Print(7, "그러나, " + p_name + ReturnSJosa(p_name) + " 적의 공격을 방어했다");
        return;
    }
    
    if (player[j].dead > 0) player[j].dead += i_dmg;
    if (player[j].unconscious > 0 && player[j].dead == 0) player[j].unconscious += i_dmg;
    if (player[j].hp > 0) player[j].hp -= i_dmg;
    
    Print(13, p_name + ReturnSJosa(p_name) + " " + e.name + "에게 공격 받았다");
    cPrint(5, 13, p_name + ReturnSJosa(p_name) + " ", std::to_string(i_dmg), "만큼의 피해를 입었다");
}

void castattacksub(const std::string& magic_str, int power, int num) {
    if (player[num].name_len == 0) return;
    EnemyData2& e = enemy[person];
    std::string p_name = FromPas(player[num].name, player[num].name_len);
    
    if ((std::rand() % 20) >= e.accuracy[1]) {
        Print(7, e.name + "의 마법공격은 빗나갔다"); return;
    }
    
    if (exist(num)) {
        if ((std::rand() % 50) < player[num].resistance) {
            Print(7, "그러나, " + p_name + ReturnSJosa(p_name) + " 적의 마법을 저지했다"); return;
        }
    }
    
    power -= (std::rand() % (power / 2 + 1));
    
    if (exist(num)) {
        power -= (player[num].ac * player[num].level * ((std::rand() % 10) + 1) / 10);
    }
    
    if (power <= 0) {
        Print(7, "그러나, " + p_name + ReturnSJosa(p_name) + " 적의 마법을 막아냈다"); return;
    }
    
    if (player[num].dead > 0) player[num].dead += power;
    if (player[num].unconscious > 0 && player[num].dead == 0) player[num].unconscious += power;
    if (player[num].hp > 0) player[num].hp -= power;
    
    cPrint(5, 13, p_name + ReturnSJosa(p_name) + " ", std::to_string(power), "만큼의 피해를 입었다");
}

void castattackone(int num) {
    EnemyData2& e = enemy[person];
    std::string s_magic; int k_mult = 1;
    
    if (e.mentality <= 3) { s_magic = "충격"; k_mult = 1; }
    else if (e.mentality <= 8) { s_magic = "냉기"; k_mult = 2; }
    else if (e.mentality <= 10) { s_magic = "고통"; k_mult = 4; }
    else if (e.mentality <= 14) { s_magic = "혹한"; k_mult = 6; }
    else if (e.mentality <= 18) { s_magic = "화염"; k_mult = 7; }
    else { s_magic = "번개"; k_mult = 10; }
    
    int i_dmg = k_mult * e.level * 5;
    std::string p_name = FromPas(player[num].name, player[num].name_len);
    Print(13, e.name + ReturnSJosa(e.name) + " " + p_name + "에게 '" + s_magic + "'마법을 사용했다");
    castattacksub(s_magic, i_dmg, num);
}

void castattackall() {
    EnemyData2& e = enemy[person];
    std::string method; int k_mult = 1;
    
    if (e.mentality <= 6) { method = "열파"; k_mult = 1; }
    else if (e.mentality <= 12) { method = "에너지"; k_mult = 2; }
    else if (e.mentality <= 16) { method = "초음파"; k_mult = 3; }
    else if (e.mentality <= 20) { method = "혹한기"; k_mult = 5; }
    else { method = "화염폭풍"; k_mult = 8; }
    
    int i_dmg = k_mult * e.level * 5;
    Print(13, e.name + ReturnSJosa(e.name) + " 일행 모두에게 '" + method + "'마법을 사용했다");
    for (int j = 1; j <= 6; j++) castattacksub(method, i_dmg, j);
}

void enemycure(int num, int plus) {
    EnemyData2& e = enemy[person];
    EnemyData2& t = enemy[num];
    if (person == num) {
        Print(13, e.name + ReturnSJosa(e.name) + " 자신을 치료했다");
    } else {
        Print(13, e.name + ReturnSJosa(e.name) + " " + t.name + ReturnOJosa(t.name) + " 치료했다");
    }
    
    if (t.dead) t.dead = false;
    else if (t.unconscious) {
        t.unconscious = false;
        if (t.hp <= 0) t.hp = 1;
    } else {
        t.hp += plus;
        if (t.hp > t.endurance * t.level * 10) t.hp = t.endurance * t.level * 10;
    }
}

void castattack() {
    EnemyData2& e = enemy[person];
    int j = 0; int k_cnt = 0; int h = 0;
    long l1 = 0, l2 = 0;
    
    switch (e.castlevel) {
        case 1:
            k_cnt = (player[6].name_len == 0) ? 5 : 6;
            j = (std::rand() % k_cnt) + 1;
            if (!exist(j)) j = (std::rand() % k_cnt) + 1;
            castattackone(j);
            break;
        case 2:
            for (int i = 1; i <= 6; i++) if (exist(i)) k_cnt++;
            if (k_cnt == 0) return;
            h = (std::rand() % k_cnt) + 1;
            k_cnt = 0;
            for (int i = 1; i <= 6; i++) { if (exist(i)) { k_cnt++; if (k_cnt == h) j = i; } }
            if (j == 0) j = (std::rand() % 6) + 1;
            if (player[j].name_len == 0) j = (std::rand() % 5) + 1;
            castattackone(j);
            break;
        case 3:
            for (int i = 1; i <= 6; i++) if (exist(i)) k_cnt++;
            if (k_cnt > 0 && (std::rand() % k_cnt) < 2) {
                k_cnt = 0;
                for (int i = 1; i <= 6; i++) if (exist(i)) k_cnt++;
                h = (std::rand() % k_cnt) + 1;
                k_cnt = 0; j = 0;
                for (int i = 1; i <= 6; i++) { if (exist(i)) { k_cnt++; if (k_cnt == h) j = i; } }
                if (j == 0) j = (std::rand() % 6) + 1;
                if (player[j].name_len == 0) j = (std::rand() % 5) + 1;
                castattackone(j);
            } else castattackall();
            break;
        case 4:
            if ((e.hp < e.endurance * e.level * 3) && (std::rand() % 2 == 0)) {
                enemycure(person, e.level * e.mentality * 3);
            } else {
                k_cnt = 0;
                for (int i = 1; i <= 6; i++) if (exist(i)) k_cnt++;
                if (k_cnt > 0 && (std::rand() % k_cnt) < 2) {
                    k_cnt = 0;
                    for (int i = 1; i <= 6; i++) if (exist(i)) k_cnt++;
                    h = (std::rand() % k_cnt) + 1;
                    k_cnt = 0; j = 0;
                    for (int i = 1; i <= 6; i++) { if (exist(i)) { k_cnt++; if (k_cnt == h) j = i; } }
                    if (j == 0) j = (std::rand() % 6) + 1;
                    if (player[j].name_len == 0) j = (std::rand() % 5) + 1;
                    castattackone(j);
                } else castattackall();
            }
            break;
        case 5:
            if ((e.hp < e.endurance * e.level * 3) && (std::rand() % 3 == 0)) {
                enemycure(person, e.level * e.mentality * 3);
            } else {
                k_cnt = 0;
                for (int i = 1; i <= 6; i++) if (exist(i)) k_cnt++;
                if (k_cnt > 0 && (std::rand() % k_cnt) < 2) {
                    for (int i = 1; i <= enemynumber; i++) l1 += enemy[i].hp;
                    for (int i = 1; i <= enemynumber; i++) l2 += enemy[i].endurance * enemy[i].level * 10;
                    l2 /= 3;
                    if (enemynumber > 2 && l1 < l2 && (std::rand() % 2 == 0)) {
                        for (int i = 1; i <= enemynumber; i++) enemycure(i, e.level * e.mentality * 2);
                    } else {
                        j = 1;
                        for (int i = 6; i >= 1; i--) if (exist(i)) j = i;
                        for (int i = 2; i <= 6; i++) if (exist(i) && player[i].hp < player[j].hp) j = i;
                        castattackone(j);
                    }
                } else castattackall();
            }
            break;
        case 6:
            if ((e.hp < e.endurance * e.level * 4) && (std::rand() % 3 == 0)) {
                enemycure(person, e.level * e.mentality * 3); return;
            }
            j = 0; k_cnt = 0;
            for (int i = 1; i <= 6; i++) {
                if (player[i].name_len > 0) { j++; k_cnt += player[i].ac; }
            }
            if (j > 0) k_cnt /= j;
            if (k_cnt > 4 && (std::rand() % 5 == 0)) {
                for (int i = 1; i <= 6; i++) {
                    if (player[i].name_len > 0) {
                        std::string p_name = FromPas(player[i].name, player[i].name_len);
                        Print(13, e.name + "는 " + p_name + "의 갑옷파괴를 시도했다");
                        if (player[i].luck > (std::rand() % 21)) Print(7, "그러나, " + e.name + ReturnSJosa(e.name) + " 성공하지 못했다");
                        else {
                            Print(5, p_name + "의 갑옷은 파괴되었다");
                            if (player[i].ac > 0) player[i].ac--;
                        }
                    }
                }
                Display_Condition();
            } else {
                for (int i = 1; i <= enemynumber; i++) l1 += enemy[i].hp;
                for (int i = 1; i <= enemynumber; i++) l2 += enemy[i].endurance * enemy[i].level * 10;
                l2 /= 3;
                if (enemynumber > 2 && l1 < l2 && (std::rand() % 3 != 0)) {
                    for (int i = 1; i <= enemynumber; i++) enemycure(i, e.level * e.mentality * 2);
                } else {
                    k_cnt = 0;
                    for (int i = 1; i <= 6; i++) if (exist(i)) k_cnt++;
                    if (k_cnt > 0 && (std::rand() % k_cnt) < 2) {
                        j = 1;
                        for (int i = 6; i >= 1; i--) if (exist(i)) j = i;
                        for (int i = 2; i <= 6; i++) if (exist(i) && player[i].hp < player[j].hp) j = i;
                        castattackone(j);
                    } else castattackall();
                }
            }
            break;
    }
}

void specialattack() {
    EnemyData2& e = enemy[person];
    int k_cnt = 0, h = 0, j = 0;
    
    switch (e.special) {
        case 1: { // Poison
            k_cnt = 0;
            for (int i = 1; i <= 6; i++) if (player[i].poison == 0) k_cnt++;
            if (k_cnt == 0) return;
            h = (std::rand() % k_cnt) + 1;
            k_cnt = 0;
            for (int i = 1; i <= 6; i++) { if (player[i].poison == 0) { k_cnt++; if (k_cnt == h) j = i; } }
            if (j == 0) j = (std::rand() % 6) + 1;
            if (player[j].name_len == 0) j = (std::rand() % 5) + 1;
            
            std::string p_name = FromPas(player[j].name, player[j].name_len);
            Print(13, e.name + "는 " + p_name + "에게 독 공격을 시도했다");
            if ((std::rand() % 40) > e.agility) { Print(7, "독 공격은 실패했다"); return; }
            if ((std::rand() % 20) < player[j].luck) { Print(7, "그러나, " + p_name + ReturnSJosa(p_name) + " 독 공격을 피했다"); return; }
            Print(4, p_name + ReturnSJosa(p_name) + " 중독 되었다 !!");
            if (player[j].poison == 0) player[j].poison = 1;
            break;
        }
        case 2: { // Unconscious
            k_cnt = 0;
            for (int i = 1; i <= 6; i++) if (player[i].unconscious == 0) k_cnt++;
            if (k_cnt == 0) return;
            h = (std::rand() % k_cnt) + 1;
            k_cnt = 0;
            for (int i = 1; i <= 6; i++) { if (player[i].unconscious == 0) { k_cnt++; if (k_cnt == h) j = i; } }
            if (j == 0) j = (std::rand() % 6) + 1;
            if (player[j].name_len == 0) j = (std::rand() % 5) + 1;
            
            std::string p_name = FromPas(player[j].name, player[j].name_len);
            Print(13, e.name + "는 " + p_name + "에게 치명적 공격을 시도했다");
            if ((std::rand() % 50) > e.agility) { Print(7, "치명적 공격은 실패했다"); return; }
            if ((std::rand() % 20) < player[j].luck) { Print(7, "그러나, " + p_name + ReturnSJosa(p_name) + " 치명적 공격을 피했다"); return; }
            Print(4, p_name + ReturnSJosa(p_name) + " 의식불명이 되었다 !!");
            if (player[j].unconscious == 0) {
                player[j].unconscious = 1;
                if (player[j].hp > 0) player[j].hp = 0;
            }
            break;
        }
        case 3: { // Death
            k_cnt = 0;
            for (int i = 1; i <= 6; i++) if (player[i].dead == 0) k_cnt++;
            if (k_cnt == 0) return;
            h = (std::rand() % k_cnt) + 1;
            k_cnt = 0;
            for (int i = 1; i <= 6; i++) { if (player[i].dead == 0) { k_cnt++; if (k_cnt == h) j = i; } }
            if (j == 0) j = (std::rand() % 6) + 1;
            if (player[j].name_len == 0) j = (std::rand() % 5) + 1;
            
            std::string p_name = FromPas(player[j].name, player[j].name_len);
            Print(13, e.name + "는 " + p_name + "에게 죽음의 공격을 시도했다");
            if ((std::rand() % 60) > e.agility) { Print(7, "죽음의 공격은 실패했다"); return; }
            if ((std::rand() % 20) < player[j].luck) { Print(7, "그러나, " + p_name + ReturnSJosa(p_name) + " 죽음의 공격을 피했다"); return; }
            Print(4, p_name + ReturnSJosa(p_name) + " 죽었다 !!");
            if (player[j].dead == 0) {
                player[j].dead = 1;
                if (player[j].hp > 0) player[j].hp = 0;
            }
            break;
        }
    }
}

void SpecialCastAttack(byte& Exit_Code) {
    Exit_Code = 0;
    EnemyData2& e = enemy[person];
    if (e.E_number <= 1) return;
    
    if ((e.specialcastlevel & 0x80) > 0) {
        if (enemynumber < 8) {
            enemynumber = 8;
            for (int i = 1; i <= 8; i++) {
                if (i != person) {
                    enemy[i].name = e.name;
                    enemy[i].E_number = 0;
                    enemy[i].hp = 1;
                    enemy[i].aux_hp = 0;
                }
            }
            DisplayEnemies(true);
        }
        int j = (std::rand() % 8) + 1;
        enemy[j] = e; // Equivalent to Pascal's move
        for (int i = 1; i <= 8; i++) {
            if (i != j) {
                enemy[i].E_number = 0; enemy[i].hp = 1; enemy[i].poison = false;
                enemy[i].unconscious = false; enemy[i].dead = false;
                enemy[i].resistance[0] = 0; enemy[i].resistance[1] = 0;
                enemy[i].ac = 0; enemy[i].level = 0;
            }
        }
        DisplayEnemies(false);
    }
    
    int j = 0;
    for (int i = 0; i <= 6; i++) {
        if ((e.specialcastlevel & (1 << i)) > 0) j++;
    }
    if (j == 0) return;
    
    int method = 0, k_cnt_var = 0;
    if (j == 0) return;
    j = (std::rand() % j) + 1;
    do {
        if ((e.specialcastlevel & (1 << method)) > 0) k_cnt_var++;
        method++;
    } while (k_cnt_var < j);
    method--;
    
    switch (method) {
        case 0: {
            j = 0; int k_tgt = enemynumber;
            for (int i = enemynumber; i >= 1; i--) {
                if (!enemy[i].dead) j++; else k_tgt = i;
            }
            if (player[6].name_len > 0 && j < 8 && (std::rand() % 5 == 0)) {
                std::string s_name = FromPas(player[6].name, player[6].name_len);
                if (s_name.length() >= 3) {
                    std::string first_char = s_name.substr(0, 3); // UTF-8 한글은 3바이트
                    if (first_char == "크" || first_char == "대") return;
                }
                if (enemynumber < 8) { enemynumber++; k_tgt = enemynumber; }
                turn_mind(6, k_tgt);
                player[6].name_len = 0;
                Display_Condition(); DisplayEnemies(true);
                Print(13, e.name + ReturnSJosa(e.name) + " 독심술을 사용하여 " + enemy[k_tgt].name + ReturnSJosa(enemy[k_tgt].name) + " 자기편으로 끌어들였다");
            }
            break;
        }
        case 1: {
            j = 0; int ac_sum = 0;
            for (int i = 1; i <= 6; i++) {
                if (player[i].name_len > 0) { j++; ac_sum += player[i].ac; }
            }
            if (j > 0) ac_sum /= j;
            if (ac_sum > 4 && (std::rand() % 5 == 0)) {
                for (int i = 1; i <= 6; i++) {
                    if (player[i].name_len > 0) {
                        std::string p_name = FromPas(player[i].name, player[i].name_len);
                        Print(13, e.name + "는 " + p_name + "의 갑옷파괴를 시도했다");
                        if (player[i].luck > (std::rand() % 21)) Print(7, "그러나, " + e.name + ReturnSJosa(e.name) + " 성공하지 못했다");
                        else {
                            Print(5, p_name + "의 갑옷은 파괴되었다");
                            if (player[i].ac > 0) player[i].ac--;
                        }
                    }
                }
                Display_Condition();
            }
            break;
        }
        case 2: {
            j = 0; int k_tgt = enemynumber;
            for (int i = enemynumber; i >= 1; i--) {
                if (!enemy[i].dead) j++; else k_tgt = i;
            }
            if (j < (std::rand() % 3) + 2) {
                if (enemynumber < 8) { enemynumber++; k_tgt = enemynumber; }
                int e_idx = 0;
                switch (e.E_number) {
                    case 29: e_idx = 1 + (std::rand() % 5); break;
                    case 42: e_idx = 6 + (std::rand() % 5); break;
                    case 45: e_idx = 39 + (std::rand() % 2) * 2; break;
                    case 52: e_idx = 11 + (std::rand() % 5); break;
                    case 59: e_idx = 50; break;
                    case 73: case 74: e_idx = 71; break;
                    default: e_idx = 1 + (std::rand() % 15); break;
                }
                joinenemy(k_tgt, e_idx);
                DisplayEnemies(true);
                Print(13, e.name + ReturnSJosa(e.name) + " " + enemy[k_tgt].name + ReturnOJosa(enemy[k_tgt].name) + " 생성시켰다");
            }
            break;
        }
        case 3: case 4: case 5: {
            j = 0;
            for (int i = 1; i <= 6; i++) if (exist(i)) j++;
            if (j < (std::rand() % 3) + 5) return;
            for (int i = 1; i <= 6; i++) {
                if (player[i].name_len == 0) continue;
                std::string p_name = FromPas(player[i].name, player[i].name_len);
                if (method == 3) {
                    if ((std::rand() % 40) > player[i].luck) {
                        player[i].poison++;
                        if (player[i].poison == 0) player[i].poison = 255;
                        Print(13, p_name + ReturnSJosa(p_name) + " " + e.name + "에 의해 독에 감염 되었다.");
                    }
                } else if (method == 4) {
                    if (player[i].unconscious == 0 && (std::rand() % 30) > player[i].luck) {
                        player[i].unconscious = 1;
                        Print(13, p_name + ReturnSJosa(p_name) + " " + e.name + "에 의해 의식불명이 되었다.");
                    }
                } else if (method == 5) {
                    if (player[i].dead == 0 && (std::rand() % 22) > player[i].luck) {
                        player[i].dead = 1;
                        Print(13, p_name + ReturnSJosa(p_name) + " " + e.name + "에 의해 급사 당했다.");
                    }
                }
            }
            break;
        }
        case 6: {
            for (int i = 1; i <= enemynumber; i++) enemycure(i, e.level * e.mentality * 4);
            if (e.E_number == 5 || e.E_number == 10 || e.E_number == 47) Exit_Code = 1;
            break;
        }
    }
}

void EnemyAttack() {
    if (enemy[person].E_number == 0) return;
    if (enemy[person].specialcastlevel > 0) {
        byte b_code = 0;
        SpecialCastAttack(b_code);
        if (b_code > 0) return;
    }
    
    int i_ag = enemy[person].agility;
    if (i_ag > 20) i_ag = 20;
    
    if (enemy[person].special > 0 && (std::rand() % 50) < i_ag) {
        int j = 0;
        for (int i = 1; i <= enemynumber; i++) {
            if (!(enemy[i].unconscious || enemy[i].dead)) j++;
        }
        if (j > 3) {
            specialattack();
            return;
        }
    }
    
    if ((std::rand() % (enemy[person].accuracy[0] * 1000 + 1)) > (std::rand() % (enemy[person].accuracy[1] * 1000 + 1)) &&
        enemy[person].strength > 0) {
        WeaponAttack();
    } else {
        castattack();
    }
}

bool EndBattle(int& exitcode) {
    int i_cnt = 0, j_cnt = 0;
    for (int p = 1; p <= 6; p++) {
        if (player[p].name_len > 0) {
            i_cnt++;
            if (!exist(p)) j_cnt++;
        }
    }
    if (i_cnt == j_cnt) {
        exitcode = 1; // Lost in Battle
        return true;
    }
    
    i_cnt = 0; j_cnt = 0;
    for (int p = 1; p <= enemynumber; p++) {
        i_cnt++;
        if (enemy[p].dead || enemy[p].unconscious) j_cnt++;
    }
    if (i_cnt == j_cnt) {
        exitcode = 0; // Win in Battle
        return true;
    }
    return false;
}

void BattleMode(bool assault, bool auto_end) {
    party.etc[6] = 1; // Set combat flag
    // Pause(false); // Disable standard pause mechanism if implemented
    
    int exitcode = 0;
    bool battle_ok = false;
    
    while (!battle_ok) {
        if (!assault) {
            assault = true;
            goto loop;
        }
        
        for (person = 1; person <= 6; person++) battle[person][1] = 0;
        DisplayEnemies(false);
        autobattle = false;
        
        for (person = 1; person <= 5; person++) {
            if (exist(person)) {
                int k_sel = 0;
                if (!autobattle) {
                    std::string p_name = FromPas(player[person].name, player[person].name_len);
                    m[0] = p_name + "의 전투 모드 ===>";
                    m[1] = "한 명의 적을 " + ReturnWeapon(player[person].weapon) + Josa + "로 공격";
                    m[2] = "한 명의 적에게 마법 공격";
                    m[3] = "모든 적에게 마법 공격";
                    m[4] = "적에게 특수 마법 공격";
                    m[5] = "일행을 치료";
                    m[6] = "적에게 초능력 사용";
                    m[7] = "소환 마법 사용";
                    m[8] = "약초나 크리스탈을 사용";
                    m[9] = "도망을 시도함";
                    if (person == 1) m[9] = "일행에게 무조건 공격 할 것을 지시";
                    
                    k_sel = Select(80, 9, 9, true, false);
                    if (k_sel != 1) Clear();
                    m[1] = "없음";
                    
                    if (k_sel == 9 && person == 1) {
                        k_sel = 10;
                        autobattle = true;
                    }
                } else {
                    k_sel = 10;
                }
                
                battle[person][1] = k_sel;
                
                if (player[person].classtype == sword && (k_sel >= 2 && k_sel <= 7)) {
                    if (player[person]._class != 7 || k_sel != 5) {
                        k_sel = 1;
                        battle[person][1] = 1;
                    }
                }
                
                switch (k_sel) {
                    case 1: {
                        int j_tgt = SelectEnemy(); Clear();
                        if (j_tgt == 0) battle[person][1] = 0;
                        battle[person][2] = player[person].weapon;
                        battle[person][3] = j_tgt;
                        break;
                    }
                    case 2: {
                        for (int j = 1; j <= 10; j++) m[j] = ReturnMagic(0, j);
                        int max_val = player[person].raw_skills[0] / 10;
                        int i_sel = Select(60, max_val, 10, true, false);
                        int j_tgt = 0;
                        if (i_sel == 0) battle[person][1] = 0; else j_tgt = SelectEnemy();
                        Clear();
                        battle[person][2] = i_sel; battle[person][3] = j_tgt;
                        break;
                    }
                    case 3: {
                        for (int j = 1; j <= 10; j++) m[j] = ReturnMagic(1, j);
                        int max_val = player[person].raw_skills[0] / 10;
                        int i_sel = Select(60, max_val, 10, true, true);
                        if (i_sel == 0) battle[person][1] = 0;
                        battle[person][2] = i_sel; battle[person][3] = 0;
                        break;
                    }
                    case 4: {
                        for (int j = 1; j <= 10; j++) m[j] = ReturnMagic(4, j);
                        int max_val = player[person].raw_skills[3] / 10;
                        int i_sel = Select(60, max_val, 10, true, false);
                        int j_tgt = 1;
                        if (i_sel == 0) battle[person][1] = 0;
                        else if (i_sel >= 1 && i_sel <= 5) j_tgt = SelectEnemy();
                        Clear();
                        battle[person][2] = i_sel; battle[person][3] = j_tgt;
                        break;
                    }
                    case 5: CureSpell(); break; // Must be provided externally
                    case 6: {
                        for (int j = 1; j <= 5; j++) m[j] = ReturnMagic(5, j);
                        int i_sel = Select(74, 5, 5, true, false);
                        int j_tgt = 0;
                        if (i_sel == 0) battle[person][1] = 0;
                        else if (i_sel == 3) j_tgt = SelectEnemy();
                        else if (i_sel == 5) {
                            for (int idx = 1; idx <= 5; idx++) m[idx] = ReturnMagic(5, idx + 5);
                            int max_esp = 0;
                            if (player[person].raw_skills[4] > 19) max_esp = 1;
                            if (player[person].raw_skills[4] > 29) max_esp = 2;
                            if (player[person].raw_skills[4] > 79) max_esp = 3;
                            if (player[person].raw_skills[4] > 89) max_esp = 4;
                            if (player[person].raw_skills[4] > 99) max_esp = 5;
                            i_sel = Select(74, max_esp, 5, true, false) + 5;
                            if (i_sel == 5) battle[person][1] = 0;
                            else if (i_sel > 6) j_tgt = SelectEnemy();
                        }
                        Clear();
                        battle[person][2] = i_sel; battle[person][3] = j_tgt;
                        if (enemy[j_tgt].unconscious || enemy[j_tgt].dead) {
                            if (i_sel != 6) battle[person][1] = 0;
                        }
                        break;
                    }
                    case 7: {
                        for (int j = 1; j <= 10; j++) m[j] = ReturnMagic(6, j);
                        int max_val = player[person].raw_skills[5] / 10;
                        int i_sel = Select(60, max_val, 10, true, true);
                        if (i_sel == 0) battle[person][1] = 0;
                        battle[person][2] = i_sel; battle[person][3] = 1;
                        break;
                    }
					case 8: {
						bool has_crystal = false;
						for (int i = 1; i <= 7; i++) {
							if (party.crystal[i] > 0) has_crystal = true;
						}

						if (has_crystal) {
							m[0] = ""; m[1] = "약초나 약물을 사용한다"; m[2] = "크리스탈을 사용한다";
							switch (Select(100, 2, 2, true, true)) {
							    case 0: battle[person][1] = 0; break;
							    case 1: Use_Item(5); break;
							    case 2: {
								    m[0] = "당신이 사용할 크리스탈을 고르시오.";
								    int j_c = 0;
								    for (int i = 1; i <= 7; i++) {
									    if (party.crystal[i] > 0) {
										    j_c++;
										    m[j_c] = ReturnMagic(7, i);
									    }
								    }
								    int k_c = Select(80, j_c, j_c, false, true);
								    if (k_c == 0) {
									    battle[person][1] = 0;
								    }
								    else {
									    int i_c = 0; j_c = 0;
									    do {
										    i_c++;
										    if (party.crystal[i_c] > 0) j_c++;
									    } while (j_c != k_c && i_c <= 7);

									    switch (i_c) {
									    case 1:
										    battle[person][1] = 2; battle[person][2] = 11;
										    battle[person][3] = SelectEnemy();
										    party.crystal[1]--;
										    break;
									    case 2:
										    battle[person][1] = 3; battle[person][2] = 11;
										    battle[person][3] = 0;
										    party.crystal[2]--;
										    break;
									    case 3:
									    case 4: {
										    if (party.crystal[3] > 0 && party.crystal[4] > 0) {
											    int k_tgt = SelectEnemy();
											    if (enemy[k_tgt].name == "로드안") {
												    Clear();
												    std::string p_name = FromPas(player[person].name, player[person].name_len);
												    Print(7, p_name + ReturnSJosa(p_name) + " 로드안을 향해 다크 크리스탈과 에보니 크리스탈을 동시에 사용하였다.");
												    Print(7, "크리스탈에서 뿜어져 나온  검은 기운은  금새");
												    Print(7, "로드안에게 침투해 들어갔고 그는 순식간에 백");
												    Print(7, "여년을 늙어 버렸다");
												    EnemyData2& e_tgt = enemy[k_tgt];
												    e_tgt.strength /= 2; e_tgt.mentality /= 2; e_tgt.endurance /= 2;
												    e_tgt.resistance[0] /= 2; e_tgt.resistance[1] /= 2;
												    e_tgt.agility /= 2; e_tgt.accuracy[0] /= 2; e_tgt.accuracy[1] /= 2;
												    e_tgt.ac /= 2; e_tgt.level /= 2;
												    party.crystal[3]--; party.crystal[4]--;
											    }
											    else {
												    Clear();
												    Print(7, " 이 크리스탈은  선의 힘을 사용하는 적에게만");
												    Print(7, "반응한다.");
											    }
										    }
										    else {
											    Clear();
											    Print(7, " 크리스탈은 전혀 반응하지 않았다.");
										    }
										    PressAnyKey();
										    break;
									    }
									    case 5: {
										    int k_tgt = SelectEnemy();
										    if (enemy[k_tgt].hp > 0) {
											    Clear();
											    Print(7, " 당신이  영혼의 크리스탈을  적에게 사용하자");
											    Print(7, "적의 영혼은 크리스탈에 의해 저주를 받아  황");
											    Print(7, "폐화 되어졌다.");
											    enemy[k_tgt].aux_hp = (enemy[k_tgt].aux_hp + 1) / 2;
											    enemy[k_tgt].hp = enemy[k_tgt].hp / 2;
											    DisplayEnemies(false);
											    party.crystal[5]--;
										    }
										    else {
											    Clear();
											    Print(7, " 하지만 크리스탈은 전혀 반응을 보이지 않았다.");
										    }
										    PressAnyKey();
										    break;
									    }
									    case 6:
										    battle[person][1] = 7; battle[person][2] = 11;
										    battle[person][3] = 11;
										    party.crystal[6]--;
										    break;
									    case 7: {
										    for (int idx = 1; idx <= 6; idx++) {
											    if (player[idx].name_len > 0) {
												    if (player[idx].dead == 0) {
													    player[idx].unconscious = 0;
													    player[idx].hp = player[idx].endurance * player[idx].level * 10;
												    }
												    else {
													    player[idx].dead = 0;
													    player[idx].unconscious = 0;
													    player[idx].hp = 1;
												    }
											    }
										    }
										    Clear();
										    Print(7, " 에너지 크리스탈은 강한 에너지를  우리 대원");
										    Print(7, "들의 몸속으로  방출하였고  그 에너지를 취한");
										    Print(7, "대원들은 모두 원래의 기운을 되찾았다.");
										    Display_Condition();
										    PressAnyKey();
										    party.crystal[7]--;
										    break;
									    }
									    }
								    }
								    break;
							    }
							}
						}
						else {
							Use_Item(5);
						}
						break;
					}
                    case 10: {
                        if (player[person].classtype == sword) {
                            battle[person][1] = 1; battle[person][2] = player[person].weapon; battle[person][3] = 1;
                        } else if (player[person].classtype == magic) {
                            int atk_magic = player[person].raw_skills[0];
                            if (atk_magic > 9 && player[person].sp > atk_magic) {
                                battle[person][1] = 2; battle[person][2] = (atk_magic / 20) + 1; battle[person][3] = 1;
                            } else {
                                battle[person][1] = 1; battle[person][2] = player[person].weapon; battle[person][3] = 1;
                            }
                        }
                        break;
                    }
                }
            }
        }
        
        battle[6][1] = 1; battle[6][3] = 1;
        // Pause(false);
        
        for (person = 1; person <= 5; person++) {
            if (exist(person)) {
                std::string s_msg = ReturnMessage(person, battle[person][1], battle[person][2], battle[person][3]);
                if (battle[person][1] == 0 || battle[person][1] == 4 || battle[person][1] == 6 || battle[person][1] == 7 || battle[person][1] == 9) {
                    Print(15, s_msg);
                }
                
                int w_type = (player[person].weapon - 1 >= 0) ? (player[person].weapon - 1) / 7 : 0;
                if (w_type == 3) { // Bows
                    if (party.arrow > 0) party.arrow--;
                    else goto Skip_Attack;
                }
                
                switch (battle[person][1]) {
                    case 1: AttackOne(); break;
                    case 2: CastOne(); break;
                    case 3: CastAll(); break;
                    case 4: CastSpecial(); break;
                    case 6: BattleESP(); break;
                    case 7: Summon(); break;
                    case 9: {
                        if (RunAway()) {
                            party.etc[6] = 2; // Fled
                            Clear(); Scroll(true);
                            return;
                        }
                        break;
                    }
                }
            Skip_Attack:
                Print(7, "");
            }
        }
        
        Assist_Attack();
        DisplayEnemies(false);
        
        // Poison damage application for players
        for (person = 1; person <= 6; person++) {
            if (player[person].name_len > 0 && player[person].poison > 0) {
                if (player[person].dead > 0) {
                    if (player[person].dead < 10000) player[person].dead += player[person].poison;
                } else if (player[person].unconscious > 0) {
                    player[person].unconscious += player[person].poison;
                    if (player[person].unconscious > player[person].endurance * player[person].level * 10) player[person].dead = 1;
                } else {
                    player[person].hp -= player[person].poison;
                    if (player[person].hp <= 0) player[person].unconscious = 1;
                }
            }
        }
        Print(7, "");
        PressAnyKey();
        
    loop:
        for (person = 1; person <= enemynumber; person++) {
            EnemyData2& e = enemy[person];
            if (e.poison) {
                if (e.unconscious) e.dead = true;
                else {
                    e.hp -= 50;
                    if (e.hp <= 0) e.unconscious = true;
                }
            }
            if (e.aux_hp > 0) {
                e.unconscious = false; e.dead = false;
                while (e.aux_hp > 0 && e.hp < 30000) {
                    e.aux_hp--; e.hp += 1000;
                }
            }
            if (!(e.dead || e.unconscious)) EnemyAttack();
        }
        
        SimpleDisCond();
        battle_ok = EndBattle(exitcode);
        if (!battle_ok) {
            Print(7, "");
            PressAnyKey();
        }
        Clear();
    }
    
    switch (exitcode) {
        case 0:
            Clear();
            PlusGold();
            break;
        case 1:
            if (auto_end) {
                GameOver();
                // Pause(true);
                return;
            }
            break;
    }
    
    // Pause(true);
    party.etc[6] = 0;
    if (auto_end) Scroll(true);
}

void randomenemy(int& result_enemynumber) {
    byte range = 0, plus = 0;
    
    if (mapname == "Ground2" && !getbit(3)) { range = 3; plus = 18; }
    else if (mapname == "Ground3" && !getbit(4)) { range = 4; plus = 25; }
    else if (mapname == "Ground4" && !getbit(5)) { range = 3; plus = 35; }
    else if (mapname == "Ground5" && !getbit(6)) { range = 2; plus = 48; }
    else if (mapname == "UnderGrd") { range = 4; plus = 6; }
    else if (mapname == "TrolTown" && !getbit(4)) { range = 2; plus = 31; }
    else if (mapname == "OrcTown" && !getbit(3)) { range = 4; plus = 18; }
    else if (mapname == "Vesper" && !getbit(4)) { range = 3; plus = 26; }
    else if (mapname == "Kobold" && !getbit(5)) { range = 3; plus = 35; }
    else if (mapname == "DracTown" && !getbit(6)) { range = 2; plus = 48; }
    else { range = 0; plus = 0; }
    
    result_enemynumber = (range > 0) ? (std::rand() % range) + plus : 0;
}

void EncounterEnemy() {
    extern PositionType position;
    if (position == town) return;
    
    int max_e = party.etc[8];
    if (max_e <= 0) max_e = 5; // Fallback
    enemynumber = (std::rand() % max_e) + 1;
    if (enemynumber > 8) enemynumber = 8;
    
    for (int i = 1; i <= enemynumber; i++) {
        int e_idx = 0;
        randomenemy(e_idx);
        if (e_idx == 0) return;
        joinenemy(i, e_idx);
    }
    
    Clear();
    SetColor(13); // HPrintXY assumes x, y coordinates
    // OutHPrintXY is available from UHanX.h in MystSub context
    Print(13, "적이 출현했다 !!!");
    DisplayEnemies(true);
    
    int j = 0, h_val = 0;
    for (int i = 1; i <= enemynumber; i++) {
        j++; h_val += enemy[i].agility;
    }
    int avg_enemy_agi = (j > 0) ? h_val / j : 0;
    
    Print(11, "적의 평균 민첩성 : " + std::to_string(avg_enemy_agi));
    
    m[0] = "";
    m[1] = "적과 교전한다";
    m[2] = "도망간다";
    int k_sel = Select(130, 2, 2, false, true);
    Clear();
    
    bool assault = true;
    
    if (k_sel != 1) {
        j = 0; h_val = 0;
        for (int i = 1; i <= 6; i++) {
            if (player[i].name_len > 0) { j++; h_val += player[i].luck; }
        }
        int avg_player_luck = (j > 0) ? h_val / j : 0;
        
        j = 0; h_val = 0;
        for (int i = 1; i <= enemynumber; i++) {
            j++; h_val += enemy[i].agility;
        }
        avg_enemy_agi = (j > 0) ? h_val / j : 0;
        
        if (avg_player_luck > avg_enemy_agi) {
            Scroll(true);
            return;
        }
        assault = false;
    } else {
        j = 0; h_val = 0;
        for (int i = 1; i <= 6; i++) {
            if (player[i].name_len > 0) { j++; h_val += player[i].agility; }
        }
        int avg_player_agi = (j > 0) ? h_val / j : 0;
        
        j = 0; h_val = 0;
        for (int i = 1; i <= enemynumber; i++) {
            j++; h_val += enemy[i].agility;
        }
        avg_enemy_agi = (j > 0) ? h_val / j : 0;
        
        if (avg_player_agi > avg_enemy_agi) assault = true; else assault = false;
    }
    
    BattleMode(assault, true);
}

// --- END OF FILE MystBatt.cpp ---