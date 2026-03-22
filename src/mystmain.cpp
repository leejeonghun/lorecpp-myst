#include "mystmain.h"
#include "mystsub.h"
#include "uhanx.h"
#include "mystmenu.h"
#include "mystspec.h"
#include "mystent.h"
#include "mysttalk.h"
#include "mystbatt.h"
#include "mysttime.h"
#include <SDL2/SDL.h>
#include <cstdlib>

void Move_Mode() {
    Scroll(true);
    int j_val = 0;
    
    // 파티원 독/죽음/기절 상태 업데이트
    for (int p_idx = 1; p_idx <= 6; p_idx++) {
        Lore& p = player[p_idx];
        if (p.name_len > 0) {
            if (p.poison > 0) p.poison++;
            if (p.poison > 10) {
                p.poison = 1;
                if (p.dead > 0 && p.dead < 100) p.dead++;
                else if (p.unconscious > 0) {
                    p.unconscious++;
                    if (p.unconscious > p.endurance * p.level) p.dead = 1;
                } else {
                    p.hp--; 
                    if (p.hp <= 0) p.unconscious = 1;
                }
                j_val++;
            }
        }
    }
    
    // 변화된 파티원이 있다면 체력/상태창 갱신
    if (j_val > 0) { 
        DisplayHP(); 
        DisplayCondition(); 
    }
    DetectGameOver();
    
    // 이동 관련 타이머 감소
    if (party.etc[5] > 0) party.etc[5]--;
    
    // 적 조우 확률 (encounter 포인터 참조)
    if (encounter && (std::rand() % ((*encounter) * 20 + 1)) == 0) { 
        EncounterEnemy(); 
    }
    
    // 시간에 따른 변화
    if (position == ground) PlusTime(0, 2, 0); 
    else PlusTime(0, 0, 5);
    
    // 타임워치 이벤트 발생 조건
    if (Time_Watch) {
        if ((party.year > watch_year) || 
            (party.year == watch_year && party.day > watch_day) || 
            (party.year == watch_year && party.day == watch_day && party.hour > watch_hour) || 
            (party.year == watch_year && party.day == watch_day && party.hour == watch_hour && party.min >= watch_min)) {
            Time_Watch = false; 
            Time_Event(time_event_no);
        }
    }
}

void enter_water() { 
    if (party.etc[2] > 0) Move_Mode(); 
    else originposition(); 
}

void enter_swamp() { 
    Scroll(true);
    int j_val = 0;
    
    for (int p_idx = 1; p_idx <= 6; p_idx++) {
        Lore& p = player[p_idx];
        if (p.name_len > 0) {
            if (p.poison > 0) p.poison++;
            if (p.poison > 10) {
                p.poison = 1;
                if (p.dead > 0 && p.dead < 100) p.dead++;
                else if (p.unconscious > 0) {
                    p.unconscious++;
                    if (p.unconscious > p.endurance * p.level) p.dead = 1;
                } else {
                    p.hp--;
                    if (p.hp <= 0) p.unconscious = 1;
                }
                j_val++;
            }
        }
    }
    
    if (party.etc[3] > 0) {
        party.etc[3]--;
    } else {
        Clear();
        for (int i_idx = 1; i_idx <= 6; i_idx++) m[i_idx] = "";
        
        for (int i_idx = 1; i_idx <= 6; i_idx++) {
            int p_luck = player[i_idx].luck > 0 ? player[i_idx].luck : 1;
            if (std::rand() % 20 + 1 >= p_luck) m[i_idx] = "!";
        }
        
        Print(12, "일행은 독이 있는 늪에 들어갔다 !!!");
        Print(7, "");
        
        for (int p_idx = 1; p_idx <= 6; p_idx++) {
            Lore& p = player[p_idx];
            if (p.name_len > 0 && m[p_idx] == "!") {
                std::string n_str = FromPas(p.name, p.name_len);
                Print(13, n_str + ReturnSJosa(n_str) + " 중독 되었다.");
                if (p.poison == 0) p.poison = 1;
            }
        }
        DisplayCondition();
    }
    
    if (j_val > 0) {
        DisplayHP();
        DisplayCondition();
    }
    DetectGameOver();
}

void enter_lava() { 
    Scroll(true);
    Clear();
    Print(12, "일행은 용암지대로 들어섰다 !!!");
    Print(7, "");
    
    for (int p_idx = 1; p_idx <= 6; p_idx++) {
        Lore& p = player[p_idx];
        if (p.name_len > 0) {
            int p_luck = p.luck > 0 ? p.luck : 1;
            int dmg = ((std::rand() % 40) + 40 - 2 * (std::rand() % p_luck)) * 10;
            
            if (dmg > 0) {
                if (p.hp > 0 && p.unconscious == 0) {
                    p.hp -= dmg;
                    if (p.hp <= 0) p.unconscious = 1;
                } else if (p.hp > 0 && p.unconscious > 0) {
                    p.hp -= dmg;
                } else if (p.unconscious > 0 && p.dead == 0) {
                    p.unconscious += dmg;
                    if (p.unconscious > p.endurance * p.level) p.dead = 1;
                } else if (p.dead > 0) {
                    if (p.dead + dmg > 30000) p.dead = 30000;
                    else p.dead += dmg;
                }
            }
        }
    }
    SimpleDisCond();
    DetectGameOver();
}

void MainLoopStep(SDL_Event& e) {
    if (e.type != SDL_KEYDOWN) return;
    
    bool move_attempt = false; 
    pas_x1 = 0; 
    pas_y1 = 0;
    
    switch(e.key.keysym.sym) {
        // 이동 키맵
        case SDLK_UP:    pas_y1 = -1; face = 5; move_attempt = true; break;
        case SDLK_DOWN:  pas_y1 = 1;  face = 4; move_attempt = true; break;
        case SDLK_LEFT:  pas_x1 = -1; face = 7; move_attempt = true; break;
        case SDLK_RIGHT: pas_x1 = 1;  face = 6; move_attempt = true; break;
        
        // 단축키 UI 메뉴
        case SDLK_p: Clear(); ViewParty(); break;
        case SDLK_v: Clear(); ViewCharacter(); break;
        case SDLK_i: Clear(); Inventory(); break;
        case SDLK_q: Clear(); QuickView(); break;
        case SDLK_c: Clear(); CastSpell(); break;
        case SDLK_e: Clear(); Extrasense(); break;
        case SDLK_w: Clear(); SetBackPack(); break;
        case SDLK_r: Clear(); Rest(); break;
        case SDLK_u: { 
            Clear();
            m[0] = ""; 
            m[1] = "약초나 약물을 사용한다"; 
            m[2] = "크리스탈을 사용한다"; 
            int k_sel = Select(100, 2, 2, true, true); 
            if (k_sel == 1) {
                person = ChooseWhom(false);
                if (person > 0) {
                    if (exist(person)) {
                        Use_Item(10); // 원본 파스칼 파라미터 10
                    } else {
                        std::string n_str = FromPas(player[person].name, player[person].name_len);
                        Print(7, " " + n_str + ReturnSJosa(n_str) + " 물품을 사용할 수 있는 상태가 아닙니다.");
                    }
                }
            } else if (k_sel == 2) {
                Use_Crystal(); 
            }
            break; 
        }
        case SDLK_g: Clear(); GameOption(); break;
        
        case SDLK_SPACE: 
            party.etc[6] = 0; 
            SelectMode(); 
            break;
            
        case SDLK_BACKSPACE: 
            SoundOn = !SoundOn; 
            break;
            
        case SDLK_F1: // 원본 #197 폰트 디버그용 치트
            for (int i_idx = 5; i_idx <= 294; i_idx++) font[0].data[i_idx] = 0;
            break;
    }
    
    if (move_attempt) {
        // 직업 계열에 따른 캐릭터 애니메이션 방향 조정
        if (player[1].classtype == sword) {
            if (position == town) face -= 4;
            if (mapname == "Menace") face -= 4;
        }
        if (player[1].classtype == magic) {
            face += 4;
        }
        
        pas_x += pas_x1; 
        pas_y += pas_y1;
        
        // 타일의 물리적 바운더리 검사
        if (pas_x > 4 && pas_x < Map_Header.xmax - 3 && pas_y > 5 && pas_y < Map_Header.ymax - 4) {
            byte tile = Real_Map(pas_x, pas_y);
            
            if (position == town) {
                if (tile == 0 || tile == 19) SpecialEvent();
                else if ((tile >= 1 && tile <= 18) || tile == 20 || tile == 21) originposition();
                else if (tile == 22) { originposition(); EnterMode(); }
                else if (tile == 23) { originposition(); Sign(); }
                else if (tile == 24) enter_water();
                else if (tile == 25) enter_swamp();
                else if (tile == 26) enter_lava();
                else if (tile >= 27 && tile <= 47) Move_Mode();
                else { originposition(); TalkMode(); }
            }
            else if (position == ground) {
                if (tile == 0 || tile == 20) SpecialEvent();
                else if (tile >= 1 && tile <= 19) originposition();
                else if (tile == 22) { originposition(); Sign(); }
                else if (tile == 48) enter_water();
                else if (tile == 23 || tile == 49) enter_swamp();
                else if (tile == 50) enter_lava();
                else if (tile >= 24 && tile <= 47) Move_Mode();
                else { originposition(); EnterMode(); }
            }
            else if (position == den) {
                if (tile == 0 || tile == 52) SpecialEvent();
                else if ((tile >= 1 && tile <= 19) || (tile >= 22 && tile <= 40) || tile == 51) originposition();
                else if (tile == 53) { originposition(); Sign(); }
                else if (tile == 48) enter_water();
                else if (tile == 49) enter_swamp();
                else if (tile == 50) enter_lava();
                else if (tile == 54) { originposition(); EnterMode(); }
                else if (tile >= 41 && tile <= 47) Move_Mode();
                else { originposition(); TalkMode(); }
            }
            else if (position == keep) {
                if (tile == 0 || tile == 52) SpecialEvent();
                else if ((tile >= 1 && tile <= 39) || tile == 51) originposition();
                else if (tile == 53) { originposition(); Sign(); }
                else if (tile == 48) enter_water();
                else if (tile == 49) enter_swamp();
                else if (tile == 50) enter_lava();
                else if (tile == 54) { originposition(); EnterMode(); }
                else if (tile >= 40 && tile <= 47) Move_Mode();
                else { originposition(); TalkMode(); }
            }
        } else { 
            originposition(); 
        }
    }
}

void MainLoop() {
    bool quit = false; 
    SDL_Event e;
    
    // 시작 시 맵 진입 후 최초 렌더링
    Scroll(true);
    
    while (!quit) { 
        while (SDL_PollEvent(&e) != 0) { 
            if (e.type == SDL_QUIT) {
                quit = true; 
            } else if (e.type == SDL_KEYDOWN) {
                // Tab키를 통한 프로그램 종료 (원 파스칼 소스 #9 asm 블록 대응)
                if (e.key.keysym.sym == SDLK_TAB) {
                    quit = true;
                } else {
                    MainLoopStep(e); 
                }
            } 
        } 
        SDL_Delay(10); // CPU 점유율 방어
    }
}