#include "mystcret.h"
#include "mystsub.h"
#include "uhanx.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <cstdlib>

// 외부 폰트/아스키 출력 함수 선언 (UHanX.h에 존재한다고 가정)
extern void PutHan(int x, int y, int idx);
extern void AsciiXY(int x, int y, int ascii);

// 키보드 입력을 동기식(blocking)으로 처리하기 위한 헬퍼 함수
static int ReadKeySync() {
    SDL_Event ev_flush; while(SDL_PollEvent(&ev_flush)) {};
    SDL_Event ev;
    while (true) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                    case SDLK_RIGHT: return 77;
                    case SDLK_LEFT:  return 75;
                    case SDLK_DOWN:  return 80;
                    case SDLK_UP:    return 72;
                    case SDLK_RETURN: case SDLK_SPACE: return 13;
                    case SDLK_ESCAPE: return 27;
                    case SDLK_BACKSPACE: return 8;
                    default:
                        if (ev.key.keysym.sym >= SDLK_0 && ev.key.keysym.sym <= SDLK_9)
                            return ev.key.keysym.sym;
                        if (ev.key.keysym.sym >= SDLK_a && ev.key.keysym.sym <= SDLK_z)
                            return ev.key.keysym.sym;
                        break;
                }
            }
        }
        UpdateScreen();
        SDL_Delay(10);
    }
}

static void P(int x, int y, const std::string& s) {
    OutHPrintXY(x, y, s.c_str(), false); UpdateScreen();
}

static void Screen_Set() {
    SetColor(1); // Blue
    SetPalette(1, 15, 10, 20);
    Bar(0, 0, 639, 479); UpdateScreen();
}

static void RectAngle(int x1, int y1, int x2, int y2) {
    int co = GetColor();
    for (int i = x1; i <= x2; i++) { PutPixel(i, y1, co); PutPixel(i, y2, co); }
    for (int i = y1; i <= y2; i++) { PutPixel(x1, i, co); PutPixel(x2, i, co); }
}

static void Display(int x1, int y1, int x_gap, int y_gap, bool writing) {
    int co = GetColor();
    if (writing) SetColor(12); else SetColor(1); // LightRed vs Blue
    RectAngle(x1, y1, x1 + x_gap, y1 + y_gap);
    SetColor(co);
}

static bool Choose(int x_pos, int y_pos, int x_gap, int y_gap, int x_max, int y_max, int& x, int& y, bool enable_esc) {
    if (x < 1 || x > x_max) x = 1;
    if (y < 1 || y > y_max) y = 1;
    Display(x_pos + (x - 1) * x_gap, y_pos + (y - 1) * y_gap, x_gap, y_gap, true);
    bool ok_sel = false;
    int c;
    do {
        c = ReadKeySync();
        if (c == 77 || c == 75 || c == 80 || c == 72 || c == 13 || c == 27) {
            Display(x_pos + (x - 1) * x_gap, y_pos + (y - 1) * y_gap, x_gap, y_gap, false);
            switch (c) {
                case 77: // Right
                    if (x < x_max) x++; else { if (y < y_max) y++; x = 1; } break;
                case 75: // Left
                    if (x > 1) x--; else { if (y > 1) y--; x = x_max; } break;
                case 80: // Down
                    if (y < y_max) y++; else { if (x < x_max) x++; y = 1; } break;
                case 72: // Up
                    if (y > 1) y--; else { if (x > 1) x--; y = y_max; } break;
                case 13: // Enter
                    ok_sel = true; break;
                case 27: // Esc
                    if (enable_esc) ok_sel = true; break;
            }
            Display(x_pos + (x - 1) * x_gap, y_pos + (y - 1) * y_gap, x_gap, y_gap, true);
        }
    } while (!ok_sel);
    
    Display(x_pos + (x - 1) * x_gap, y_pos + (y - 1) * y_gap, x_gap, y_gap, false);
    return (c != 27);
}

static void Put_Name() {
    const int x_pos = 10;
    const int y_pos = 15;
    const int gap = 20;
    int main_menu = 1;
    std::string name = "";
    int point[5] = { 1, 1, 1, 1, 1 };
    
    SetColor(0); Bar(0, 0, 639, 479); UpdateScreen();
    Restore_Palette();
    Screen_Set();
    SetColor(15);
    
    P(x_pos, y_pos, "당신의 이름을 기록 하십시오.");
    P(x_pos, y_pos + 60, "초성");
    for (int i = 1; i <= 19; i++) PutHan(x_pos + i * gap, y_pos + 80, (i - 1) * 6);
    P(x_pos, y_pos + 120, "중성");
    for (int i = 1; i <= 27; i++) PutHan(x_pos + i * gap, y_pos + 140, (i + 56) * 2);
    P(x_pos, y_pos + 180, "종성");
    AsciiXY(x_pos + gap + 3, y_pos + 203, 'X');
    for (int i = 2; i <= 29; i++) PutHan(x_pos + i * gap, y_pos + 200, (i + 82) * 2);
    
    P(x_pos, y_pos + 240, "영문 & 기호");
    for (int i = 1; i <= 26; i++) AsciiXY(x_pos + i * gap, y_pos + 260, i + 31);
    for (int i = 1; i <= 26; i++) AsciiXY(x_pos + i * gap, y_pos + 280, i + 64);
    for (int i = 1; i <= 26; i++) AsciiXY(x_pos + i * gap, y_pos + 300, i + 96);
    
    P(x_pos + 200, y_pos + 360, "한글  영문  지움  완료");
    SetColor(11);
    P(x_pos, y_pos + 415, "당신의 이름 :");

    auto Menu_Display = [&](bool writing) {
        Display(x_pos + 192 + (main_menu - 1) * 48, y_pos + 352, 48, 32, writing);
    };

    auto Output_Name = [&]() {
        SetColor(1); Bar(x_pos + 110, y_pos + 415, x_pos + 300, y_pos + 415 + 16);
        SetColor(10); P(x_pos + 110, y_pos + 415, name);
    };

    bool ok_outer = false;
    Menu_Display(true);
    
    while (!ok_outer) {
        int c = ReadKeySync();
        if (c == 77) { // Right
            Menu_Display(false);
            if (main_menu >= 1 && main_menu <= 3) main_menu++; else main_menu = 1;
            Menu_Display(true);
        } else if (c == 75) { // Left
            Menu_Display(false);
            if (main_menu >= 2 && main_menu <= 4) main_menu--; else main_menu = 4;
            Menu_Display(true);
        } else if (c == 13) { // Enter
            if (main_menu == 1) { // Hangul Input
                while (name.length() < 12) {
                    Menu_Display(false);
                    int fix;
                    if (!Choose(x_pos + 16, y_pos + 80, 20, 20, 19, 1, point[0], fix, true)) { Menu_Display(true); break; }
                    int Han_Data1 = point[0];
                    if (!Choose(x_pos + 16, y_pos + 140, 20, 20, 27, 1, point[1], fix, true)) { Menu_Display(true); break; }
                    int Han_Data2 = point[1];
                    int Han_Data3;
                    if (!Choose(x_pos + 16, y_pos + 200, 20, 20, 29, 1, point[2], fix, true)) Han_Data3 = 0;
                    else Han_Data3 = (point[2] == 1) ? 0 : point[2] - 1;

                    if (Han_Data1 > 0) point[0] = Han_Data1;
                    if (Han_Data2 > 0) point[1] = Han_Data2;
                    if (Han_Data3 > 0) point[2] = Han_Data3;

                    Han_Data1++; Han_Data2 += 2; Han_Data3++;
                    unsigned char t1 = (0xFF & 0x80) | (Han_Data1 << 2) | (Han_Data2 >> 3);
                    unsigned char t2 = (0xFF & (Han_Data2 << 5)) | Han_Data3;
                    
                    name += (char)t1; name += (char)t2;
                    Output_Name();
                    point[2]++;
                }
            } else if (main_menu == 2) { // English Input
                while (name.length() < 14) {
                    Menu_Display(false);
                    if (!Choose(x_pos + 15, y_pos + 258, 20, 20, 26, 3, point[3], point[4], true)) { Menu_Display(true); break; }
                    char new_char;
                    if (point[4] == 1) new_char = (char)(31 + point[3]);
                    else if (point[4] == 2) new_char = (char)(64 + point[3]);
                    else new_char = (char)(96 + point[3]);
                    name += new_char;
                    Output_Name();
                }
            } else if (main_menu == 3) { // Delete
                if (name.length() > 0) {
                    size_t strptr = 0;
                    bool hangul = false;
                    while (strptr < name.length()) {
                        if ((unsigned char)name[strptr] > 127) { hangul = true; strptr += 2; }
                        else { hangul = false; strptr += 1; }
                    }
                    if (hangul && name.length() >= 2) name.erase(name.length() - 2);
                    else if (!hangul && name.length() >= 1) name.erase(name.length() - 1);
                    Output_Name();
                }
            } else if (main_menu == 4) { // Complete
                if (name.length() > 0) ok_outer = true;
            }
        }
    }
    
    player[1].name_len = name.length();
    std::memcpy(player[1].name, name.c_str(), player[1].name_len);
}

static void Put_Sex() {
    Screen_Set();
    SetColor(15);
    P(200, 40, "당신의 성별을 결정하십시오.");
    P(265, 65, "남성  여성");
    int x_choose = 1, y_choose = 1;
    Choose(258, 60, 47, 26, 2, 1, x_choose, y_choose, false);
    player[1].sex = (x_choose == 1) ? male : female;
}

static void Select_Ability1() {
    SetFont(1);
    const int title_x_pos = 80;
    const int title_y_pos = 320;
    const int title_y_gap = 20;
    const int x_pos = 180;
    const int y_pos = 350;
    const int y_gap = 20;

    int transdata[6] = {0}; // 1~5 사용
    
    auto Which = [&]() -> char {
        while (true) {
            int c = ReadKeySync();
            if (c >= '1' && c <= '3') return (char)c;
        }
    };
    
    auto Erase = [&]() {
        int old_c = GetColor();
        SetColor(1); Bar(title_x_pos, title_y_pos, 639, 450); 
        SetColor(old_c);
        UpdateScreen();
    };

    // Screen clear removed to match original behavior
    SetColor(10);
    P(title_x_pos + 40, y_pos - 100, "지금 부터는 묻는 말에 대답을 해 주십시오.");
    P(title_x_pos + 80, y_pos - 100 + y_gap, "그리고, 자신에게 맞는 답을 소신있게 눌러 주십시오.");
    
    SetColor(11);
    
    // Question 1
    if (rand() % 2 == 0) {
        P(title_x_pos, title_y_pos, "당신이 한 밤중에 공부하고 있을때 밖에서 무슨 소리가 들렸다");
        P(x_pos, y_pos + 0 * y_gap, "1] 밖으로 나가서 알아본다");
        P(x_pos, y_pos + 1 * y_gap, "2] 그 소리가 무엇일까 생각을 한다");
        P(x_pos, y_pos + 2 * y_gap, "3] 공부에만 열중한다");
    } else {
        P(title_x_pos, title_y_pos, "당신이 한 시골에서 버스를 타고가던 중 길 앞을 막고 쓰러져");
        P(title_x_pos, title_y_pos + title_y_gap, "있는 나무 때문에 버스가 지나갈 수 없게 되었다.");
        P(x_pos, y_pos + 1 * y_gap, "1] 버스에서 내려 나무를 치운다");
        P(x_pos, y_pos + 2 * y_gap, "2] 남이 치울거라 생각하며 기다린다");
        P(x_pos, y_pos + 3 * y_gap, "3] 버스 안에서 나무를 치울 방법을 궁리한다");
    }
    char c = Which();
    if (c == '1') transdata[1]++; if (c == '2') transdata[2]++; if (c == '3') transdata[3]++;
    Erase();

    // Question 2
    if (rand() % 2 == 0) {
        P(title_x_pos, title_y_pos, "당신은 체력장 오래달리기에서 포기할 수 없는 한 바퀴를 남겨 놓고");
        P(title_x_pos, title_y_pos + title_y_gap, "거의 탈진 상태가 되었다");
        P(x_pos, y_pos + 1 * y_gap, "1] 힘으로 밀고 나간다");
        P(x_pos, y_pos + 2 * y_gap, "2] 정신력으로 버티며 달린다");
        P(x_pos, y_pos + 3 * y_gap, "3] 그래도 여태까지와 마찬가지로 달린다");
    } else {
        P(title_x_pos, title_y_pos, "당신은 별것도 아닌데 다른 사람에게 심한 모욕을 당했다. 이때");
        P(title_x_pos, title_y_pos + title_y_gap, "당신의 행동은 ?");
        P(x_pos, y_pos + 1 * y_gap, "1] 당장 그 사람과 싸운다");
        P(x_pos, y_pos + 2 * y_gap, "2] 그냥 그 일을 잊어 버린다");
        P(x_pos, y_pos + 3 * y_gap, "3] 속에서는 끓어 오르지만 참고 넘어간다");
    }
    c = Which();
    if (c == '1') transdata[1]++; if (c == '2') transdata[2]++; if (c == '3') transdata[4]++;
    Erase();

    // Question 3
    if (rand() % 2 == 0) {
        P(title_x_pos, title_y_pos, "당신은 이 게임 속에서 적들에게 완전히 포위되어 승산 없이 싸우고");
        P(title_x_pos, title_y_pos + title_y_gap, "있다");
        P(x_pos, y_pos + 1 * y_gap, "1] 힘이 남아 있는한 죽을때까지 싸운다");
        P(x_pos, y_pos + 2 * y_gap, "2] 한가지라도 탈출할 가능성을 찾는다");
        P(x_pos, y_pos + 3 * y_gap, "3] 일단 싸우면서 여러 방법을 생각한다");
    } else {
        P(title_x_pos, title_y_pos, "당신은 큰 죄를 지은 결과로 지금 사형집행을 당하게 되었다");
        P(title_x_pos, title_y_pos + title_y_gap, "그때 당신의 행동은 ?");
        P(x_pos, y_pos + 1 * y_gap, "1] 탈출을 시도한다");
        P(x_pos, y_pos + 2 * y_gap, "2] 현실을 받아들인다");
        P(x_pos, y_pos + 3 * y_gap, "3] 죽지 않기 위해 강한 저항을 한다");
    }
    c = Which();
    if (c == '1') transdata[1]++; if (c == '2') transdata[2]++; if (c == '3') transdata[5]++;
    Erase();

    // Question 4
    if (rand() % 2 == 0) {
        P(title_x_pos, title_y_pos, "당신은 매우 복잡한 매듭을 풀어야하는 일이 생겼다");
        P(x_pos, y_pos + 0 * y_gap, "1] 칼로 매듭을 잘라 버린다");
        P(x_pos, y_pos + 1 * y_gap, "2] 매듭의 끝부분 부터 차근차근 훓어본다");
        P(x_pos, y_pos + 2 * y_gap, "3] 어쨌든 계속 풀려고 손을 놀린다");
    } else {
        P(title_x_pos, title_y_pos, "당신이 이 게임속에서 벌이는 활약상은 ?");
        P(x_pos, y_pos + 0 * y_gap, "1] 거대한 검을 휘두르며 적을 무찌르는 당신");
        P(x_pos, y_pos + 1 * y_gap, "2] 상념의 힘만으로 적을 무력화 시키는 당신");
        P(x_pos, y_pos + 2 * y_gap, "3] 쓰러지고 또 쓰러져도 끝까지 싸워 승리하는 당신");
    }
    c = Which();
    if (c == '1') transdata[1]++; if (c == '2') transdata[3]++; if (c == '3') transdata[4]++;
    Erase();

    // Question 5
    if (rand() % 2 == 0) {
        P(title_x_pos, title_y_pos, "허허 벌판을 걸어가던 당신은 갑작스런 우박을 만난다");
        P(x_pos, y_pos + 0 * y_gap, "1] 당항한 나머지 피할곳을 찾아 뛴다");
        P(x_pos, y_pos + 1 * y_gap, "2] 침착하게 주위를 살펴 안전한곳을 찾는다");
        P(x_pos, y_pos + 2 * y_gap, "3] 우박 정도는 그냥 견딘다");
    } else {
        P(title_x_pos, title_y_pos, "당신은 해발 5000m 가 넘는 고지에서 눈사태 때문에 고립되었다.");
        P(title_x_pos, title_y_pos + title_y_gap, "구조대가 오려면 12시간. 그동안 당신은 어떻게 하겠는가 ?");
        P(x_pos, y_pos + 0 * y_gap, "1] 힘을 다해 구조대의 눈에 잘 띄는 곳으로 이동한다");
        P(x_pos, y_pos + 1 * y_gap, "2] 만약의 사태를 대비해서 힘을 최대한 아껴둔다");
        P(x_pos, y_pos + 2 * y_gap, "3] 추위를 가볍게 견뎌내며 그곳에서 기다린다");
    }
    c = Which();
    if (c == '1') transdata[1]++; if (c == '2') transdata[3]++; if (c == '3') transdata[5]++;
    Erase();

    // Question 6
    if (rand() % 2 == 0) {
        P(title_x_pos, title_y_pos, "집안에 불이나서 탈출하려는데 나무로 만든 문이 좀처럼 열리지");
        P(title_x_pos, title_y_pos + title_y_gap, "않는다");
        P(x_pos, y_pos + 1 * y_gap, "1] 다른 탈출구를 찾아간다");
        P(x_pos, y_pos + 2 * y_gap, "2] 1] 번과 같은 불확실한 도전을 하는것 보다는 확실한 ");
        P(x_pos, y_pos + 3 * y_gap, "   탈출구인 이 문을 끝까지 열려한다");
        P(x_pos, y_pos + 4 * y_gap, "3] 나무문이 타서 구멍이 생길때까지 기다려 탈출한다");
    } else {
        P(title_x_pos, title_y_pos, "당신은 사막을 여행중이다. 오아시스까지는 앞으로 1 km, 당신의");
        P(title_x_pos, title_y_pos + title_y_gap, "행동은 ?");
        P(x_pos, y_pos + 1 * y_gap, "1] 체력을 소모해서라도 좀 더 빨리 걸음을 재촉한다");
        P(x_pos, y_pos + 2 * y_gap, "2] 여태껏 걸어왔던것처럼 꿋꿋하게 간다");
        P(x_pos, y_pos + 3 * y_gap, "3] 당신은 원래 더위를 타지 않아서 빨리갈 필요가 없다");
    }
    c = Which();
    if (c == '1') transdata[1]++; if (c == '2') transdata[4]++; if (c == '3') transdata[5]++;
    Erase();

    // Question 7
    if (rand() % 2 == 0) {
        P(title_x_pos, title_y_pos, "고대에 태어난 당신은, 한날 당신의 눈앞에서 물체가 사라지는");
        P(title_x_pos, title_y_pos + title_y_gap, "마술을 보았을때 당신의 해석은 ?");
        P(x_pos, y_pos + 1 * y_gap, "1] 이것은 마법이다");
        P(x_pos, y_pos + 2 * y_gap, "2] 이것은 사람의 새로운 능력이다");
        P(x_pos, y_pos + 3 * y_gap, "3] 단순한 사람의 눈속임이다");
    } else {
        P(title_x_pos, title_y_pos, "이 게임 속에서는 당신이 주인공이며 전능해질 수있다. 그렇다면");
        P(title_x_pos, title_y_pos + title_y_gap, "지금 당신 앞에있는 수많은 괴물들을 어떻게 처리하겠는가 ?");
        P(x_pos, y_pos + 1 * y_gap, "1] 화염의 폭풍으로 전부 태워버린다");
        P(x_pos, y_pos + 2 * y_gap, "2] 적의 마음을 조작하여 적개심을 없에 버린다");
        P(x_pos, y_pos + 3 * y_gap, "3] 핼버드로 적을 종횡무진 쓰러뜨린다");
    }
    c = Which();
    if (c == '1') transdata[2]++; if (c == '2') transdata[3]++; if (c == '3') transdata[4]++;
    Erase();

    // Question 8
    if (rand() % 2 == 0) {
        P(title_x_pos, title_y_pos, "시험 기간에 당신이 도서관에서 공부를 하려는데 주위가 너무");
        P(title_x_pos, title_y_pos + title_y_gap, "시끄럽다");
        P(x_pos, y_pos + 1 * y_gap, "1] 상관없이 참으며 공부한다");
        P(x_pos, y_pos + 2 * y_gap, "2] 너무 공부를 열심히해서 그런 소리가 안 들린다");
        P(x_pos, y_pos + 3 * y_gap, "3] 시끄러움에 대항하는 마음으로 공부한다");
    } else {
        P(title_x_pos, title_y_pos, "당신은 레드 드래곤에 의해 궁지에 몰리게 되었다. 마지막 일격으로");
        P(title_x_pos, title_y_pos + title_y_gap, "화염을 뿜었다. 당신의 대응은 ?");
        P(x_pos, y_pos + 1 * y_gap, "1] 남극의 브리자드를 불러와서 화염을 무력화 시킨다");
        P(x_pos, y_pos + 2 * y_gap, "2] 당신 앞의 땅이 솟아 오르게 하여 화염을 차단 시킨다");
        P(x_pos, y_pos + 3 * y_gap, "3] 당신의 신체를 불에 견딜 수 있는 상태로 바꾸어 버린다");
    }
    c = Which();
    if (c == '1') transdata[2]++; if (c == '2') transdata[3]++; if (c == '3') transdata[5]++;
    Erase();

    // Question 9
    if (rand() % 2 == 0) {
        P(title_x_pos, title_y_pos, "직장 생활을 하던 당신은 아무 이유없이 상관에게 심한 욕을 들었다");
        P(x_pos, y_pos + 0 * y_gap, "1] 겉으로는 순종하면서 속으로는 감정을 쌓는다");
        P(x_pos, y_pos + 1 * y_gap, "2] 웬만하면 참고 넘긴다");
        P(x_pos, y_pos + 2 * y_gap, "3] 상관인걸 무시하고 이유를 들라며 대든다");
    } else {
        P(title_x_pos, title_y_pos, "당신은 게임속에서 거대한 악마를 만났다. 당신은 혼자뿐이고 이길");
        P(title_x_pos, title_y_pos + title_y_gap, "승산은 희박했다. 어떻게 할 것인가 ?");
        P(x_pos, y_pos + 1 * y_gap, "1] 일단 싸우다가 불리하다고 판단되면 도망간다.");
        P(x_pos, y_pos + 2 * y_gap, "2] 끝까지 악마와 싸운다");
        P(x_pos, y_pos + 3 * y_gap, "3] 재빨리 몸을 숨긴다");
    }
    c = Which();
    if (c == '1') transdata[2]++; if (c == '2') transdata[4]++; if (c == '3') transdata[5]++;
    Erase();

    // Question 10
    if (rand() % 2 == 0) {
        P(title_x_pos, title_y_pos, "당신이 새로운 프로그램을 짜던중 알수없는 오류가 생겼다");
        P(x_pos, y_pos + 0 * y_gap, "1] 차근차근 순서도를 생각하며 오류를 찾는다");
        P(x_pos, y_pos + 1 * y_gap, "2] 여러번 실행 시키며 오류를 찾는다");
        P(x_pos, y_pos + 2 * y_gap, "3] 오류가 작으면 그냥 사용한다");
    } else {
        P(title_x_pos, title_y_pos, "지금은 당신의 국민학교 1 학년 시절. 산수 시험 문제지를 받고보니");
        P(title_x_pos, title_y_pos + title_y_gap, "앞이 깜깜했다. 당신은 어떻게 하겠는가?");
        P(x_pos, y_pos + 1 * y_gap, "1] 배운대로 차근차근 풀어본다");
        P(x_pos, y_pos + 2 * y_gap, "2] 모든 능력(?)을 동원하여 답을 알아낸다");
        P(x_pos, y_pos + 3 * y_gap, "3] 그냥 몇대 맞을 셈치고 백지를 낸다");
    }
    c = Which();
    if (c == '1') transdata[3]++; if (c == '2') transdata[4]++; if (c == '3') transdata[5]++;
    
    SetColor(1); Bar(title_x_pos, y_pos - 100, 639, 450); UpdateScreen();
    
    // 점수 환산
    for (int i = 1; i <= 5; i++) {
        switch (transdata[i]) {
            case 0: transdata[i] = 5; break;
            case 1: transdata[i] = 7; break;
            case 2: transdata[i] = 11; break;
            case 3: transdata[i] = 14; break;
            case 4: transdata[i] = 17; break;
            case 5: transdata[i] = 19; break;
            case 6: transdata[i] = 20; break;
            default: transdata[i] = 10; break;
        }
    }

    player[1].strength = transdata[1];
    player[1].mentality = transdata[2];
    player[1].concentration = transdata[3];
    player[1].endurance = transdata[4];
    player[1].resistance = transdata[5];

    if (player[1].strength > player[1].mentality) player[1].classtype = sword;
    else if (player[1].mentality > player[1].strength) player[1].classtype = magic;
    else if (player[1].endurance > player[1].concentration) player[1].classtype = sword;
    else if (player[1].concentration > player[1].endurance) player[1].classtype = magic;
    else player[1].classtype = magic;

    int j = 4;
    if (player[1].sex == male) {
        player[1].strength += j;
        if (player[1].strength <= 20) j = 0; else { j = player[1].strength - 20; player[1].strength = 20; }
        player[1].endurance += j;
        if (player[1].endurance <= 20) j = 0; else { j = player[1].endurance - 20; player[1].endurance = 20; }
        player[1].resistance += j;
        if (player[1].resistance > 20) player[1].resistance = 20;
    } else {
        player[1].mentality += j;
        if (player[1].mentality <= 20) j = 0; else { j = player[1].mentality - 20; player[1].mentality = 20; }
        player[1].concentration += j;
        if (player[1].concentration <= 20) j = 0; else { j = player[1].concentration - 20; player[1].concentration = 20; }
        player[1].resistance += j;
        if (player[1].resistance > 20) player[1].resistance = 20;
    }

    SetColor(11);
    P(115, 225, "\x04 체  력 : " + std::to_string(player[1].strength));
    P(115, 245, "\x04 정신력 : " + std::to_string(player[1].mentality));
    P(115, 265, "\x04 집중력 : " + std::to_string(player[1].concentration));
    P(115, 285, "\x04 인내력 : " + std::to_string(player[1].endurance));
    P(115, 305, "\x04 저항력 : " + std::to_string(player[1].resistance));

    SetColor(3);
    player[1].hp = player[1].endurance * 10;
    P(123, 340, "체력   지수 : " + std::to_string(player[1].hp));
    player[1].sp = (player[1].classtype == magic) ? player[1].mentality * 10 : 0;
    P(123, 360, "마법   지수 : " + std::to_string(player[1].sp));
    P(123, 380, "레       벨 : 1");
    SetColor(11);
}

static void Select_Ability2() {
    int transdata[4] = {0, 0, 0, 0};
    int pool = 40;
    int j = 17; // Represents current row for Agility, Accuracy, Luck

    SetColor(10); P(320, 225, "남아있는 지수 : ");
    SetColor(14);
    P(350, 256, "민첩성  ==>");
    P(350, 272, "정확성  ==>");
    P(350, 288, "행  운  ==>");

    auto Writing = [&](int px, int py, std::string s) {
        SetColor(1); 
        int width = (px < 400) ? 14 : 40; // cursor area (336) vs values area (456)
        Bar(px, py, px + width, py + 16);
        SetColor(15); P(px, py, s);
    };

    Writing(456, 225, "40");
    Writing(336, 272, " ");
    Writing(336, 256, "\x1a"); // chr(26) = ^Z, 파스칼 원본과 동일한 폰트 특수문자 커서
    for (int i = 0; i < 3; i++) Writing(456, 256 + i * 16, "0");

    while (true) {
        int c = ReadKeySync();
        int y1 = 0, x1 = 0;
        
        if (c == 72) y1 = -1;
        else if (c == 80) y1 = 1;
        else if (c == 75) x1 = -1;
        else if (c == 77) x1 = 1;
        else if (c == 13 && pool == 0) break; // Enter when pool is empty
        
        if (y1 != 0) {
            j += y1;
            if (j < 17 || j > 19) j -= y1;
            else {
                Writing(336, 256 + (j - y1 - 17) * 16, " ");
                Writing(336, 256 + (j - 17) * 16, "\x1a");
            }
        }
        
        if (x1 != 0) {
            transdata[j - 16] += x1;
            pool -= x1;
            if (transdata[j - 16] < 0 || transdata[j - 16] > 20 || pool < 0 || pool > 40) {
                transdata[j - 16] -= x1;
                pool += x1;
            } else {
                Writing(456, 225, std::to_string(pool));
                Writing(456, 256 + (j - 17) * 16, std::to_string(transdata[j - 16]));
            }
        }
    }

    player[1].agility = transdata[1];
    player[1].accuracy = transdata[2];
    player[1].luck = transdata[3];
    SetFont(0);
    SetColor(3); Bar(310, 160, 639, 400);
}

static void Put_Class() {
    const char* skill_str[2][6] = {
        {"베는  무기류  기술치", "찍는  무기류  기술치", "찌르는 무기류 기술치", "쏘는  무기류  기술치", "방패  사용    기술치", "맨손  전투    기술치"},
        {"공격 마법 능력", "변화 마법 능력", "치료 마법 능력", "특수 마법 능력", "초 자연력 능력", "소환 마법 능력"}
    };

    int data[7] = {0, 10, 10, 10, 10, 10, 10};
    int exist_point = 0;
    bool class_enable[8];
    bool aux_class_enable[8];

    Screen_Set();
    SetColor(11); // LightCyan
    std::string n_str(player[1].name, player[1].name_len);
    P(200, 20, "당신의 이름 : " + n_str);
    P(200, 40, "당신의 성별 : " + ReturnSex(1));
    P(200, 60, "당신의 계열 : " + ReturnClassType(1));

    SetColor(7); // LightGray
    for (int i = 1; i <= 6; i++) {
        P(60, 150 + (i - 1) * 20, skill_str[player[1].classtype][i - 1]);
    }

    int default_j = (player[1].classtype == sword) ? 1 : 3;
    for (int i = 1; i <= 7; i++) {
        aux_class_enable[i] = (i <= default_j);
        if (aux_class_enable[i]) SetColor(15); else SetColor(8);
        player[1]._class = i;
        P(60, 300 + (i - 1) * 20, std::to_string(i) + "." + ReturnClass(1));
    }

    auto Skill_Line = [&](int x_pos, int y_pos, bool MainLine) {
        int co = GetColor();
        if (MainLine) SetColor(14); else SetColor(7); // Yellow vs LightGray
        P(60, 150 + (y_pos - 1) * 20, skill_str[player[1].classtype][y_pos - 1]);
        P(230, 150 + (y_pos - 1) * 20, ":");
        SetColor(14);
        RectAngle(250, 157 + (y_pos - 1) * 20, 250 + x_pos * 5, 157 + (y_pos - 1) * 20);
        SetColor(1); // Blue clear
        RectAngle(250 + x_pos * 5, 157 + (y_pos - 1) * 20, 640, 157 + (y_pos - 1) * 20);
        SetColor(co);
    };

    int y_pos = 1;
    for (int i = 1; i <= 6; i++) {
        Skill_Line(data[i], i, (i == y_pos));
    }

    bool ok_sel = false;
    char selected_class = '1';

    while (!ok_sel) {
        int c = ReadKeySync();
        if (c == 77 && exist_point > 0) { data[y_pos]++; exist_point--; }
        else if (c == 75 && data[y_pos] > 0) { data[y_pos]--; exist_point++; }
        else if (c == 80) { Skill_Line(data[y_pos], y_pos, false); y_pos = (y_pos % 6) + 1; }
        else if (c == 72) { Skill_Line(data[y_pos], y_pos, false); y_pos = (y_pos == 1) ? 6 : y_pos - 1; }
        else if (c >= '1' && c <= '7') { selected_class = c; ok_sel = true; }

        Skill_Line(data[y_pos], y_pos, true);

        for (int i = 0; i < 6; i++) player[1].raw_skills[i] = data[i + 1];

        bool change_class = false;
        for (int i = 1; i <= 7; i++) {
            class_enable[i] = ReturnClassEnable(1, i);
            if (class_enable[i] != aux_class_enable[i]) change_class = true;
        }

        if (change_class) {
            for (int i = 1; i <= 7; i++) {
                if (class_enable[i]) SetColor(15); else SetColor(8);
                player[1]._class = i;
                P(60, 300 + (i - 1) * 20, std::to_string(i) + "." + ReturnClass(1));
                aux_class_enable[i] = class_enable[i];
            }
        }

        if (ok_sel && !class_enable[selected_class - '0']) ok_sel = false;
    }

    player[1]._class = selected_class - '0';
}

bool Save_Character() {
    Screen_Set();
    SetColor(15);
    
    std::string n_str(player[1].name, player[1].name_len);
    P(200, 20 + 20 * 0, "당신의 이름 : " + n_str);
    P(200, 20 + 20 * 1, "당신의 성별 : " + ReturnSex(1));
    P(200, 20 + 20 * 2, "당신의 계열 : " + ReturnClassType(1));
    P(200, 20 + 20 * 3, "당신의 계급 : " + ReturnClass(1));

    SetColor(10); // LightGreen
    P(150, 350, "당신이 만든 캐릭터에 만족 합니까 ?");
    SetColor(7);
    P(200, 375, "그렇습니다   아닙니다");

    int x = 1, y = 1;
    Choose(192, 371, 100, 26, 2, 1, x, y, false);
    if (x != 1) return false;

    for (int i = 2; i <= 7; i++) {
        std::memset(&player[i], 0, sizeof(player[i]));
    }

    player[1].potential_ac = (player[1].classtype == sword) ? 2 : 0;
    player[1].level = 1;
    player[1].poison = 0;
    player[1].unconscious = 0;
    player[1].dead = 0;
    player[1].hp = player[1].endurance * player[1].level * 10;
    player[1].sp = (player[1].classtype == magic) ? player[1].mentality * player[1].level * 10 : 0;
    player[1].ac = 0;
    player[1].experience = 0;
    player[1].potential_experience = 0;
    player[1].weapon = 0;
    player[1].shield = 0;
    player[1].armor = 0;
    player[1].wea_power = 2;
    player[1].shi_power = 0;
    player[1].arm_power = 0;

    std::memset(&party, 0, sizeof(party));
    party.year = 673;
    party.day = 326;
    party.hour = 16;
    party.min = 0;
    party.food = 0;
    party.gold = 0;
    party.arrow = 0;

    mapname = "Menace";
    Screen_Set();
    return true;
}

bool CreateCharacter() {
    bool done = false;
    while (!done) {
        Put_Name();
        Put_Sex();
        Select_Ability1();
        Select_Ability2();
        Put_Class();
        done = Save_Character();
    }
    return true;
}