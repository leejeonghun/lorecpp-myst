#include "mystmenu.h"
#include "mystsub.h"
#include "uhanx.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cstring>

void SPnotEnough() { 
    Print(7, "그러나, 마법 지수가 충분하지 않습니다."); 
    PressAnyKey(); 
}

void HealOne(byte whom) {
    Lore& p_whom = player[whom];
    if (p_whom.dead > 0 || p_whom.unconscious > 0 || p_whom.poison > 0) { 
        if (party.etc[6] == 0) Print(7, FromPas(p_whom.name, p_whom.name_len) + "는 치료될 상태가 아닙니다."); 
        return; 
    }
    if (p_whom.hp >= p_whom.endurance * p_whom.level * 10) { 
        if (party.etc[6] == 0) Print(7, FromPas(p_whom.name, p_whom.name_len) + "는 치료할 필요가 없습니다."); 
        return; 
    }
    int cost = 2 * player[person].level;
    if (player[person].sp < cost) { 
        if (party.etc[6] == 0) SPnotEnough(); 
        return; 
    }
    player[person].sp -= cost; 
    p_whom.hp = std::min((int)p_whom.level * p_whom.endurance * 10, (int)p_whom.hp + cost * 10);
    Print(15, FromPas(p_whom.name, p_whom.name_len) + "는 치료되어 졌습니다.");
}

void CureOne(byte whom) {
    Lore& p_whom = player[whom];
    if (p_whom.dead > 0 || p_whom.unconscious > 0) { 
        if (party.etc[6] == 0) Print(7, FromPas(p_whom.name, p_whom.name_len) + "는 독이 치료될 상태가 아닙니다."); 
        return; 
    }
    if (p_whom.poison == 0) { 
        if (party.etc[6] == 0) Print(7, FromPas(p_whom.name, p_whom.name_len) + "는 독에 걸리지 않았습니다."); 
        return; 
    }
    if (player[person].sp < 15) { 
        if (party.etc[6] == 0) SPnotEnough(); 
        return; 
    }
    player[person].sp -= 15; 
    p_whom.poison = 0; 
    Print(15, FromPas(p_whom.name, p_whom.name_len) + "의 독은 제거 되었습니다.");
}

void ConsciousOne(byte whom) {
    Lore& p_whom = player[whom];
    if (p_whom.dead > 0) { 
        if (party.etc[6] == 0) Print(7, FromPas(p_whom.name, p_whom.name_len) + "는 의식이 돌아올 상태가 아닙니다."); 
        return; 
    }
    if (p_whom.unconscious == 0) { 
        if (party.etc[6] == 0) Print(7, FromPas(p_whom.name, p_whom.name_len) + "는 의식불명이 아닙니다."); 
        return; 
    }
    int cost = 10 * p_whom.unconscious;
    if (player[person].sp < cost) { 
        if (party.etc[6] == 0) SPnotEnough(); 
        return; 
    }
    player[person].sp -= cost; 
    p_whom.unconscious = 0; 
    if (p_whom.hp <= 0) p_whom.hp = 1;
    Print(15, FromPas(p_whom.name, p_whom.name_len) + "는 의식을 되찾았습니다.");
}

void RevitalizeOne(byte whom) {
    Lore& p_whom = player[whom];
    if (p_whom.dead == 0) { 
        if (party.etc[6] == 0) Print(7, FromPas(p_whom.name, p_whom.name_len) + "는 아직 살아 있습니다."); 
        return; 
    }
    int cost = 30 * p_whom.dead;
    if (player[person].sp < cost) { 
        if (party.etc[6] == 0) SPnotEnough(); 
        return; 
    }
    player[person].sp -= cost; 
    p_whom.dead = 0; 
    p_whom.unconscious = std::max(1, (int)p_whom.endurance * p_whom.level);
    Print(15, FromPas(p_whom.name, p_whom.name_len) + "는 다시 생명을 얻었습니다.");
}

void HealAll() { for (int i = 1; i <= 6; i++) if (player[i].name_len > 0) HealOne(i); }
void CureAll() { for (int i = 1; i <= 6; i++) if (player[i].name_len > 0) CureOne(i); }
void ConsciousAll() { for (int i = 1; i <= 6; i++) if (player[i].name_len > 0) ConsciousOne(i); }
void RevitalizeAll() { for (int i = 1; i <= 6; i++) if (player[i].name_len > 0) RevitalizeOne(i); }

void CureSpell() {
    int power = (player[person].classtype == magic) ? (player[person].magic_data.cure_magic / 10) : (player[person].sword_data.axe_skill / 10);
    if (power <= 0) { 
        Print(7, "당신은 치료 마법을 사용할 능력이 없습니다."); 
        PressAnyKey(); return; 
    }
    
    m[0] = "누구에게"; 
    for (int i = 1; i <= 6; i++) {
        if (player[i].name_len > 0) m[i] = FromPas(player[i].name, player[i].name_len);
        else m[i] = "없습니다";
    }
    int j_val = (player[6].name_len > 0) ? 7 : 6;
    m[j_val] = "모든 사람들에게";

    int whom_sel = (power < 6) ? Select(60, j_val - 1, j_val - 1, true, true) : Select(60, j_val, j_val, true, true);
    if (whom_sel == 0) return;

    m[0] = "선택"; 
    if (whom_sel != j_val) {
        for (int i = 1; i <= 5; i++) m[i] = ReturnMagic(3, i);
        int limit = std::min(power, 5);
        int magic_sel = Select(60, limit, 5, false, true);
        if (magic_sel == 0) return;
        switch (magic_sel) {
            case 1: HealOne(whom_sel); break;
            case 2: CureOne(whom_sel); break;
            case 3: ConsciousOne(whom_sel); break;
            case 4: RevitalizeOne(whom_sel); break;
            case 5: RevitalizeOne(whom_sel); ConsciousOne(whom_sel); CureOne(whom_sel); HealOne(whom_sel); break;
        }
    } else {
        for (int i = 6; i <= 10; i++) m[i - 5] = ReturnMagic(3, i);
        int limit = power - 5;
        if (limit < 1) {
            Print(7, "강한 치료 마법은 아직 불가능 합니다.");
            PressAnyKey(); return;
        }
        int magic_sel = Select(60, std::min(limit, 5), 5, true, true);
        switch (magic_sel) {
            case 1: HealAll(); break;
            case 2: CureAll(); break;
            case 3: ConsciousAll(); break;
            case 4: RevitalizeAll(); break;
            case 5: RevitalizeAll(); ConsciousAll(); CureAll(); HealAll(); break;
        }
    }
    Print(7, ""); Print(7, "");
    SimpleDisCond();
    PressAnyKey();
}

void Wizard_Eye() {
    if (Map_Header.handicap && (Map_Header.handicap_bit & 64)) {
        Print(13, " 당신의 머리속에는 아무런 형상도 떠오르지 않았다.");
        return;
    }
    Clear();
    int x_init, y_init, x_end, y_end;
    if (Map_Header.xmax <= 100 && Map_Header.ymax <= 80) {
        x_init = 1; y_init = 1;
        x_end = Map_Header.xmax; y_end = Map_Header.ymax;
    } else {
        x_init = pas_x - 50; y_init = pas_y - 40;
        x_end = pas_x + 50; y_end = pas_y + 40;
        if (x_init < 1) { x_init = 1; x_end = std::min((int)Map_Header.xmax, 100); }
        if (y_init < 1) { y_init = 1; y_end = std::min((int)Map_Header.ymax, 80); }
        if (x_end > Map_Header.xmax) { x_end = Map_Header.xmax; x_init = std::max(1, (int)Map_Header.xmax - 100); }
        if (y_end > Map_Header.ymax) { y_end = Map_Header.ymax; y_init = std::max(1, (int)Map_Header.ymax - 80); }
    }

    for (int jy = y_init; jy <= y_end; jy++) {
        for (int ix = x_init; ix <= x_end; ix++) {
            byte rmap = Real_Map(ix, jy);
            int color = 5; // LightMagenta default
            if (position == town) {
                if ((rmap >= 1 && rmap <= 18) || rmap == 20 || rmap == 21) color = 15;
                else if (rmap == 22) color = 10;
                else if (rmap == 23) color = 11;
                else if (rmap == 24) color = 9;
                else if (rmap == 25) color = 3;
                else if (rmap == 26) color = 12;
                else if (rmap == 0 || rmap == 19 || (rmap >= 27 && rmap <= 47)) color = 0;
            } else if (position == ground) {
                if (rmap >= 2 && rmap <= 20) color = 15;
                else if (rmap == 22) color = 11;
                else if (rmap == 1 || rmap == 48) color = 9;
                else if (rmap == 23 || rmap == 49) color = 3;
                else if (rmap == 50) color = 12;
                else if (rmap == 0 || (rmap >= 24 && rmap <= 47)) color = 0;
                else color = 10;
            } else if (position == den || position == keep) {
                if ((rmap >= 1 && rmap <= 40) || rmap == 51) color = 15;
                else if (rmap == 54) color = 10;
                else if (rmap == 53) color = 11;
                else if (rmap == 48) color = 9;
                else if (rmap == 49) color = 3;
                else if (rmap == 50) color = 12;
                else if (rmap == 0 || rmap == 52 || (rmap >= 41 && rmap <= 47)) color = 0;
            }
            PutPixel((ix - x_init) * 2 + 32, (jy - y_init) * 2 + 305, color);
        }
    }
    
    // Blinking player position
    bool exit_loop = false;
    int blink_color = 8;
    SDL_Event ev;
    while (!exit_loop) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_KEYDOWN) { exit_loop = true; break; }
        }
        blink_color = (blink_color == 8) ? 14 : 8;
        PutPixel((pas_x - x_init) * 2 + 32, (pas_y - y_init) * 2 + 305, blink_color);
        UpdateScreen();
        SDL_Delay(150);
    }
}

bool Teleport() {
    if (Map_Header.handicap && (Map_Header.handicap_bit & 16)) {
        Print(13, "알 수 없는 힘이 공간 이동을 방해 합니다.");
        return false;
    }
    int x1 = 0, y1 = 0;
    switch (face % 4) {
        case 0: y1 = 1; break;
        case 1: y1 = -1; break;
        case 2: x1 = 1; break;
        case 3: x1 = -1; break;
    }
    
    Clear();
    Print(11, "당신의 공간 이동력을 지정");
    SetColor(15); HGotoXY(266, 70); HPrint("## ");
    SetColor(10); HPrint("5000");
    SetColor(15); HPrint(" 공간 이동력");
    
    int k_dist = 5;
    bool dist_ok = false;
    SDL_Event ev;
    
    while (!dist_ok) {
        char buf[16]; std::snprintf(buf, sizeof(buf), "%d", k_dist);
        SetColor(8); Bar(290, 70, 297, 86);
        SetColor(10); HPrintXY(290, 70, buf);
        UpdateScreen();
        
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_KEYDOWN) {
                int j_dir = 0;
                switch (ev.key.keysym.sym) {
                    case SDLK_UP: case SDLK_RIGHT: j_dir = 1; break;
                    case SDLK_DOWN: case SDLK_LEFT: j_dir = -1; break;
                    case SDLK_RETURN: case SDLK_SPACE: dist_ok = true; break;
                    case SDLK_ESCAPE: Clear(); return false;
                }
                if (j_dir != 0) {
                    k_dist += j_dir;
                    if (k_dist < 1) k_dist = 1;
                    if (k_dist > 9) k_dist = 9;
                }
            }
        }
        SDL_Delay(20);
    }

    pas_x += k_dist * x1; pas_y += k_dist * y1;
    if (pas_x < 5 || pas_x >= Map_Header.xmax - 3 || pas_y < 5 || pas_y >= Map_Header.ymax - 3) {
        pas_x -= k_dist * x1; pas_y -= k_dist * y1;
        message(7, "공간 이동이 통하지 않습니다.");
        return false;
    }

    bool tile_ok = false;
    byte rmap = Real_Map(pas_x, pas_y);
    if (position == town && rmap >= 27 && rmap <= 47) tile_ok = true;
    else if (position == ground && rmap >= 24 && rmap <= 47) tile_ok = true;
    else if (position == den && rmap >= 41 && rmap <= 47) tile_ok = true;
    else if (position == keep && rmap >= 27 && rmap <= 47) tile_ok = true;

    if (!tile_ok) {
        if (rmap == 0 || ((position == den || position == keep) && rmap == 52)) {
            message(13, "알수없는 힘이 당신을 배척합니다.");
            pas_x -= k_dist * x1; pas_y -= k_dist * y1;
            return true;
        } else {
            message(7, "공간 이동 장소로 부적합 합니다.");
            pas_x -= k_dist * x1; pas_y -= k_dist * y1;
            return false;
        }
    }

    pas_x -= k_dist * x1; pas_y -= k_dist * y1;
    for (int i = 1; i <= k_dist - 1; i++) {
        pas_x += x1; pas_y += y1;
        Scroll(false);
    }
    pas_x += x1; pas_y += y1;
    message(15, "공간 이동 마법이 성공했습니다.");
    Scroll(true);
    return true;
}

void PhenominaSpell() {
    if (player[person].classtype == sword) {
        std::string n_str = FromPas(player[person].name, player[person].name_len);
        message(7, n_str + ReturnSJosa(n_str) + " 변화 마법을 사용하는 계열이 아닙니다.");
        return;
    }
    
    int power = player[person].magic_data.pheno_magic / 10;
    m[0] = "선택";
    for (int i = 1; i <= 10; i++) m[i] = ReturnMagic(2, i);
    
    int magic_sel = Select(60, std::min(power, 10), 10, true, true);
    if (magic_sel == 0) return;
    
    int x1 = 0, y1 = 0;
    switch (face % 4) {
        case 0: y1 = 1; break; case 1: y1 = -1; break; case 2: x1 = 1; break; case 3: x1 = -1; break;
    }

    switch (magic_sel) {
        case 1:
            if (player[person].sp < 1) { SPnotEnough(); return; }
            party.etc[1] = std::min(255, (int)party.etc[1] + power);
            message(15, "일행은 마법의 횃불을 밝혔습니다.");
            player[person].sp -= 1; Scroll(true);
            break;
        case 2:
            if (!Map_Header.handicap || !(Map_Header.handicap_bit & 64)) {
                if (player[person].sp < 5) { SPnotEnough(); return; }
                player[person].sp -= 5;
            }
            Wizard_Eye();
            break;
        case 3:
            if (player[person].sp < 5) { SPnotEnough(); return; }
            message(15, "일행은 공중부상중 입니다.");
            party.etc[4] = 255; player[person].sp -= 5;
            break;
        case 4:
            if (player[person].sp < 10) { SPnotEnough(); return; }
            message(15, "일행은 물위를 걸을수 있습니다.");
            party.etc[2] = 255; player[person].sp -= 10;
            break;
        case 5:
            if (player[person].sp < 20) { SPnotEnough(); return; }
            message(15, "일행은 늪위를 걸을수 있습니다.");
            party.etc[3] = 255; player[person].sp -= 20;
            break;
        case 6: {
            if (Map_Header.handicap && (Map_Header.handicap_bit & 8)) {
                Print(13, "알 수 없는 힘이 기화 이동을 방해 합니다."); return;
            }
            if (player[person].sp < 25) { SPnotEnough(); return; }
            pas_x += 2 * x1; pas_y += 2 * y1;
            if (pas_x < 5 || pas_x >= Map_Header.xmax - 3 || pas_y < 5 || pas_y >= Map_Header.ymax - 3) {
                pas_x -= 2 * x1; pas_y -= 2 * y1; return;
            }
            bool ok = false;
            byte rmap = Real_Map(pas_x, pas_y);
            if (position == town && (rmap == 0 || (rmap >= 27 && rmap <= 47))) ok = true;
            else if (position == ground && (rmap == 0 || (rmap >= 24 && rmap <= 47))) ok = true;
            else if (position == den && (rmap == 0 || (rmap >= 41 && rmap <= 47))) ok = true;
            else if (position == keep && (rmap == 0 || (rmap >= 40 && rmap <= 47))) ok = true;
            
            if (!ok) {
                pas_x -= 2 * x1; pas_y -= 2 * y1;
                message(7, "기화 이동이 통하지 않습니다."); return;
            }
            player[person].sp -= 25;
            byte rmap_prev = Real_Map(pas_x - x1, pas_y - y1);
            if (rmap_prev == 0 || ((position == den || position == keep) && Real_Map(pas_x + x1, pas_y + y1) == 52)) {
                pas_x -= 2 * x1; pas_y -= 2 * y1;
                message(13, "알수없는 힘이 당신의 마법을 배척합니다.");
            } else {
                pas_x -= x1; pas_y -= y1; Scroll(false);
                pas_x += x1; pas_y += y1; message(15, "기화 이동을 마쳤습니다."); Scroll(true);
            }
            break;
        }
        case 7: {
            if (Map_Header.handicap && (Map_Header.handicap_bit & 2)) {
                Print(13, "이 지역의 악의 힘이 지형변화를 방해 합니다."); return;
            }
            if (player[person].sp < 30) { SPnotEnough(); return; }
            int target_val = (position == town) ? 47 : (position == ground ? 41 : 43);
            player[person].sp -= 30;
            byte rt = Real_Map(pas_x + x1, pas_y + y1);
            if (rt == 0 || ((position == den || position == keep) && rt == 52)) {
                message(13, "알수없는 힘이 당신의 마법을 배척합니다.");
            } else if (position == keep && rt >= 42) {}
            else if (position == town && rt >= 48) {}
            else {
                PutMap(pas_x + x1, pas_y + y1, target_val);
                message(15, "지형변화에 성공했습니다."); Scroll(true);
            }
            break;
        }
        case 8:
            if (!Map_Header.handicap || !(Map_Header.handicap_bit & 16)) {
                if (player[person].sp < 50) { SPnotEnough(); return; }
            }
            if (Teleport()) player[person].sp -= 50;
            break;
        case 9: {
            if (player[person].sp < 30) { SPnotEnough(); return; }
            int alive = 0; for (int i = 1; i <= 6; i++) if (player[i].name_len > 0) alive++;
            party.food = std::min(255, (int)party.food + alive);
            player[person].sp -= 30;
            hany = 30; Print(15, " 식량 제조 마법은 성공적으로 수행되었습니다");
            hany += 20; Print(15, "            " + std::to_string(alive) + " 개의 식량이 증가됨");
            hany += 30; Print(11, "      일행의 현재 식량은 " + std::to_string(party.food) + " 개 입니다");
            break;
        }
        case 10: {
            if (Map_Header.handicap && (Map_Header.handicap_bit & 4)) {
                Print(13, "이 지역의 악의 힘이 지형변화를 방해 합니다."); return;
            }
            if (player[person].sp < 60) { SPnotEnough(); return; }
            int target_val = (position == town) ? 47 : (position == ground ? 41 : 43);
            player[person].sp -= 60;
            int limit = (x1 == 0) ? 5 : 4;
            int step = 1;
            while (step <= limit) {
                byte rt = Real_Map(pas_x + x1 * step, pas_y + y1 * step);
                if (rt == 0 || ((position == den || position == keep) && rt == 52)) {
                    message(13, "알수없는 힘이 당신의 마법을 배척합니다."); break;
                } else if (position == keep && rt >= 42) { break; }
                else if (position == town && rt >= 48) { break; }
                else { PutMap(pas_x + x1 * step, pas_y + y1 * step, target_val); }
                step++;
            }
            if (step > limit) message(15, "지형변화에 성공했습니다.");
            Silent_Scroll();
            break;
        }
    }
    DisplaySP();
}

void Inventory() {
    Clear();
    SetColor(7);
    int Y_Pos = 30, X_Pos = 270;
    HPrintXY4Select(X_Pos, Y_Pos + 0 * 15, ("체력 회복약 : " + std::to_string(party.item[0])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 1 * 15, ("마법 회복약 : " + std::to_string(party.item[1])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 2 * 15, ("해독의 약초 : " + std::to_string(party.item[2])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 3 * 15, ("의식의 약초 : " + std::to_string(party.item[3])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 4 * 15, ("부활의 약초 : " + std::to_string(party.item[4])).c_str());

    X_Pos = 430;
    HPrintXY4Select(X_Pos, Y_Pos + 0 * 15, ("소환 문서 : " + std::to_string(party.item[5])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 1 * 15, ("대형 횃불 : " + std::to_string(party.item[6])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 2 * 15, ("수정 구슬 : " + std::to_string(party.item[7])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 3 * 15, ("비행 부츠 : " + std::to_string(party.item[8])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 4 * 15, ("이동 구슬 : " + std::to_string(party.item[9])).c_str());

    Y_Pos = 120; X_Pos = 270;
    HPrintXY4Select(X_Pos, Y_Pos + 0 * 15, ("화염의크리스탈 : " + std::to_string(party.crystal[0])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 1 * 15, ("한파의크리스탈 : " + std::to_string(party.crystal[1])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 2 * 15, ("다크  크리스탈 : " + std::to_string(party.crystal[2])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 3 * 15, ("에보니크리스탈 : " + std::to_string(party.crystal[3])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 4 * 15, ("영혼의크리스탈 : " + std::to_string(party.crystal[4])).c_str());

    X_Pos = 430;
    HPrintXY4Select(X_Pos, Y_Pos + 0 * 15, ("소환의크리스탈 : " + std::to_string(party.crystal[5])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 1 * 15, ("에너지크리스탈 : " + std::to_string(party.crystal[6])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 2 * 15, ("크로매틱크리스탈 : " + std::to_string(party.crystal[7])).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 3 * 15, ("크 리 스 탈 볼 : " + std::to_string(party.crystal[8])).c_str());

    PressAnyKey();
}

void ViewParty() {
    Clear();
    SetColor(7);
    int X_Pos = 270;
    HPrintXY(X_Pos, 45, ("X 축 = " + std::to_string(pas_x)).c_str());
    HPrintXY(X_Pos, 60, ("Y 축 = " + std::to_string(pas_y)).c_str());
    HPrintXY(X_Pos, 80, ("남은 식량 = " + std::to_string(party.food)).c_str());
    HPrintXY(X_Pos, 95, ("남은 황금 = " + std::to_string(party.gold)).c_str());
    HPrintXY(X_Pos, 115, ("남은 화살 = " + std::to_string(party.arrow)).c_str());
    
    X_Pos = 430;
    HPrintXY4Select(X_Pos, 80, ("마법의 횃불 :" + std::string(party.etc[1] > 0 ? " 가능" : " 불가")).c_str());
    HPrintXY4Select(X_Pos, 95, ("공중 부상   :" + std::string(party.etc[4] > 0 ? " 가능" : " 불가")).c_str());
    HPrintXY4Select(X_Pos, 115, ("물위를 걸음 :" + std::string(party.etc[2] > 0 ? " 가능" : " 불가")).c_str());
    HPrintXY4Select(X_Pos, 130, ("늪위를 걸음 :" + std::string(party.etc[3] > 0 ? " 가능" : " 불가")).c_str());

    int Y_Pos = 160; X_Pos = 270;
    HPrintXY4Select(X_Pos, Y_Pos + 0 * 15, ("오 크  글 :" + std::string(getbit(8) ? " 가능" : " 불가")).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 1 * 15, ("트 롤  글 :" + std::string(getbit(10) ? " 가능" : " 불가")).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 2 * 15, ("코볼트 글 :" + std::string(getbit(12) ? " 가능" : " 불가")).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 3 * 15, ("드라콘 글 :" + std::string(getbit(14) ? " 가능" : " 불가")).c_str());

    X_Pos = 430;
    HPrintXY4Select(X_Pos, Y_Pos + 0 * 15, ("오 크  말 :" + std::string(getbit(7) ? " 가능" : " 불가")).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 1 * 15, ("트 롤  말 :" + std::string(getbit(9) ? " 가능" : " 불가")).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 2 * 15, ("코볼트 말 :" + std::string(getbit(11) ? " 가능" : " 불가")).c_str());
    HPrintXY4Select(X_Pos, Y_Pos + 3 * 15, ("드라콘 말 :" + std::string(getbit(13) ? " 가능" : " 불가")).c_str());

    X_Pos = 270;
    char datebuf[64];
    std::snprintf(datebuf, sizeof(datebuf), "%d 년 %d 월 %d 일", party.year, (party.day / 30) + 1, (party.day % 30) + 1);
    HPrintXY4Select(X_Pos, 240, datebuf);
    std::snprintf(datebuf, sizeof(datebuf), "%d 시 %d 분", party.hour, party.min);
    HPrintXY4Select(X_Pos, 255, datebuf);

    PressAnyKey();
}

void ViewCharacter() {
    Clear(); Print(15, "능력을 보고싶은 인물을 선택하시오");
    int i_idx = ChooseWhom(false); Clear(); if (i_idx == 0) return;
    Lore& p = player[i_idx];
    const int X_Pos = 260;
    SetColor(11);
    HPrintXY4Select(X_Pos, 30, ("# 이름 : " + FromPas(p.name, p.name_len)).c_str());
    HPrintXY4Select(X_Pos, 46, ("# 성별 : " + ReturnSex(i_idx)).c_str());
    HPrintXY4Select(X_Pos, 62, ("# 계급 : " + ReturnClass(i_idx)).c_str());
    
    SetColor(3); char buf[64];
    std::snprintf(buf, sizeof(buf), "체  력 : %d", p.strength); HPrintXY4Select(X_Pos+4, 88, buf);
    std::snprintf(buf, sizeof(buf), "정신력 : %d", p.mentality); HPrintXY4Select(X_Pos+4, 104, buf);
    std::snprintf(buf, sizeof(buf), "집중력 : %d", p.concentration); HPrintXY4Select(X_Pos+4, 120, buf);
    std::snprintf(buf, sizeof(buf), "인내력 : %d", p.endurance); HPrintXY4Select(X_Pos+4, 136, buf);
    std::snprintf(buf, sizeof(buf), "저항력 : %d", p.resistance); HPrintXY4Select(X_Pos+4, 152, buf);
    std::snprintf(buf, sizeof(buf), "민첩성 : %d", p.agility); HPrintXY4Select(X_Pos+4, 168, buf);
    std::snprintf(buf, sizeof(buf), "정확성 : %d", p.accuracy); HPrintXY4Select(X_Pos+4, 184, buf);
    std::snprintf(buf, sizeof(buf), "행운   : %d", p.luck); HPrintXY4Select(X_Pos+4, 200, buf);
    
    SetColor(2); int j_col = X_Pos + 150;
    if (p.classtype == sword) {
        std::snprintf(buf, sizeof(buf), "베는 무기  기술치  : %d", p.sword_data.sword_skill); HPrintXY4Select(j_col, 88, buf);
        std::snprintf(buf, sizeof(buf), p._class != 7 ? "찍는 무기  기술치  : %d" : "치료 마법  능력치  : %d", p.sword_data.axe_skill); HPrintXY4Select(j_col, 104, buf);
        std::snprintf(buf, sizeof(buf), "찌르는 무기 기술치 : %d", p.sword_data.spear_skill); HPrintXY4Select(j_col, 120, buf);
        std::snprintf(buf, sizeof(buf), "쏘는 무기  기술치  : %d", p.sword_data.bow_skill); HPrintXY4Select(j_col, 136, buf);
        std::snprintf(buf, sizeof(buf), "방패 사용  기술치  : %d", p.sword_data.shield_skill); HPrintXY4Select(j_col, 152, buf);
        std::snprintf(buf, sizeof(buf), "맨손 사용  기술치  : %d", p.sword_data.fist_skill); HPrintXY4Select(j_col, 168, buf);
    } else if (p.classtype == magic) {
        std::snprintf(buf, sizeof(buf), "공격 마법 능력치 : %d", p.magic_data.attack_magic); HPrintXY4Select(j_col, 88, buf);
        std::snprintf(buf, sizeof(buf), "변화 마법 능력치 : %d", p.magic_data.pheno_magic); HPrintXY4Select(j_col, 104, buf);
        std::snprintf(buf, sizeof(buf), "치료 마법 능력치 : %d", p.magic_data.cure_magic); HPrintXY4Select(j_col, 120, buf);
        std::snprintf(buf, sizeof(buf), "특수 마법 능력치 : %d", p.magic_data.special_magic); HPrintXY4Select(j_col, 136, buf);
        std::snprintf(buf, sizeof(buf), "초 자연력 능력치 : %d", p.magic_data.esp_magic); HPrintXY4Select(j_col, 152, buf);
        std::snprintf(buf, sizeof(buf), "소환 마법 능력치 : %d", p.magic_data.summon_magic); HPrintXY4Select(j_col, 168, buf);
    }
    
    SetColor(3);
    std::snprintf(buf, sizeof(buf), "## 경험치 : %d", (int)p.experience); HPrintXY4Select(X_Pos+4, 216, buf);
    std::snprintf(buf, sizeof(buf), "## 레벨 : %d", p.level); HPrintXY4Select(X_Pos+220, 216, buf);
    
    SetColor(2);
    HPrintXY4Select(X_Pos+4, 240, ("사용 무기 - " + ReturnWeapon(p.weapon)).c_str());
    if (p.shield != 0) HPrintXY4Select(X_Pos+4, 256, ("방패 - " + ReturnShield(p.shield)).c_str());
    if (p.armor != 0) HPrintXY4Select(X_Pos+190, 256, ("갑옷 - " + ReturnArmor(p.armor)).c_str());
    PressAnyKey();
}

void QuickView() {
    const int X_Pos = 270;
    Clear();
    SetColor(15); HPrintXY(X_Pos+30, 40, "이름");
    SetColor(12); HPrintXY(X_Pos+150, 40, " 중독 의식불명 죽음");
    SetColor(7);
    for (int j_idx = 1; j_idx <= 6; j_idx++) {
        if (player[j_idx].name_len > 0) {
            HPrintXY4Select(X_Pos, 60+j_idx*15, FromPas(player[j_idx].name, player[j_idx].name_len).c_str());
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%d", player[j_idx].poison); HPrintXY4Select(X_Pos+174, 60+j_idx*15, buf);
            std::snprintf(buf, sizeof(buf), "%3d", player[j_idx].unconscious); HPrintXY4Select(X_Pos+214, 60+j_idx*15, buf);
            std::snprintf(buf, sizeof(buf), "%5d", player[j_idx].dead); HPrintXY4Select(X_Pos+250, 60+j_idx*15, buf);
        }
    }
    PressAnyKey();
}

void CastSpell() {
    person = ChooseWhom(false); if (person == 0) return;
    if (!exist(person)) { 
        ReturnSexData(); message(7, sexdata + "는 마법을 사용할수있는 상태가 아닙니다"); return; 
    }
    if (player[person].classtype == sword && player[person]._class != 7) { 
        std::string name_str = FromPas(player[person].name, player[person].name_len);
        message(7, name_str + ReturnSJosa(name_str) + " 마법을 사용할 수 없는 계열입니다."); return; 
    }
    
    m[0] = "사용할 마법의 종류 ===>"; m[1] = "치료 마법"; m[2] = "변화 마법";
    int k_sel = Select(80, 2, 2, true, true);
    if (k_sel == 1) CureSpell(); 
    else if (k_sel == 2) PhenominaSpell();
}

int ReturnPredict() {
    int data = 0;
    switch (party.etc[20]) {
        case 0: data = getbit(0) ? 2 : 1; break;
        case 1: case 2: data = 3; break;
        case 3:
            switch(party.etc[21]) {
                case 0: data = (mapname != "Gaea") ? 4 : 5; break;
                case 1: data = 6; break;
                case 2: data = 7; break;
                case 3:
                    if(getbit(4)) data = 11;
                    else if(getbit(1)) data = 10;
                    else if(mapname == "Ground3" || mapname == "TrolTown" || mapname == "Vesper") data = 9;
                    else data = 8;
                    break;
            }
            break;
        case 4: case 5: case 6:
            if(getbit(6)) data = 18;
            else if(mapname == "Ground5") data = 17;
            else if(getbit(5)) data = (mapname == "Ground2") ? 16 : 15;
            else if(mapname == "Ground4") data = 14;
            else if(mapname == "Gaea") data = 13;
            else if(mapname == "Ground2" || mapname == "Valiant" || mapname == "OccTown") data = 7;
            else if(party.etc[20] == 5) data = 3;
            else if(party.etc[20] == 4) data = 11;
            else data = 12;
            break;
        case 7: data = 19; break;
        case 8: data = 20; break;
    }

    if (party.etc[25] == 1) {
        switch(party.etc[20]) {
            case 9: data = 19; break;
            case 10: data = party.etc[26] + 21; break;
            case 11: data = 11; break;
        }
    } else if (party.etc[25] == 2) {
        if (party.year >= 795) data = 30;
        else if (getbit(49)) data = 29;
        else if (getbit(48)) data = 28;
        else if (getbit(32)) data = 27;
        else if (mapname == "UnderGrd") data = 26;
        else data = 25;
    }
    return data;
}

void Extrasense() {
    const std::string Predict_Data[30] = {
        "메너스 광산을 나갈", "로드안에게 불려갈", "로드안과 계속 이야기 할", "가이아테라로 갈",
        "가이아 테라성의 성주에게 부탁을 받을", "오크 마을을 점령 할", "가이아 테라성의 성주를 다시 만날",
        "아프로디테 테라로 갈", "대륙의 중앙에 있는 베스퍼성을 점령 할", "베스퍼성 북동쪽의 트롤 마을을 점령 할",
        "로드안에게 돌아갈", "가이아 테라로 다시 갈", "가이아 테라성 북쪽에 있는 게이트를 통해 이쉬도 테라로 갈",
        "대륙 뷱동쪽에 있는 코볼트 마을을 점령 할", "가이아 테라로 돌아갈", "배리언트 피플즈의 성주를 만나 우라누스 테라로 갈",
        "드라코니안성을 점령 할", "대륙의 서쪽에 있는 임페리엄 마이너성에서 에인션트 이블을 만날", "다시 로드안을 만날",
        "배리언트 피플즈의 반란을 진압하려 갈", "가이아 테라에 있는 오크의 원혼을 봉인 할", "아프로디테 테라에 있는 트롤의 원혼을 봉인 할",
        "이쉬도 테라에 있는 코볼트의 원혼을 봉인 할", "로어 대륙에 있는 드라코니안의 원혼을 봉인 할", "하데스 게이트를 통해서 하데스 테라로 갈",
        "투시를 통해서 어떤 장소를 발견 할", "에인션트 이블의 도움을 받은뒤 다시 로어 대륙으로 나갈", "헤어졌던 동료를 만날",
        "로어성의 궁사들과 결전을 벌일", "로어성에서 로드안과 최후의 결전을 벌일"
    };

    Clear();
    person = ChooseWhom(false); if (person == 0) return;
    ReturnSexData();
    if (!exist(person)) { message(7, sexdata + "는 초감각을 사용할수있는 상태가 아닙니다"); return; }
    if (player[person].classtype == sword) { Print(7, sexdata + "에게는 초감각의 능력이 없습니다."); PressAnyKey(); return; }

    m[0] = "사용할 초감각의 종류 ===>";
    for (int j = 1; j <= 5; j++) m[j] = ReturnMagic(5, j);
    int k_sel = Select(74, 5, 5, true, true);
    
    if (k_sel == 5) { message(7, ReturnMagic(5, 5) + "은 전투에서만 사용됩니다."); return; }

    switch (k_sel) {
        case 1: // Clairvoyance/See Map logic
            if (Map_Header.handicap && (Map_Header.handicap_bit & 32)) { Print(13, " 당신은 누군가에게 투시를 방해 받고 있다."); PressAnyKey(); return; }
            if (player[person].magic_data.esp_magic < 70) { Print(7, sexdata + "는 투시를 시도해 보았지만 아직은 역부족이었다."); PressAnyKey(); return; }
            if (player[person].sp < 10) { SPnotEnough(); return; }
            Clear(); Print(15, "일행은 주위를 투시하고 있다.");
            player[person].sp -= 10;
            Wizard_Eye(); // Simulated with Wizard Eye
            break;
        case 2: // Predict
            if (player[person].magic_data.esp_magic < 10) { Print(7, sexdata + "는 예언을 시도해 보았지만 아직은 역부족이었다."); PressAnyKey(); return; }
            if (player[person].sp < 5) { SPnotEnough(); return; }
            Print(7, " 당신은 당신의 미래를 예언한다 ..."); Print(7, "");
            {
                int pred = ReturnPredict(); std::string s_pred;
                if (pred >= 1 && pred <= 30) s_pred = "당신은 " + Predict_Data[pred - 1] + " 것이다";
                else s_pred = "당신은 어떤 힘에 의해 예언을 방해 받고 있다";
                cPrint(10, 15, " # ", s_pred, "");
            }
            player[person].sp -= 5;
            PressAnyKey();
            break;
        case 3: // Read Mind
            if (player[person].magic_data.esp_magic < 40) { Print(7, sexdata + "는 독심을 시도해 보았지만 아직은 역부족이었다."); PressAnyKey(); return; }
            if (player[person].sp < 20) { SPnotEnough(); return; }
            message(15, "당신은 잠시동안 다른 사람의 마음을 읽을수 있다.");
            party.etc[5] = 3;
            break;
        case 4: // Teleview
            if (player[person].magic_data.esp_magic < 55) { Print(7, sexdata + "는 천리안을 시도해 보았지만 아직은 역부족이었다."); PressAnyKey(); return; }
            if (player[person].sp < player[person].level * 5) { SPnotEnough(); return; }
            player[person].sp -= player[person].level * 5;
            {
                int x1 = 0, y1 = 0;
                switch (face % 4) { case 0: y1 = 1; break; case 1: y1 = -1; break; case 2: x1 = 1; break; case 3: x1 = -1; break; }
                int startX = pas_x, startY = pas_y;
                bool old_snd = SoundOn; SoundOn = false;
                hany = 30; Print(15, "천리안의 사용중 ...");
                SetColor(14); HPrintXY4Select(250, 184, "아무키나 누르시오 ...");
                
                for (int i = 1; i <= player[person].level; i++) {
                    pas_x += x1; pas_y += y1;
                    if (pas_x < 5 || pas_x >= Map_Header.xmax - 3 || pas_y < 5 || pas_y >= Map_Header.ymax - 3) {
                        pas_x -= x1; pas_y -= y1;
                    } else {
                        Scroll(false);
                        SDL_Event ev; bool skip = false;
                        while (!skip) {
                            while(SDL_PollEvent(&ev)) {
                                if (ev.type == SDL_KEYDOWN) skip = true;
                            }
                            SDL_Delay(10);
                        }
                    }
                }
                Clear(); pas_x = startX; pas_y = startY;
                SoundOn = old_snd; Scroll(true);
            }
            break;
    }
    DisplaySP();
}

void Rest() {
    Clear();
    Print(11, "일행이 여기서 쉴 시간을 지정 하십시오.");
    SetColor(15); HGotoXY(266, 70); HPrint("## ");
    SetColor(10); 
    Rest_Time = 8; // Default
    bool ok_rest = false;
    SDL_Event ev;
    
    while (!ok_rest) {
        char buf[16]; std::snprintf(buf, sizeof(buf), "%2d", Rest_Time);
        SetColor(8); Bar(290, 70, 305, 86);
        SetColor(10); HPrintXY(290, 70, buf);
        SetColor(15); HPrintXY(310, 70, " 시간 동안");
        UpdateScreen();
        
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_KEYDOWN) {
                int j_dir = 0;
                switch (ev.key.keysym.sym) {
                    case SDLK_UP: case SDLK_RIGHT: j_dir = 1; break;
                    case SDLK_DOWN: case SDLK_LEFT: j_dir = -1; break;
                    case SDLK_RETURN: case SDLK_SPACE: ok_rest = true; break;
                    case SDLK_ESCAPE: Clear(); return;
                }
                if (j_dir != 0) {
                    Rest_Time += j_dir;
                    if (Rest_Time < 1) Rest_Time = 1;
                    if (Rest_Time > 24) Rest_Time = 24;
                }
            }
        }
        SDL_Delay(20);
    }
    
    Clear();
    for (person = 1; person <= 6; person++) {
        Lore& p = player[person];
        if (p.name_len > 0) {
            std::string n_str = FromPas(p.name, p.name_len);
            std::string s_gen = (p.sex == male) ? "그의" : "그녀의";
            if (party.food <= 0) Print(4, "일행은 식량이 바닥났다");
            else if (p.dead > 0) Print(7, n_str + ReturnSJosa(n_str) + " 죽었다");
            else if (p.unconscious > 0 && p.poison == 0) {
                p.unconscious -= p.level * (Rest_Time / 2);
                if (p.unconscious <= 0) {
                    Print(15, n_str + ReturnSJosa(n_str) + " 의식이 회복되었다");
                    p.unconscious = 0; if (p.hp <= 0) p.hp = 1; party.food--;
                } else Print(15, n_str + ReturnSJosa(n_str) + " 여전히 의식 불명이다");
            }
            else if (p.unconscious > 0 && p.poison > 0) Print(7, "독때문에, " + n_str + " " + s_gen + " 의식은 회복되지 않았다");
            else if (p.poison > 0) Print(7, "독때문에, " + n_str + " " + s_gen + " 건강은 회복되지 않았다");
            else {
                int heal_amount = p.level * Rest_Time;
                if (p.hp >= p.endurance * p.level * 10) { if (party.food < 255) party.food++; }
                p.hp += heal_amount;
                if (p.hp >= p.endurance * p.level * 10) {
                    p.hp = p.endurance * p.level * 10;
                    Print(15, n_str + ReturnSJosa(n_str) + " 모든 건강이 회복되었다");
                } else Print(15, n_str + ReturnSJosa(n_str) + " 치료되었다");
                party.food--;
            }
        }
    }
    
    if (party.etc[1] > 0) {
        int red = Rest_Time / 3 + 1;
        party.etc[1] = (party.etc[1] >= red) ? party.etc[1] - red : 0;
    }
    for (int i = 2; i <= 4; i++) party.etc[i] = 0;
    
    for (int i = 1; i <= 6; i++) {
        if (exist(i)) {
            if (player[i].classtype == magic) player[i].sp = player[i].mentality * player[i].level * 10;
            else if (player[i].classtype == sword && player[i]._class == 7) player[i].sp = player[i].mentality * player[i].level * 5;
            else player[i].sp = 0;
        }
    }

    Print(7, "");
    for (int k = 1; k <= 6; k++) {
        if (exist(k)) {
            longint pe = player[k].potential_experience;
            int lvl = 0;
            while (pe >= ReturnEXP(lvl) && lvl <= 40) lvl++;
            lvl--;
            if (player[k].level < lvl || lvl == 40) {
                if (lvl < 40 || player[k].level != 40) {
                    Print(11, FromPas(player[k].name, player[k].name_len) + "의 레벨은 " + std::to_string(lvl) + " 입니다");
                    player[k].level = lvl;
                }
            }
        }
    }

    SimpleDisCond();
    PlusTime(Rest_Time, 0, 0);

    if (Time_Watch) {
        if (party.year > watch_year || (party.year == watch_year && party.day > watch_day) ||
            (party.year == watch_year && party.day == watch_day && party.hour > watch_hour) ||
            (party.year == watch_year && party.day == watch_day && party.hour == watch_hour && party.min >= watch_min)) {
            party.year = watch_year; party.day = watch_day; party.hour = watch_hour; party.min = watch_min; party.sec = watch_sec;
        }
    }
    PressAnyKey();
}

void SetBackPack() {
    // A simplified Backpack/Equipment menu adapting original semantics
    Clear();
    person = ChooseWhom(false); if (person == 0) return;
    Lore& p = player[person];
    std::string n_str = FromPas(p.name, p.name_len);
    
    while(true) {
        Clear();
        Print(15, n_str + "의 장비 관리");
        m[0] = ""; m[1] = "장비 장착"; m[2] = "장비 해제";
        int action = Select(70, 2, 2, false, true);
        if (action == 0) break;
        
        if (action == 1) { // Equip
            while(true) {
                Clear();
                Print(11, "장착할 항목을 선택하세요");
                int list_idx = 1; int mapping[60][2];
                for (int j = 1; j <= 6; j++) {
                    for (int i = 1; i <= 10; i++) {
                        if (party.BackPack.data[j-1][i-1] > 0) {
                            mapping[list_idx][0] = j; mapping[list_idx][1] = i;
                            std::string item_name;
                            if (j <= 4) item_name = ReturnWeapon((j - 1) * 7 + i);
                            else if (j == 5) item_name = ReturnShield(i);
                            else item_name = ReturnArmor(i);
                            m[list_idx] = item_name + " (" + std::to_string(party.BackPack.data[j-1][i-1]) + "개)";
                            list_idx++;
                        }
                    }
                }
                if (list_idx == 1) { Print(7, "가진 장비가 없습니다."); PressAnyKey(); break; }
                
                int sel = Select(70, list_idx - 1, list_idx - 1, false, true);
                if (sel == 0) break;
                
                int j_cat = mapping[sel][0], i_item = mapping[sel][1];
                bool ok = false;
                if (j_cat <= 4) {
                    if (p.weapon == 0 && Verify_Weapon(person, (j_cat - 1) * 7 + i_item)) {
                        ok = true; p.weapon = (j_cat - 1) * 7 + i_item;
                    }
                } else if (j_cat == 5) {
                    if (p.shield == 0 && Verify_Shield(person, i_item)) {
                        ok = true; p.shield = i_item;
                    }
                } else if (j_cat == 6) {
                    if (p.armor == 0 && Verify_Armor(person, i_item)) {
                        ok = true; p.armor = i_item;
                    }
                }
                
                if (ok) {
                    party.BackPack.data[j_cat-1][i_item-1]--;
                    Print(15, "장착했습니다.");
                } else Print(7, "장착할 수 없습니다.");
                PressAnyKey();
            }
        } else if (action == 2) { // Disarm
            while(true) {
                Clear(); Print(11, "해제할 장비를 선택하세요");
                m[0] = ""; 
                m[1] = "무기: " + ReturnWeapon(p.weapon);
                m[2] = "방패: " + ReturnShield(p.shield);
                m[3] = "갑옷: " + ReturnArmor(p.armor);
                int sel = Select(70, 3, 3, false, true);
                if (sel == 0) break;
                
                bool ok = false;
                if (sel == 1 && p.weapon > 0) {
                    ok = true;
                    int j_cat = (p.weapon - 1) / 7 + 1; int i_item = p.weapon - (j_cat - 1) * 7;
                    if (party.BackPack.data[j_cat-1][i_item-1] < 255) party.BackPack.data[j_cat-1][i_item-1]++;
                    p.weapon = 0;
                } else if (sel == 2 && p.shield > 0) {
                    ok = true;
                    if (party.BackPack.data[4][p.shield-1] < 255) party.BackPack.data[4][p.shield-1]++;
                    p.shield = 0;
                } else if (sel == 3 && p.armor > 0) {
                    ok = true;
                    if (party.BackPack.data[5][p.armor-1] < 255) party.BackPack.data[5][p.armor-1]++;
                    p.armor = 0;
                }
                
                if (ok) Print(15, "해제했습니다."); else Print(7, "해제할 수 없습니다.");
                PressAnyKey();
            }
        }
    }
}

void Use_Item(int select_num) {
    m[0] = "사용할 물품을 고르시오.";
    m[1] = "체력 회복약"; m[2] = "마법 회복약"; m[3] = "해독의 약초"; m[4] = "의식의 약초"; m[5] = "부활의 약초";
    m[6] = "소환 문서"; m[7] = "대형 횃불"; m[8] = "수정 구슬"; m[9] = "비행 부츠"; m[10] = "이동 구슬";
    
    int _select = Select(70, select_num, select_num, true, true);
    if (_select == 0) return;
    
    std::string remember = m[_select];
    if (party.item[_select - 1] == 0) {
        std::string s_jos = (_select == 1 || _select == 2 || _select == 7 || _select == 8 || _select == 10) ? "이" : "가";
        Print(7, " 하지만 " + remember + s_jos + " 없습니다."); PressAnyKey(); return;
    }
    party.item[_select - 1]--;
    
    switch (_select) {
        case 1: { // HP Potion
            Lore& p = player[person]; std::string n_str = FromPas(p.name, p.name_len);
            p.hp += 1000;
            if (p.hp > p.endurance * p.level * 10) {
                Print(15, " " + n_str + ReturnSJosa(n_str) + " 모든 건강이 회복 되었습니다.");
                p.hp = p.endurance * p.level * 10;
            } else Print(15, " " + n_str + ReturnSJosa(n_str) + " 건강이 회복 되었습니다.");
            Display_Condition(); break;
        }
        case 2: { // SP Potion
            Lore& p = player[person]; std::string n_str = FromPas(p.name, p.name_len);
            if (p.classtype != magic && p._class != 7) {
                Print(7, " " + n_str + ReturnSJosa(n_str) + " 마법사 계열이 아닙니다."); break;
            }
            int max_sp = (p.classtype == magic) ? p.mentality * p.level * 10 : p.mentality * p.level * 5;
            p.sp += 1000;
            if (p.sp > max_sp) {
                Print(15, " " + n_str + ReturnSJosa(n_str) + " 모든 마법 지수가 회복 되었습니다.");
                p.sp = max_sp;
            } else Print(15, " " + n_str + ReturnSJosa(n_str) + " 마법 지수가 회복 되었습니다.");
            Display_Condition(); break;
        }
        case 3: { // Cure Poison
            Lore& p = player[person]; std::string n_str = FromPas(p.name, p.name_len);
            if (p.poison == 0) { Print(7, " " + n_str + ReturnSJosa(n_str) + " 중독 되지 않았습니다."); break; }
            p.poison = 0; Print(15, " " + n_str + ReturnSJosa(n_str) + " 해독 되었습니다.");
            Display_Condition(); break;
        }
        case 4: { // Awaken
            int k_idx = ChooseWhom(false); if (k_idx == 0) break;
            Lore& p = player[k_idx]; std::string n_str = FromPas(p.name, p.name_len);
            if (p.unconscious == 0) { Print(7, " " + n_str + ReturnSJosa(n_str) + " 의식이 있습니다."); break; }
            p.unconscious = 0;
            if (p.dead == 0) { Print(15, " " + n_str + ReturnSJosa(n_str) + " 의식을 차렸습니다."); if (p.hp <= 0) p.hp = 1; }
            else Print(7, " " + n_str + ReturnSJosa(n_str) + " 이미 죽은 상태 입니다.");
            Display_Condition(); break;
        }
        case 5: { // Revive
            int k_idx = ChooseWhom(false); if (k_idx == 0) break;
            Lore& p = player[k_idx]; std::string n_str = FromPas(p.name, p.name_len);
            if (p.dead == 0) { Print(7, " " + n_str + ReturnSJosa(n_str) + " 죽지 않았습니다."); break; }
            if (p.dead < 10000) {
                p.dead = 0; if (p.unconscious >= p.endurance * p.level) p.unconscious = p.endurance * p.level - 1;
                Print(15, " " + n_str + ReturnSJosa(n_str) + " 다시 살아났습니다.");
            } else Print(7, " " + n_str + "의 죽음은 이 약초로는 살리지 못합니다.");
            Display_Condition(); break;
        }
        case 6: { // Summon
            int ability = 0, cnt = 0;
            for (int i = 1; i <= 5; i++) { if (exist(i)) { cnt++; ability += player[i].level; } }
            ability = (cnt > 0) ? (ability * 5 / cnt) : 5;
            
            Lore& p = player[6];
            int r_idx = rand() % 8;
            auto random_offset = [](int seed) { return (rand() % (seed * 2 + 1)) - seed; };
            std::string s_name;
            switch(r_idx) {
                case 0: s_name="밴더스내치"; p.endurance=15+random_offset(5); p.resistance=8+random_offset(5); p.accuracy=12+random_offset(5); p.weapon=33; p.wea_power=ability*3; p.potential_ac=3; p.ac=3; break;
                case 1: s_name="캐리온 크롤러"; p.endurance=20+random_offset(5); p.resistance=14+random_offset(5); p.accuracy=13+random_offset(5); p.weapon=34; p.wea_power=ability*1; p.potential_ac=3; p.ac=3; break;
                case 2: s_name="켄타우루스"; p.endurance=17+random_offset(5); p.resistance=12+random_offset(5); p.accuracy=18+random_offset(2); p.weapon=35; p.wea_power=ability*1.5; p.potential_ac=2; p.ac=2; break;
                case 3: s_name="데모고르곤"; p.endurance=18+random_offset(5); p.resistance=5+random_offset(5); p.accuracy=17+random_offset(3); p.weapon=36; p.wea_power=ability*4; p.potential_ac=4; p.ac=4; break;
                case 4: s_name="듈라한"; p.endurance=10+random_offset(5); p.resistance=20; p.accuracy=17; p.weapon=16; p.wea_power=ability*1; p.potential_ac=3; p.ac=3; break;
                case 5: s_name="에틴"; p.endurance=10+random_offset(5); p.resistance=10; p.accuracy=10+random_offset(9); p.weapon=8; p.wea_power=ability*0.8; p.potential_ac=1; p.ac=1; break;
                case 6: s_name="헬하운드"; p.endurance=14+random_offset(5); p.resistance=9+random_offset(5); p.accuracy=11+random_offset(5); p.weapon=33; p.wea_power=ability*3; p.potential_ac=2; p.ac=2; break;
                case 7: s_name="미노타우루스"; p.endurance=13+random_offset(5); p.resistance=11+random_offset(5); p.accuracy=14+random_offset(5); p.weapon=9; p.wea_power=ability*3; p.potential_ac=2; p.ac=2; break;
            }
            p.name_len = std::min((int)s_name.length(), 16); std::memcpy(p.name, s_name.c_str(), p.name_len);
            p.sex = neutral; p._class = 0; p.classtype = unknown; p.strength = 10+random_offset(5); p.level = std::max(1, ability / 5);
            p.mentality = 10+random_offset(5); p.concentration = 10+random_offset(5); p.agility = 0; p.luck = 10+random_offset(5);
            p.poison = 0; p.unconscious = 0; p.dead = 0; p.hp = p.endurance * p.level * 10; p.sp = 0;
            p.experience = 0; p.potential_experience = 0; p.shield = 0; p.shi_power = 0; p.armor = 0;
            std::memset(&p.raw_skills, 0, sizeof(p.raw_skills));
            Display_Condition();
            Print(15, s_name + ReturnSJosa(s_name) + " 다른 차원으로 부터 소환 되어졌습니다.");
            break;
        }
        case 7: { // Torch
            party.etc[1] = std::min(255, (int)party.etc[1] + 10);
            Print(15, " 일행은 대형 횃불을 켰습니다."); Silent_Scroll(); break;
        }
        case 8: Wizard_Eye(); break;
        case 9: { // Levitate boots
            party.etc[2] = 255; party.etc[3] = 255; party.etc[4] = 255;
            Print(15, " 일행은 모두 비행 부츠를 신었습니다."); break;
        }
        case 10: Teleport(); break;
    }
    
    if (_select != 8) {
        SetColor(3); HPrintXY4Select(260, 256, (" " + remember + ReturnSJosa(remember) + " 이제 " + std::to_string(party.item[_select - 1]) + "개 남았습니다.").c_str());
        if (party.etc[6] == 1) PressAnyKey();
    }
}

void Use_Crystal() {
    int j = 0;
    for (int i = 5; i <= 8; i++) {
        if (party.crystal[i] > 0) { j++; m[j] = ReturnMagic(7, i + 1); }
    }
    if (j == 0) return;
    
    Clear(); m[0] = ""; Print(12, "크리스탈을 사용 ----"); Print(7, ""); Print(11, "당신이 사용할 크리스탈을 고르시오.");
    int k_sel = Select(130, j, j, false, true); if (k_sel == 0) return;
    
    int i_val = 4, j_val = 0;
    while (j_val < k_sel) {
        i_val++; if (party.crystal[i_val] > 0) j_val++;
    }
    
    switch (i_val) {
        case 5: { // Summon Crystal Creature
            Lore& p = player[6]; std::string s_name;
            int r_idx = rand() % 5;
            if (r_idx <= 1) {
                s_name = "크리스탈 드래곤"; p.classtype = dragon; p.strength = 25; p.mentality = 20; p.concentration = 20;
                p.endurance = 30; p.resistance = 20; p.agility = 0; p.accuracy = 20; p.luck = 20; p.weapon = 49; p.wea_power = 255; p.potential_ac = 4;
            } else if (r_idx <= 3) {
                s_name = "크리스탈 고렘"; p.classtype = golem; p.strength = 20; p.mentality = 0; p.concentration = 0;
                p.endurance = 40; p.resistance = 25; p.agility = 0; p.accuracy = 13; p.luck = 0; p.weapon = 0; p.wea_power = 150; p.potential_ac = 5;
            } else {
                s_name = "대천사장"; p.classtype = magic; p.strength = 20; p.mentality = 20; p.concentration = 20;
                p.endurance = 30; p.resistance = 19; p.agility = 20; p.accuracy = 20; p.luck = 20; p.weapon = 44; p.wea_power = 200; p.potential_ac = 5;
            }
            p.name_len = std::min((int)s_name.length(), 16); std::memcpy(p.name, s_name.c_str(), p.name_len);
            p.sex = neutral; p._class = 0; p.level = 30; p.poison = 0; p.unconscious = 0; p.dead = 0;
            p.hp = p.endurance * p.level * 10; p.sp = 0; p.ac = p.potential_ac;
            p.experience = 0; p.potential_experience = 0; p.shield = 0; p.shi_power = 0; p.armor = 0;
            std::memset(&p.raw_skills, 0, sizeof(p.raw_skills));
            Display_Condition(); party.crystal[5]--;
            break;
        }
        case 6: // Energy Crystal (Full Heal)
            for (int i = 1; i <= 6; i++) {
                if (player[i].name_len > 0) {
                    if (player[i].dead == 0) {
                        player[i].unconscious = 0; player[i].hp = player[i].endurance * player[i].level * 10;
                    } else {
                        player[i].dead = 0; player[i].unconscious = 0; player[i].hp = 1;
                    }
                }
            }
            Print(7, " 에너지 크리스탈은 강한 에너지를  우리 대원들의 몸속으로  방출하였고  그 에너지를 취한 대원들은 모두 원래의 기운을 되찾았다.");
            Display_Condition(); PressAnyKey(); party.crystal[6]--;
            break;
        case 7: // Ancient Evil / Dome
            if (mapname == "UnderGrd" && pas_x == 46 && pas_y == 48) {
                Clear();
                Print(7, " 갑자기 당신의 위쪽에서 귀에 익은 음성이 들려왔다.  그 목소리의 주인공은 에인션트 이블이었다."); PressAnyKey();
                Print(9, " 역시  당신은 제가 생각했던대로 정말 훌륭하십니다.  당신이 찾은 이곳이라면 아마 베리언트 피플즈의 사람들이 숨기에는 충분할 것입니다.  잠깐만 기다려 주십시오.  저의 염력으로 도시를 건설해 보겠습니다."); PressAnyKey();
                mapname = "Dome"; Load();
                for (int jy = 1; jy <= Map_Header.ymax; jy++) {
                    for (int ix = 1; ix <= Map_Header.xmax; ix++) {
                        PutMap(ix, jy, (rand() % 10 == 0) ? 45 : 47);
                    }
                }
                pas_x = 25; pas_y = 50; Scroll(true);
                watch_year = party.year; watch_day = party.day; watch_hour = party.hour; watch_min = party.min; watch_sec = party.sec;
                Time_Watch = true; time_event_no = 1;
                watch_min += 20;
                if (watch_min > 59) {
                    watch_min -= 60; watch_hour++;
                    if (watch_hour > 23) {
                        watch_hour -= 24; watch_day++;
                        if (watch_day > 359) { watch_day -= 360; watch_year++; }
                    }
                }
            } else {
                Clear(); Print(7, " 크리스탈에는 아무런 반응이 없었다."); PressAnyKey();
            }
            break;
        case 8: // Crystal Ball
            if (Map_Header.Tile_Type != ground) {
                Print(7, " 크리스탈 볼은 주위가 트인곳에서만 작동합니다."); PressAnyKey(); return;
            }
            watch_year = party.year; watch_day = party.day; watch_hour = party.hour; watch_min = party.min; watch_sec = party.sec;
            Time_Watch = true; time_event_no = 5;
            break;
    }
}

void GameOption() {
    m[0] = "게임 선택 상황";
    m[1] = "난이도 조절"; m[2] = "정식 일행의 순서 정렬"; m[3] = "일행에서 제외 시킴";
    m[4] = "일행의 장비를 버림"; m[5] = "이전의 게임을 재개"; m[6] = "현재의 게임을 저장"; m[7] = "게임을 마침";
    
    int k_sel = Select(60, 7, 7, true, true);
    switch (k_sel) {
        case 1: { // Difficulty
            SetColor(12); HPrintXY(275, 30, "한번에 출현하는 적들의"); HPrintXY(287, 46, "최대치를 기입하십시오");
            m[0] = ""; for (int i = 1; i <= 6; i++) m[i] = std::to_string(i + 2) + " 명의 적들";
            int enc_max = Select(90, 6, 6, false, true) + 2;
            if (enc_max >= 3 && enc_max <= 8 && maxenemy) *maxenemy = enc_max;
            
            m[0] = "일행들의 지금 성격은 어떻습니까 ?";
            m[1] = "일부러 전투를 피하고 싶다"; m[2] = "너무 잦은 전투는 원하지 않는다"; m[3] = "마주친 적과는 전투를 하겠다";
            m[4] = "보이는 적들과는 모두 전투하겠다"; m[5] = "그들은 피에 굶주려 있다";
            int enc_rate = Select(71, 5, 5, false, true);
            if (enc_rate >= 1 && enc_rate <= 5 && encounter) *encounter = 6 - enc_rate;
            
            m[0] = "의식 불명인 적까지 공격 하겠습니까 ?"; m[1] = "물론 그렇다"; m[2] = "그렇지 않다";
            int c_sel = Select(90, 2, 2, false, true);
            if (c_sel > 0) Cruel = (c_sel == 1);
            break;
        }
        case 2: { // Reorder Party
            SetColor(12); HPrintXY(275, 30, "현재의 일원의 전투 순서를 정렬 하십시오.");
            SetColor(11); HPrintXY(265, 70, "순서를 바꿀 일원");
            m[0] = ""; 
            for (int i = 2; i <= 5; i++) m[i - 1] = (player[i].name_len > 0) ? FromPas(player[i].name, player[i].name_len) : " 빈자리";
            int p1 = Select(105, 4, 4, false, false) + 1;
            if (p1 == 1) { Clear(); break; }
            
            SetColor(11); HPrintXY(445, 70, "순서를 바꿀 일원");
            for (int i = 2; i <= 5; i++) m[i - 1] = "                       " + m[i - 1];
            int p2 = Select(105, 4, 4, false, false) + 1;
            if (p2 == 1) { Clear(); break; }
            
            Lore temp = player[p1]; player[p1] = player[p2]; player[p2] = temp;
            Display_Condition(); Clear();
            break;
        }
        case 3: { // Dismiss
            SetColor(12); HPrintXY(275, 35, "일행에서 제외 시킬 사람을 고르십시오.");
            m[0] = ""; 
            for (int i = 2; i <= 6; i++) m[i - 1] = (player[i].name_len > 0) ? FromPas(player[i].name, player[i].name_len) : "빈자리";
            int p_dis = Select(75, 5, 5, false, true) + 1;
            if (p_dis == 1) { Clear(); break; }
            
            player[p_dis].name_len = 0; std::memset(player[p_dis].name, 0, 16);
            Display_Condition();
            break;
        }
        case 4: { // Drop Equipment
            Clear(); Print(12, "장비를 버릴 사람을 고르시오.");
            int k_idx = ChooseWhom(false); if (k_idx == 0) break;
            
            Print(12, "버릴 장비를 고르시오.");
            m[0] = ""; m[1] = ReturnWeapon(player[k_idx].weapon); m[2] = ReturnShield(player[k_idx].shield); m[3] = ReturnArmor(player[k_idx].armor);
            int j_sel = Select(120, 3, 3, false, true); if (j_sel == 0) break;
            
            std::string n_str = FromPas(player[k_idx].name, player[k_idx].name_len);
            if (j_sel == 1) { player[k_idx].weapon = 0; Print(7, n_str + "의 무기는 해제 되었습니다."); }
            else if (j_sel == 2) { player[k_idx].shield = 0; Print(7, n_str + "의 방패는 해제 되었습니다."); }
            else if (j_sel == 3) { player[k_idx].armor = 0; Print(7, n_str + "의 갑옷은 해제 되었습니다."); }
            PressAnyKey(); break;
        }
        case 5: Restore_Game(); break;
        case 6: { // Save
            if (Time_Watch) {
                Clear(); Print(12, " 지금은 게임 저장을 할 수 없습니다."); Print(12, " 잠시만 더 기다려 주십시오."); PressAnyKey(); break;
            }
            m[0] = "게임의 저장 장소를 선택하십시오."; m[1] = "없습니다"; m[2] = "본 게임 데이타";
            for (int i = 3; i <= 10; i++) m[i] = "게임 데이타 " + std::to_string(i - 2) + " (부)";
            int s_sel = Select(65, 10, 10, false, true);
            if (s_sel < 2) break;
            LoadNo = '0' + (s_sel - 1);
            SetColor(12); HPrintXY(312, 80, "현재의 게임을 저장합니다");
            Save();
            SetColor(7); HPrintXY(360, 102, "성공했습니다");
            LoadNo = '1'; PressAnyKey();
            break;
        }
        case 7: GameOver(); break;
    }
}

void SelectMode() {
    m[0] = "당신의 명령을 고르시오 ===>";
    m[1] = "일행의 상황을 본다       [P]";
    m[2] = "개인의 상황을 본다       [V]";
    m[3] = "일행이 가진 물품을 확인  [I]";
    m[4] = "일행의 건강 상태를 본다  [Q]";
    m[5] = "마법을 사용한다          [C]";
    m[6] = "초능력을 사용한다        [E]";
    m[7] = "무기 장착 및 해제        [W]";
    m[8] = "여기서 쉰다              [R]";
    m[9] = "물품을 사용한다          [U]";
    m[10] = "게임 선택 상황           [G]";
    
    int k_sel = Select(60, 10, 10, true, true);
    switch (k_sel) {
        case 0: return;
        case 1: ViewParty(); break;
        case 2: ViewCharacter(); break;
        case 3: Inventory(); break;
        case 4: QuickView(); break;
        case 5: CastSpell(); break;
        case 6: Extrasense(); break;
        case 7: SetBackPack(); break;
        case 8: Rest(); break;
        case 9:
            m[0] = ""; m[1] = "약초나 약물을 사용한다"; m[2] = "크리스탈을 사용한다";
            switch (Select(100, 2, 2, true, true)) {
                case 0: break;
                case 1:
                    person = ChooseWhom(false);
                    if (person > 0) {
                        if (exist(person)) Use_Item(10);
                        else {
                            std::string n_str = FromPas(player[person].name, player[person].name_len);
                            Print(7, " " + n_str + ReturnSJosa(n_str) + " 물품을 사용할 수 있는 상태가 아닙니다.");
                            PressAnyKey();
                        }
                    }
                    break;
                case 2: Use_Crystal(); break;
            }
            break;
        case 10: GameOption(); break;
    }
}