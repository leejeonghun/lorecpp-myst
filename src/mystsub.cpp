#include "mystsub.h"
#include "uhanx.h"
#include "mystbatt.h"
#include "mystmenu.h"
#include "mystspec.h"
#include "mystent.h"
#include "mysttalk.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <algorithm>

// Global variable definitions
bool absolute_set_xy = false;
bool ebony = false;
bool Time_Watch = false;
int i, j, k;
byte b;
word h;
longint long_val;
char c, LoadNo;
std::string Josa, Mokjuk; 
std::string sexdata;
std::string s_val;
std::string m[11];
int pas_x, pas_y, pas_x1, pas_y1;
bool ok, Allright, AutoBattle, aux, LoadFont, Loading_SaveGame, AdLibOn, SoundOn;
bool Cruel;
ScrollTypes scrolltype = normal;
PositionType position = town;
Map_Header_Type Map_Header;

byte* encounter = nullptr;
byte* maxenemy = nullptr;
pathstr existfile;
ImageDirection _IMAGE_DIRECTION = _DIRECT;
byte scrollform, scrollcolor, putstyle, yline, face, Rest_Time;
int person;
bool QuitPlay = false;

std::vector<FontType> font(56);
std::vector<FontType> chara(56);
MapType* map = nullptr;
std::string mapname;
byte battle[7][4];
integer watch_year, watch_day;
byte watch_hour, watch_min, watch_sec, time_event_no;
LorePlayer party;
Lore player[8];
byte enemynumber;
EnemyData2 enemy[9];

extern const EnemyData1 EnemyData_Const[75] = {
    {"", 0, 0, 0, {0, 0}, 0, {0, 0}, 0, 0, 0, 0, 0}, 
    {"저승사자", 12, 0, 10, {100, 0}, 10, {10, 0}, 2, 1, 0, 0, 7},
    {"생령", 0, 12, 10, {50, 50}, 0, {10, 16}, 0, 0, 4, 8, 8},
    {"좀비", 13, 0, 30, {20, 20}, 10, {14, 0}, 4, 0, 0, 0, 8},
    {"흡혈귀", 12, 0, 10, {30, 30}, 15, {15, 0}, 3, 2, 0, 0, 7},
    {"다크 소서러", 0, 0, 4, {100, 0}, 0, {0, 0}, 0, 0, 0, 64, 6},
    {"불의 정령", 20, 0, 10, {0, 0}, 10, {15, 0}, 0, 0, 0, 0, 10},
    {"물의 정령", 9, 0, 8, {100, 0}, 11, {13, 0}, 0, 0, 0, 0, 10},
    {"바람의 정령", 8, 0, 6, {40, 40}, 15, {14, 0}, 0, 0, 0, 0, 10},
    {"땅의 정령", 12, 0, 30, {0, 0}, 10, {12, 0}, 0, 0, 0, 0, 10},
    {"다크 힐러", 0, 0, 5, {30, 30}, 14, {0, 0}, 0, 0, 0, 64, 10},
    {"싸이크롭스", 30, 0, 30, {10, 10}, 20, {20, 0}, 4, 0, 0, 0, 13},
    {"켈베로스", 25, 15, 20, {0, 0}, 10, {30, 0}, 6, 0, 4, 0, 12},
    {"프랑켄슈타인", 60, 0, 20, {0, 0}, 10, {10, 0}, 5, 0, 0, 0, 12},
    {"드라큘라", 25, 20, 20, {0, 50}, 17, {20, 20}, 5, 0, 4, 0, 13},
    {"투명인간", 18, 0, 15, {100, 100}, 20, {15, 0}, 4, 0, 0, 0, 8},
    {"강도", 11, 0, 13, {0, 0}, 13, {13, 0}, 3, 0, 0, 0, 3},
    {"폴록스", 13, 10, 10, {0, 0}, 10, {19, 18}, 3, 2, 2, 0, 7},
    {"오크주민", 8, 0, 8, {0, 0}, 8, {8, 0}, 1, 0, 0, 0, 1},
    {"오크병사", 10, 0, 12, {5, 5}, 8, {9, 0}, 2, 0, 0, 0, 3},
    {"오크매직유저", 7, 10, 9, {0, 0}, 9, {9, 10}, 1, 0, 2, 0, 4},
    {"오크전사", 12, 0, 13, {25, 25}, 13, {10, 0}, 3, 0, 0, 0, 5},
    {"오크아키메이지", 0, 15, 10, {40, 40}, 8, {0, 15}, 3, 4, 4, 0, 8},
    {"오크아키몽크", 17, 0, 16, {10, 10}, 15, {15, 0}, 5, 0, 0, 0, 8},
    {"오크킹", 19, 0, 17, {50, 50}, 11, {16, 0}, 5, 0, 0, 0, 10},
    {"트롤주민", 9, 0, 6, {0, 0}, 9, {9, 0}, 1, 0, 0, 0, 2},
    {"트롤병사", 11, 0, 6, {0, 0}, 9, {12, 0}, 2, 0, 0, 0, 3},
    {"트롤용사", 13, 0, 12, {0, 0}, 9, {13, 0}, 2, 0, 0, 0, 4},
    {"트롤사냥꾼", 16, 0, 14, {0, 0}, 19, {20, 0}, 2, 2, 0, 0, 6},
    {"트롤강령술사", 0, 10, 10, {70, 70}, 5, {0, 10}, 1, 0, 3, 4, 8},
    {"트롤암살자", 12, 0, 13, {50, 50}, 20, {20, 0}, 5, 3, 0, 16, 11},
    {"트롤투사", 13, 0, 12, {0, 0}, 9, {15, 0}, 2, 0, 0, 0, 5},
    {"트롤기사", 15, 0, 18, {0, 0}, 14, {14, 0}, 4, 0, 0, 0, 7},
    {"트롤위저드", 0, 18, 17, {20, 20}, 8, {0, 18}, 2, 0, 5, 0, 9},
    {"트롤광전사", 19, 15, 19, {40, 40}, 12, {19, 17}, 5, 0, 3, 0, 12},
    {"코볼트병사", 9, 0, 9, {0, 0}, 9, {9, 0}, 2, 0, 0, 0, 7},
    {"코볼트투사", 9, 0, 9, {10, 20}, 9, {12, 0}, 2, 0, 0, 0, 8},
    {"코볼트매직유저", 0, 10, 9, {0, 10}, 8, {0, 12}, 2, 1, 3, 0, 8},
    {"경비대장", 12, 0, 12, {40, 80}, 12, {20, 0}, 4, 3, 0, 0, 9},
    {"근위기사", 12, 0, 25, {20, 40}, 13, {17, 0}, 4, 2, 0, 0, 10},
    {"코볼트위저드", 0, 20, 15, {10, 50}, 15, {0, 20}, 3, 0, 4, 0, 11},
    {"코볼트성전사", 17, 18, 28, {20, 40}, 16, {20, 20}, 4, 2, 2, 0, 12},
    {"소환주술사", 0, 17, 10, {60, 100}, 8, {0, 17}, 2, 0, 4, 4, 12},
    {"대제사장", 0, 20, 15, {60, 255}, 16, {0, 20}, 3, 0, 5, 64, 13},
    {"근위기사장", 19, 19, 30, {45, 80}, 15, {20, 20}, 5, 0, 3, 0, 13},
    {"코볼트킹", 0, 25, 25, {50, 90}, 16, {0, 30}, 4, 0, 6, 69, 15},
    {"미미크", 12, 0, 10, {40, 40}, 20, {20, 0}, 2, 0, 0, 0, 4},
    {"얼음 엘리멘탈", 0, 0, 20, {30, 30}, 0, {0, 0}, 0, 0, 0, 64, 10},
    {"드라코니안", 30, 20, 30, {40, 40}, 18, {18, 18}, 4, 2, 3, 0, 12},
    {"위병 드라콘", 30, 20, 30, {40, 40}, 18, {18, 18}, 4, 2, 3, 0, 13},
    {"가드 드라콘", 50, 0, 60, {60, 70}, 20, {20, 0}, 5, 2, 0, 0, 14},
    {"기사 드라콘", 30, 20, 30, {40, 40}, 18, {18, 18}, 4, 2, 3, 0, 15},
    {"소환사 드라콘", 20, 20, 25, {30, 30}, 20, {18, 18}, 3, 0, 5, 4, 16},
    {"위저드 드라콘", 0, 30, 30, {50, 50}, 20, {0, 20}, 4, 0, 6, 0, 17},
    {"광란자 드라콘", 50, 0, 60, {60, 70}, 20, {20, 0}, 5, 2, 0, 0, 18},
    {"프로스트 드라콘", 30, 20, 30, {100, 90}, 18, {18, 18}, 4, 2, 4, 0, 20},
    {"성전사 드라콘", 45, 25, 30, {50, 50}, 18, {20, 20}, 6, 3, 5, 0, 24},
    {"크로매틱 드라콘", 30, 20, 30, {80, 80}, 18, {18, 18}, 4, 2, 4, 128, 26},
    {"아키드라코니안", 40, 30, 30, {85, 85}, 20, {20, 20}, 7, 3, 5, 7, 28},
    {"드라코니안킹", 40, 30, 30, {90, 90}, 30, {25, 25}, 8, 0, 6, 103, 30},
    {"오크의 원혼", 20, 0, 30, {50, 50}, 25, {20, 0}, 4, 0, 0, 128, 25},
    {"트롤의 원혼", 20, 0, 30, {75, 75}, 25, {30, 0}, 5, 2, 0, 128, 25},
    {"코볼트의 원혼", 0, 30, 30, {50, 50}, 20, {0, 30}, 6, 0, 6, 128, 25},
    {"드라콘의 원혼", 40, 30, 30, {90, 90}, 30, {25, 25}, 8, 0, 6, 128, 25},
    {"배리언트 주민", 8, 0, 8, {0, 0}, 8, {8, 0}, 1, 0, 0, 0, 1},
    {"배리언트 기사", 7, 0, 9, {0, 0}, 9, {12, 0}, 2, 0, 0, 0, 3},
    {"카미너스", 9, 0, 9, {0, 0}, 9, {9, 0}, 2, 0, 0, 0, 7},
    {"로어성 병사", 8, 0, 8, {0, 0}, 8, {8, 0}, 1, 0, 0, 0, 2},
    {"로어 마법사", 0, 16, 12, {0, 0}, 12, {0, 12}, 2, 0, 4, 0, 7},
    {"로어 기사", 9, 0, 9, {0, 0}, 9, {9, 0}, 2, 0, 0, 0, 7},
    {"경비대장", 40, 30, 30, {85, 85}, 20, {20, 20}, 7, 3, 5, 0, 15},
    {"경호기사", 30, 20, 30, {40, 40}, 18, {18, 18}, 4, 2, 3, 0, 10},
    {"경호대장", 40, 30, 30, {85, 85}, 20, {20, 20}, 7, 3, 5, 0, 22},
    {"로드안", 200, 200, 200, {200, 200}, 20, {50, 50}, 20, 0, 6, 103, 60},
    {"로드안", 60, 40, 60, {160, 160}, 20, {40, 40}, 20, 0, 6, 103, 60}
};

uint8_t Current_Palette[16][3] = {
    { 0, 0, 0},{ 0, 0,42},{ 0,42, 0},{ 0,42,42},
    {42,10,10},{42, 0,42},{42,21, 0},{42,42,42},
    {21,21,21},{10,10,60},{10,60,10},{10,60,60},
    {63,21,21},{60,10,60},{60,60,10},{60,60,60}
};

std::string FromPas(const char* buf, uint8_t len) { return std::string(buf, len); }

void Restore_Palette() {
    const uint8_t RGB_data[16][3] = { 
        { 0, 0, 0},{ 0, 0,42},{ 0,42, 0},{ 0,42,42}, 
        {42,10,10},{42, 0,42},{42,21, 0},{42,42,42}, 
        {21,21,21},{10,10,60},{10,60,10},{10,60,60}, 
        {63,21,21},{60,10,60},{60,60,10},{60,60,60} 
    };
    for (int i = 0; i < 16; i++) {
        // 현재 팔레트 상태 저장
        Current_Palette[i][0] = RGB_data[i][0];
        Current_Palette[i][1] = RGB_data[i][1];
        Current_Palette[i][2] = RGB_data[i][2];
        SetPalette(i, RGB_data[i][0], RGB_data[i][1], RGB_data[i][2]);
    }
}

void Clear() { int old = GetColor(); SetColor(8); Bar(245, 21, 610, 288); SetColor(old); yline = 30; hany = 32; }

void PressAnyKey() {
    SetColor(14); HPrintXY4Select(260, 272, "아무키나 누르십시오 ...");
    UpdateScreen();
    SDL_Event ev_flush; while(SDL_PollEvent(&ev_flush)) {};
    SDL_Event ev_any; bool pressed = false;
    while(!pressed) { 
        while(SDL_PollEvent(&ev_any)) { 
            if(ev_any.type == SDL_KEYDOWN || ev_any.type == SDL_QUIT) pressed = true; 
        } 
        SDL_Delay(10); 
    }
    Clear();
    UpdateScreen();
}

void Screen_Fade_Out(int d) {
    if (d <= 0) d = 1;
    // 약 60프레임(16ms) 간격으로 페이드아웃 단계를 나눔
    int steps = d / 16;
    if (steps < 1) steps = 1;
    int delay_ms = d / steps;

    for (int step = steps; step >= 0; step--) {
        float ratio = (float)step / steps;
        for (int i = 0; i < 16; i++) {
            // 원본 색상 값에서 ratio(1.0 -> 0.0)를 곱하여 점점 어둡게
            uint8_t r = (uint8_t)(Current_Palette[i][0] * ratio);
            uint8_t g = (uint8_t)(Current_Palette[i][1] * ratio);
            uint8_t b = (uint8_t)(Current_Palette[i][2] * ratio);
            SetPalette(i, r, g, b);
        }
        UpdateScreen(); // 변경된 팔레트 화면 갱신
        SDL_Delay(delay_ms);
    }
}

void Screen_Fade_In(int d) {
    if (d <= 0) d = 1;
    // 약 60프레임(16ms) 간격으로 페이드인 단계를 나눔
    int steps = d / 16;
    if (steps < 1) steps = 1;
    int delay_ms = d / steps;

    for (int step = 0; step <= steps; step++) {
        float ratio = (float)step / steps;
        for (int i = 0; i < 16; i++) {
            // 원본 색상 값에서 ratio(0.0 -> 1.0)를 곱하여 점점 밝게
            uint8_t r = (uint8_t)(Current_Palette[i][0] * ratio);
            uint8_t g = (uint8_t)(Current_Palette[i][1] * ratio);
            uint8_t b = (uint8_t)(Current_Palette[i][2] * ratio);
            SetPalette(i, r, g, b);
        }
        UpdateScreen(); // 변경된 팔레트 화면 갱신
        SDL_Delay(delay_ms);
    }
}

void OverLap(int d, bool ch) { Store_Scroll(ch); Screen_Fade_In(d); Scroll(ch); }

void Image(int X, int Y, void* BitMap, bool Darkness, PutMode mode) {
    if(!BitMap) return; uint8_t* data = (uint8_t*)BitMap;
    uint16_t w_minus_1 = data[0] | (data[1] << 8);
    uint16_t h_minus_1 = data[2] | (data[3] << 8);
    int w = w_minus_1 + 1; int h = h_minus_1 + 1;
    if (w <= 0 || h <= 0 || w > 100 || h > 100) return;
    int bpr = (w + 7) / 8;
    extern uint8_t g_IndexBuffer[640 * 480];
    for(int r=0; r<h; r++) {
        for(int c=0; c<w; c++) {
            int bit_idx = 7 - (c % 8);
            int offset = 4 + r * (bpr * 4) + (c / 8);
            uint8_t p3 = (data[offset + 0 * bpr] >> bit_idx) & 1;
            uint8_t p2 = (data[offset + 1 * bpr] >> bit_idx) & 1;
            uint8_t p1 = (data[offset + 2 * bpr] >> bit_idx) & 1;
            uint8_t p0 = (data[offset + 3 * bpr] >> bit_idx) & 1;
            uint8_t color = (p3 << 3) | (p2 << 2) | (p1 << 1) | p0;

            // 원본 파스칼 ASM과 동일하게 Bit 0 (Blue Plane)만 살리고 나머지는 0으로 만듦
            if (Darkness) color = color & 1;
            
            int screen_x = X * 24 + c;
            int screen_y = Y * 24 + r;
            if (screen_x < 0 || screen_x >= 640 || screen_y < 0 || screen_y >= 480) continue;
            
            uint8_t old_color = g_IndexBuffer[screen_y * 640 + screen_x];
            uint8_t final_color = color;
            
            if (mode == AND_PUT) final_color = old_color & color;
            else if (mode == OR_PUT) final_color = old_color | color;
            else if (mode == XOR_PUT) final_color = old_color ^ color;
            
            PutPixel(screen_x, screen_y, final_color);
        }
    }
}

byte GetMap(byte pas_x, byte pas_y) { 
    if(pas_x < 1 || pas_x > Map_Header.xmax || pas_y < 1 || pas_y > Map_Header.ymax) return 0; 
    return map->data[pas_x-1][pas_y-1]; 
}

byte Real_Map(byte pas_x, byte pas_y) { 
    if(pas_x < 1 || pas_x > Map_Header.xmax || pas_y < 1 || pas_y > Map_Header.ymax) return 0; 
    return map->data[pas_x-1][pas_y-1] & 0x7F; 
}

void PutMap(byte pas_x, byte pas_y, byte val) { 
    if(pas_x < 1 || pas_x > 150 || pas_y < 1 || pas_y > 120) return; 
    map->data[pas_x-1][pas_y-1] = (map->data[pas_x-1][pas_y-1] & 0x80) | val; 
}

void PutSprite(int x, int y, int number) { 
    Image(x, y, &chara[number+28], false, AND_PUT); 
    Image(x, y, &chara[number], false, OR_PUT); 
    UpdateScreen(); 
}
void PutTile(int x, int y, int number) { Image(x, y, &font[number], false, COPY_PUT); UpdateScreen(); }

void setscrolltype(ScrollTypes form) {
    scrolltype = form;
    if (form == normal) {
        putstyle = COPY_PUT;
        party.etc[12] = 0;
    }
    else {
        putstyle = OR_PUT;
    }

    switch (form) {
    case snow:
        scrollform = 1; // SolidFill
        scrollcolor = 1; // Blue
        party.etc[12] = 1;
        break;
    case rain:
        scrollform = 3; // LtSlashFill
        scrollcolor = 1; // Blue
        party.etc[12] = 2;
        break;
    case autumn:
        scrollform = 1; // SolidFill
        scrollcolor = 6; // Brown
        party.etc[12] = 3;
        break;
    case wilderness:
        scrollform = 9; // InterleaveFill
        scrollcolor = 1; // Blue
        party.etc[12] = 4;
        break;
    case strongrain:
        scrollform = 6; // HatchFill
        scrollcolor = 1; // Blue
        party.etc[12] = 5;
        break;
    default:
        break;
    }
}

void Clear_View_Screen() {
    int old_color = GetColor();
    SetColor(8); // SolidFill, DarkGray
    Bar(30, 305, 31, 306);
    Bar(30, 305, 240, 479);
    SetColor(old_color);
}

void Scroll(bool character) {
    SDL_Event ev_flush; while (SDL_PollEvent(&ev_flush)) {};
    uint8_t buf[300];
    int AuxX = pas_x - 5;
    int AuxY = pas_y - 6;
    bool Dark = (position == den);
    bool Moon_Light = ((party.day % 30 + 1) >= 10 && (party.day % 30 + 1) <= 20);
    if (position == den) Moon_Light = false;
    if (position == town) Moon_Light = true;

    int x_wide = 1, y_wide = 1;
    if (!(party.hour >= 7 && party.hour <= 17)) {
        Dark = true;
        if (party.hour == 18) {
            if (party.min <= 19) { x_wide = 4; y_wide = 4; }
            else if (party.min <= 39) { x_wide = 3; y_wide = 3; }
            else { x_wide = 2; y_wide = 2; }
        }
        else if (party.hour == 6) {
            if (party.min <= 19) { x_wide = 2; y_wide = 2; }
            else if (party.min <= 39) { x_wide = 3; y_wide = 3; }
            else { x_wide = 4; y_wide = 4; }
        }
        else { x_wide = 1; y_wide = 1; }
    }
    if (position == den) { x_wide = 1; y_wide = 1; }

    // Light source (etc[1]) logic
    if (Dark && (party.etc[1] > 0)) {
        if (party.etc[1] <= 2) { if (x_wide < 2) { x_wide = 2; y_wide = 2; } }
        else if (party.etc[1] <= 4) { if (x_wide < 3) { x_wide = 3; y_wide = 3; } }
        else { if (x_wide < 4) { x_wide = 3; y_wide = 3; Moon_Light = true; } }
    }

    // Prepare character drawing buffer
    if (character) {
        byte center_tile = Real_Map(pas_x, pas_y);
        std::memcpy(buf, &font[center_tile], 294);
        for (int i = 4; i < 294; i++) buf[i] = (buf[i] & chara[face + 28].data[i]) | chara[face].data[i];
    }

    bool repeat_ebony = false;
    do {
        repeat_ebony = false;
        if (ebony && (party.etc[1] == 0)) {
            Moon_Light = false;
            Dark = true;
            character = false;
            x_wide = 0; y_wide = 0;
        }

        // Fill background based on scroll type
        if (scrolltype != normal) {
            SetColor(scrollcolor);
            Bar(24, 24, 239, 287);
        }
        else {
            SetColor(0); // SolidFill, Black
            Bar(24, 24, 239, 287);
        }

        for (int j_idx = 1; j_idx <= 11; j_idx++) {
            for (int i_sub = 1; i_sub <= 9; i_sub++) {
                bool Darkness = true;
                if (i_sub >= (5 - x_wide) && i_sub <= (5 + x_wide) && j_idx >= (6 - y_wide) && j_idx <= (6 + y_wide)) Darkness = false;
                if (!Dark) Darkness = false;
                if (ebony && (party.etc[1] == 0)) Darkness = true;

                int tx = i_sub + AuxX; int ty = j_idx + AuxY;

                if (i_sub == 5 && j_idx == 6 && character) {
                    Image(i_sub, j_idx, buf, false, COPY_PUT);
                }
                else if (ty < 1 || ty > Map_Header.ymax) {
                    Image(i_sub, j_idx, &font[0], Darkness, (PutMode)putstyle);
                }
                else if (tx >= 1 && tx <= Map_Header.xmax) {
                    byte tile_idx = Real_Map(tx, ty);
                    if ((map->data[tx - 1][ty - 1] & 0x80) > 0) {
                        Image(i_sub, j_idx, &font[tile_idx], false, (PutMode)putstyle);
                    }
                    else if (Moon_Light || !Darkness) {
                        Image(i_sub, j_idx, &font[tile_idx], Darkness, (PutMode)putstyle);
                    }
                    else {
                        Image(i_sub, j_idx, &font[55], Darkness, (PutMode)putstyle);
                    }
                }
            }
        }

        if (ebony && (party.etc[1] > 0)) {
            party.etc[1] = 0;
            UpdateScreen();
            SDL_Delay(250);
            repeat_ebony = true;
        }
    } while (repeat_ebony);

    UpdateScreen();
}

void Silent_Scroll() { bool old = SoundOn; SoundOn = false; 
    Scroll(true);
    
    // 3D Border Lines from original Set_All
    SetColor(15); // White
    Line(24*10+1, 24*1-1, 24*10+1, 24*12+1);
    SetColor(7); // LightGray
    Line(24*1-1, 24*12+1, 24*10+1, 24*12+1);
    
    SetColor(11);
 SoundOn = old; }
void Store_Scroll(bool ch) { _IMAGE_DIRECTION = _STORE; aux = SoundOn; SoundOn = false; Scroll(ch); SoundOn = aux; _IMAGE_DIRECTION = _DIRECT; }

bool exist(int p_idx) { 
    if(p_idx < 1 || p_idx > 7) return false; 
    Lore& p = player[p_idx]; 
    return p.name_len > 0 && p.unconscious == 0 && p.dead == 0 && p.hp > 0; 
}

void PlusTime(int _h, int _m, int _s) { 
    party.sec += _s; while(party.sec > 59) { party.sec -= 60; party.min++; } 
    party.min += _m; while(party.min > 59) { party.min -= 60; party.hour++; } 
    party.hour += _h; while(party.hour > 23) { party.hour -= 24; party.day++; } 
    while(party.day > 359) { party.day -= 360; party.year++; } 
}

std::string ReturnSJosa(std::string s) { return HasBatchim(s) ? "은" : "는"; }
std::string ReturnBJosa(std::string s) { return HasBatchim(s) ? "이" : "가"; }
std::string ReturnOJosa(std::string s) { return HasBatchim(s) ? "을" : "를"; }

void Print(int color, std::string s1) { 
    SetColor(color); hany += 16; if (hany > 280) hany = 32; 
    HGotoXY(260, hany); HPrint(s1.c_str()); 
}

void message(int color, std::string s_msg) { Clear(); Print(color, s_msg); UpdateScreen(); }

void asyouwish() { message(7, " 당신이 바란다면 ..."); PressAnyKey(); }
void notenoughmoney() { message(7, " 당신은 충분한 돈이 없습니다."); PressAnyKey(); }
void thankyou() { message(7, " 매우 고맙습니다."); PressAnyKey(); }

static void Internal_LoadFNT(std::string filename, std::vector<FontType>& target) {
    std::string upper_filename = filename;
    for (auto &c : upper_filename) c = toupper((unsigned char)c);
    std::ifstream f("assets/" + upper_filename, std::ios::binary);
    if(!f) return;
    f.seekg(0, std::ios::end); size_t size = f.tellg(); f.seekg(0, std::ios::beg);
    int bytes_per_glyph = (int)size / 56;
    for(int i_idx=0; i_idx<56; i_idx++) { 
        std::memset(target[i_idx].data, 0, 300); 
        f.read((char*)target[i_idx].data, std::min(bytes_per_glyph, 300)); 
    }
}

static void Internal_LoadPAL(std::string filename) {
    std::string upper_filename = filename;
    for (auto &c : upper_filename) c = toupper((unsigned char)c);
    std::ifstream f("assets/" + upper_filename, std::ios::binary);
    if(!f) return;
    uint8_t rgb[3];
    for(int i_idx=0; i_idx<16; i_idx++) {
        f.read((char*)rgb, 3);
        // 현재 팔레트 상태 저장
        Current_Palette[i_idx][0] = rgb[0];
        Current_Palette[i_idx][1] = rgb[1];
        Current_Palette[i_idx][2] = rgb[2];
        SetPalette(i_idx, rgb[0], rgb[1], rgb[2]);
    }
}

// ---------------------------------------------------------------------------
// 조합형(Johab) -> UTF-8 한글 역변환기 (Load 용도)
// ---------------------------------------------------------------------------
static const int JohabChoToIdx[32] = {
    -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

static const int JohabJungToIdx[32] = {
    -1, -1, -1, 0, 1, 2, 3, 4, -1, -1, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
        20, 18, 19, -1, 17,
        -1, -1, -1, -1, -1
};

static const int JohabJongToIdx[32] = {
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    -1, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
    -1, -1
};

static void ConvertToUTF8(const char* src, int src_len, char* dst, uint8_t& dst_len) {
    int in_idx = 0;
    int out_idx = 0;
    while (in_idx < src_len) {
        uint8_t c1 = (uint8_t)src[in_idx];
        if (c1 & 0x80) {
            if (in_idx + 1 < src_len) {
                uint8_t c2 = (uint8_t)src[in_idx + 1];
                uint16_t code = (c1 << 8) | c2;
                int cho = (code >> 10) & 0x1F;
                int jung = (code >> 5) & 0x1F;
                int jong = code & 0x1F;
                int cho_idx = JohabChoToIdx[cho];
                int jung_idx = JohabJungToIdx[jung];
                int jong_idx = JohabJongToIdx[jong];
                if (cho_idx != -1 && jung_idx != -1 && jong_idx != -1) {
                    uint32_t unicode = 0xAC00 + (cho_idx * 21 * 28) + (jung_idx * 28) + jong_idx;
                    if (out_idx + 3 <= 17) {
                        dst[out_idx++] = 0xE0 | ((unicode >> 12) & 0x0F);
                        dst[out_idx++] = 0x80 | ((unicode >> 6) & 0x3F);
                        dst[out_idx++] = 0x80 | (unicode & 0x3F);
                    }
                }
                else {
                    if (out_idx + 2 <= 17) {
                        dst[out_idx++] = c1;
                        dst[out_idx++] = c2;
                    }
                }
                in_idx += 2;
            }
            else {
                if (out_idx < 17) dst[out_idx++] = c1;
                in_idx++;
            }
        }
        else {
            if (out_idx < 17) dst[out_idx++] = c1;
            in_idx++;
        }
    }
    dst_len = (uint8_t)out_idx;
}
// ---------------------------------------------------------------------------

void Load() {
    if (!LoadFont) {
        if (Loading_SaveGame) {
            std::string filename = "PLAYER" + std::string(1, LoadNo) + ".DAT";
            std::ifstream f("assets/" + filename, std::ios::binary);
            if (f.is_open()) {
                DOS_LorePlayer dp;
                f.read((char*)&dp, sizeof(dp));
                party.year = dp.year; party.day = dp.day;
                party.hour = dp.hour; party.min = dp.min; party.sec = dp.sec; party.food = dp.food;
                party.gold = dp.gold; party.arrow = dp.arrow;

                for (int i = 0; i < 10; i++) party.item[i + 1] = dp.item[i];
                for (int i = 0; i < 10; i++) party.crystal[i + 1] = dp.crystal[i];
                for (int i = 0; i < 100; i++) party.etc[i + 1] = dp.etc[i];
                for (int i = 0; i < 6; i++) {
                    for (int j = 0; j < 10; j++) party.BackPack.data[i][j] = dp.backpack[i][j];
                }

                for (int i = 1; i <= 7; i++) {
                    DOS_Lore dl;
                    f.read((char*)&dl, sizeof(dl));
                    // 백업 필드 초기화
                    player[i].name_hidden_len = 0;
                    std::memset(player[i].name_hidden, 0, 17);
                    std::memset(player[i].name, 0, 17);

                    if (dl.name.len == 0 && (uint8_t)dl.name.text[8] > 0) {
                        // 숨김 상태로 저장된 경우:
                        // Pascal 원본의 name[9..17] 트릭 — text[8]=Johab백업길이, text[9..]=Johab백업데이터
                        // Pascal 백업 영역은 name[9..17] = 최대 8바이트 데이터(name[10..17])이므로
                        // 원본 게임 버그로 저장된 길이가 8을 초과할 수 있음 → 8로 캡핑
                        uint8_t johab_backup_len = std::min((uint8_t)dl.name.text[8], (uint8_t)8);
                        player[i].name_len = 0;
                        ConvertToUTF8(dl.name.text + 9, johab_backup_len,
                                      player[i].name_hidden, player[i].name_hidden_len);
                        // null termination 보장 (ConvertToUTF8는 길이만 설정하고 '\0'을 추가하지 않음)
                        if (player[i].name_hidden_len < 17)
                            player[i].name_hidden[player[i].name_hidden_len] = '\0';
                    } else {
                        // 일반 상태
                        // Johab 소스를 문자 경계 단위로 순회하여 유효 길이 산출:
                        // 9바이트 이후 non-ASCII(한글) 문자가 시작되면 해당 위치에서 잘라냄.
                        // ASCII 문자(스페이스, 영문 등)는 9바이트 이후에도 허용하므로
                        // "안타레스 Jr." 같은 이름은 전체가 정상 표시됨.
                        uint8_t effective_len = dl.name.len;
                        {
                            uint8_t pos = 0;
                            while (pos < dl.name.len) {
                                uint8_t b = (uint8_t)dl.name.text[pos];
                                uint8_t char_bytes = (b & 0x80) ? 2 : 1;
                                if (pos >= 9 && (b & 0x80)) {
                                    // 9바이트 이후에 non-ASCII 문자 등장 → 여기서 잘라냄
                                    effective_len = pos;
                                    break;
                                }
                                pos += char_bytes;
                            }
                        }
                        player[i].name_len = 0;
                        ConvertToUTF8(dl.name.text, effective_len, player[i].name, player[i].name_len);
                        // null termination 보장
                        if (player[i].name_len < 17)
                            player[i].name[player[i].name_len] = '\0';
                    }
                    player[i].sex = dl.sex; player[i]._class = dl._class;
                    player[i].strength = dl.strength; player[i].mentality = dl.mentality;
                    player[i].concentration = dl.concentration; player[i].endurance = dl.endurance;
                    player[i].resistance = dl.resistance; player[i].agility = dl.agility;
                    player[i].accuracy = dl.accuracy; player[i].luck = dl.luck; player[i].poison = dl.poison;
                    player[i].unconscious = dl.unconscious; player[i].dead = dl.dead;
                    player[i].hp = dl.hp; player[i].sp = dl.sp;
                    player[i].ac = dl.ac; player[i].level = dl.level;
                    player[i].experience = dl.experience; player[i].potential_experience = dl.potential_experience;
                    player[i].weapon = dl.weapon; player[i].shield = dl.shield; player[i].armor = dl.armor;
                    player[i].potential_ac = dl.potential_ac; player[i].wea_power = dl.wea_power;
                    player[i].shi_power = dl.shi_power; player[i].arm_power = dl.arm_power;
                    player[i].classtype = dl.classtype;
                    std::memcpy(player[i].raw_skills, dl.raw_skills, 6);
                }
            }
            else {
                const std::string filename = "PLAYER" + std::string(1, LoadNo) + ".DAT";
                const std::string message = "'" + filename + "'" + " not found.\nYou need to CREATE CHARACTER.";
                SDL_ShowSimpleMessageBox(
                    SDL_MESSAGEBOX_ERROR,
                    "Fatal Error",
                    message.c_str(),
                    NULL
                );
                exit(-1);
            }
        }
        Internal_LoadFNT("CHARA.FNT", chara);
    }

    std::ifstream fmap;
    bool loaded_save_map = false;

    // 1. 이어하기 상태일 때 SAVE[LoadNo].M 파일을 로드
    if (Loading_SaveGame && !LoadFont) {
        std::string save_file = "SAVE" + std::string(1, LoadNo) + ".M";
        fmap = std::ifstream("assets/" + save_file, std::ios::binary);

        if (fmap) {
            fmap.read((char*)&Map_Header, sizeof(Map_Header));

            mapname = std::string(Map_Header.ID, Map_Header.ID_len);
            pas_x = Map_Header.startX;
            pas_y = Map_Header.startY;

            fmap.read((char*)map, sizeof(MapType));
            loaded_save_map = true;
        }
    }

    // 2. 세이브 파일이 없거나, 일반적인 맵 이동 시 해당 맵 파일을 로드
    if (!loaded_save_map) {
        std::string upper_mapname = mapname;
        for (auto &c : upper_mapname) c = toupper((unsigned char)c);
        fmap = std::ifstream("assets/" + upper_mapname + ".M", std::ios::binary);

        if (fmap) {
            fmap.read((char*)&Map_Header, sizeof(Map_Header));

            // 맵 초기화
            std::memset(map->data, 0, sizeof(MapType));

            for (int y_idx = 0; y_idx < Map_Header.ymax; y_idx++) {
                for (int x_idx = 0; x_idx < Map_Header.xmax; x_idx++) {
                    fmap.read((char*)&map->data[x_idx][y_idx], 1);
                }
            }
            if (!absolute_set_xy) {
                // 원본 파스칼 조건 완벽 반영
                if ((Map_Header.startX != 255 || Map_Header.startY != 255) && Map_Header.Tile_Type != ground) {
                    pas_x = Map_Header.startX;
                    pas_y = Map_Header.startY;
                }
            }
        }
    }

    position = (PositionType)Map_Header.Tile_Type;
    // ---------------------------------------------------------------
    // [BGM] Pascal 원본 Load 프로시져의 BGM 파일명 설정 완벽 복원
    // position에 따라 existfile(기본 BGM)을 결정하고 QuitPlay=true로
    // 메인 루프에 BGM 교체 신호를 보낸다.
    //   town   -> Music2.Bgm
    //   ground -> Music3.Bgm
    //   den    -> Music4.Bgm
    //   keep   -> Music5.Bgm
    // ---------------------------------------------------------------
    QuitPlay = true;
    switch (position) {
        case town:   existfile = "MUSIC2.BGM"; break;
        case ground: existfile = "MUSIC3.BGM"; break;
        case den:    existfile = "MUSIC4.BGM"; break;
        case keep:   existfile = "MUSIC5.BGM"; break;
    }

    std::string base_name = "";
    if (position == town) base_name = "TOWN";
    else if (position == ground) base_name = "GROUND";
    else if (position == den) base_name = "DEN";
    else if (position == keep) base_name = "KEEP";

    if (!base_name.empty()) {
        Internal_LoadFNT(base_name + ".FNT", font);
        Internal_LoadPAL(base_name + ".PAL");
    }

    // 타일 55번 비우기 (데이터 영역만 초기화하여 Header Size 파괴 방지)
    for (int i_idx = 4; i_idx < 294; i_idx++) font[55].data[i_idx] = 0;

    // --- Special Font Loading ---
    // 파스칼 원작의 특정 맵 런타임 타일 조합 기능 완벽 복구
    auto UpperMapName = [](std::string str) {
        for (auto& c : str) c = std::toupper((unsigned char)c);
        return str;
        };
    std::string uMap = UpperMapName(mapname);

    if (uMap == "ORCTOWN") {
        std::vector<FontType> dec(56); Internal_LoadFNT("DECORATE.FNT", dec);
        for (int i = 4; i < 294; i++) {
            font[53].data[i] = (font[43].data[i] & dec[9].data[i]) | dec[8].data[i];
            font[54].data[i] = (font[43].data[i] & dec[11].data[i]) | dec[10].data[i];
            font[34].data[i] = (font[43].data[i] & dec[13].data[i]) | dec[12].data[i];
        }
    }
    else if (uMap == "GROUND3") {
        std::memcpy(font[21].data, font[13].data, 294);
    }
    else if (uMap == "VESPER") {
        std::vector<FontType> dec(56); Internal_LoadFNT("DECORATE.FNT", dec);
        for (int i = 4; i < 294; i++) {
            font[53].data[i] = (font[43].data[i] & dec[15].data[i]) | dec[14].data[i];
            font[54].data[i] = (font[44].data[i] & dec[13].data[i]) | dec[12].data[i];
        }
    }
    else if (uMap == "TROLTOWN") {
        std::vector<FontType> dec(56); Internal_LoadFNT("DECORATE.FNT", dec);
        for (int i = 4; i < 294; i++) {
            font[53].data[i] = (font[44].data[i] & dec[17].data[i]) | dec[16].data[i];
            font[54].data[i] = (font[44].data[i] & dec[15].data[i]) | dec[14].data[i];
        }
    }
    else if (uMap == "GROUND4") {
        std::memcpy(font[52].data, font[14].data, 294);
        std::memcpy(font[54].data, font[15].data, 294);
    }
    else if (uMap == "KOBOLD") {
        std::vector<FontType> grnd(56); Internal_LoadFNT("GROUND.FNT", grnd);
        std::memcpy(font[35].data, grnd[18].data, 294);
        std::memcpy(font[52].data, font[48].data, 294);
    }
    else if (uMap == "ANCIENT") {
        std::vector<FontType> dec(56); Internal_LoadFNT("DECORATE.FNT", dec);
        for (int i = 4; i < 294; i++) {
            font[53].data[i] = (font[44].data[i] & dec[21].data[i]) | dec[20].data[i];
        }
        std::vector<FontType> grnd(56); Internal_LoadFNT("GROUND.FNT", grnd);
        std::memcpy(font[22].data, grnd[13].data, 294);
        std::memcpy(font[3].data, grnd[16].data, 294);
        std::memcpy(font[4].data, grnd[17].data, 294);
    }
    else if (uMap == "HUT") {
        std::vector<FontType> dec(56); Internal_LoadFNT("DECORATE.FNT", dec);
        for (int i = 4; i < 294; i++) {
            font[53].data[i] = (font[44].data[i] & dec[17].data[i]) | dec[16].data[i];
        }
    }
    else if (uMap == "DRACTOWN") {
        std::vector<FontType> dec(56); Internal_LoadFNT("DECORATE.FNT", dec);
        for (int i = 4; i < 294; i++) {
            font[53].data[i] = (font[44].data[i] & dec[19].data[i]) | dec[18].data[i];
            font[52].data[i] = (font[44].data[i] & dec[23].data[i]) | dec[22].data[i];
            font[49].data[i] = (font[44].data[i] & dec[21].data[i]) | dec[20].data[i];
        }
        std::vector<FontType> grnd(56); Internal_LoadFNT("GROUND.FNT", grnd);
        std::memcpy(font[8].data, grnd[16].data, 294);
        std::memcpy(font[9].data, grnd[17].data, 294);
        std::memcpy(font[14].data, grnd[19].data, 294);
        std::memcpy(font[19].data, font[24].data, 294);
    }
    else if (uMap == "TOMB") {
        std::vector<FontType> grnd(56); Internal_LoadFNT("GROUND.FNT", grnd);
        std::memcpy(font[37].data, grnd[16].data, 294);
        std::memcpy(font[38].data, grnd[17].data, 294);
        std::memcpy(font[54].data, grnd[14].data, 294);
        std::vector<FontType> twn(56); Internal_LoadFNT("TOWN.FNT", twn);
        std::memcpy(font[53].data, twn[48].data, 294);
    }
    else if (uMap == "LIGHT") {
        std::vector<FontType> dec(56); Internal_LoadFNT("DECORATE.FNT", dec);
        for (int i = 4; i < 294; i++) {
            font[53].data[i] = (font[42].data[i] & dec[25].data[i]) | dec[24].data[i];
        }
    }
    else if (uMap == "DOME") {
        std::vector<FontType> dec(56); Internal_LoadFNT("DECORATE.FNT", dec);
        for (int i = 4; i < 294; i++) {
            font[54].data[i] = (font[35].data[i] & dec[25].data[i]) | dec[24].data[i];
        }
    }
    // --- Special Font Loading END ---

    // 누락되었던 default 맵 배경 타일 복사
    if (Map_Header.default_val < 56) {
        std::memcpy(font[0].data, font[Map_Header.default_val].data, 294);
    }

    LoadFont = true;

    if (Map_Header.ymax / 2 > pas_y) face = 4; else face = 5;
    if (player[1].classtype == magic) face += 4;

    if (!(encounter[0] >= 1 && encounter[0] <= 3)) encounter[0] = 2;
    if (!(maxenemy[0] >= 3 && maxenemy[0] <= 7)) maxenemy[0] = 5;

    switch (party.etc[12]) {
    case 1: setscrolltype(snow); break;
    case 2: setscrolltype(rain); break;
    case 3: setscrolltype(autumn); break;
    case 4: setscrolltype(wilderness); break;
    case 5: setscrolltype(strongrain); break;
    default: setscrolltype(normal); break;
    }
    Clear_View_Screen();
}

void InitMystSub() { 
    if(!map) map = new MapType(); 
    face = 4;

    for (int i = 1; i <= 6; i++) {
        player[i].name_hidden_len = 0;
        memset(player[i].name_hidden, 0, 17);
    }
}

void DisplayCondition() {
    for (int p_idx = 1; p_idx <= 6; p_idx++) {
        SetColor(8);
        Bar(540, p_idx * 20 + 330, 610, p_idx * 20 + 346);

        if (player[p_idx].name_len > 0) {
            std::string cond = ReturnCondition(p_idx);
            if (cond == "   좋음") SetColor(15); else SetColor(12);
            OutHPrintXY(540, 330 + 20 * p_idx, cond.c_str(), false);
        }
    }
    SetColor(15);
    UpdateScreen();
}

void DisplayHP() {
    for (int p_idx = 1; p_idx <= 6; p_idx++) {
        SetColor(8);
        Bar(400, p_idx * 20 + 330, 440, p_idx * 20 + 346);

        if (player[p_idx].name_len > 0) {
            char buf[32];
            SetColor(15);
            std::snprintf(buf, sizeof(buf), "%5d", player[p_idx].hp);
            OutHPrintXY(400, 330 + 20 * p_idx, buf, false);
        }
    }
    UpdateScreen();
}

void DisplaySP() {
    for (int p_idx = 1; p_idx <= 6; p_idx++) {
        SetColor(8);
        Bar(463, p_idx * 20 + 330, 503, p_idx * 20 + 346); // SP 영역 무조건 지우기

        if (player[p_idx].name_len > 0) {
            char buf[32];
            SetColor(15);
            std::snprintf(buf, sizeof(buf), "%5d", player[p_idx].sp);
            OutHPrintXY(463, 330 + 20 * p_idx, buf, false);
        }
    }
    UpdateScreen();
}

void SimpleDisCond() {
    for (int p_idx = 1; p_idx <= 6; p_idx++) {
        SetColor(8);
        Bar(400, p_idx * 20 + 330, 440, p_idx * 20 + 346);
        Bar(463, p_idx * 20 + 330, 503, p_idx * 20 + 346);

        if (player[p_idx].name_len > 0) {
            char buf[32];
            SetColor(15);
            std::snprintf(buf, sizeof(buf), "%5d", player[p_idx].hp);
            OutHPrintXY(400, 330 + 20 * p_idx, buf, false);
            std::snprintf(buf, sizeof(buf), "%5d", player[p_idx].sp);
            OutHPrintXY(463, 330 + 20 * p_idx, buf, false);
        }
    }
    DisplayCondition();
}

void Display_Condition() {
    SimpleDisCond();
    for (int p_idx = 1; p_idx <= 6; p_idx++) {
        SetColor(8);
        Bar(260, p_idx * 20 + 330, 388, p_idx * 20 + 346);

        if (player[p_idx].name_len > 0) {
            SetColor(15);
            std::string name_str = FromPas(player[p_idx].name, player[p_idx].name_len);
            OutHPrintXY(260, 330 + 20 * p_idx, name_str.c_str(), false);
        }
    }
    UpdateScreen();
}


void Set_All() {
    encounter = &party.etc[7];
    maxenemy = &party.etc[8];
	SoundOn = true;
    Cruel = true;

    LoadNo = '1';
    Rest_Time = 6;

    SetFont(2); 
    InitMystSub(); 
    Load(); 
    
    SetColor(8); Bar(0, 0, 639, 479);
    SetColor(0); Bar(624, 0, 639, 479);

    // Initial Condition for Menace Mine start
    if (party.year == 673 && party.day == 326 && party.hour == 16 && party.min == 0) {
        pas_x = 27; pas_y = 29;
        PutMap(24, 34, 43); PutMap(35, 41, 43); PutMap(15, 36, 43); PutMap(27, 26, 43);
        PutMap(27, 32, 21);
        Clear();
        Print(7, " 여기는 로어 대륙의 메너스 금광.");
        Print(7, " 때는 673년 11월 27일 오후였다.");
        party.etc[1] = 4;
    }
    
    std::vector<FontType> decorate(56);
    Internal_LoadFNT("DECORATE.FNT", decorate);
    
    Image(0, 0, &decorate[3], false, COPY_PUT);
    for(int i=1; i<=24; i++) Image(i, 0, &decorate[5], false, COPY_PUT);
    Image(25, 0, &decorate[4], false, COPY_PUT);
    for(int i=1; i<=19; i++) {
        Image(0, i, &decorate[6], false, COPY_PUT);
        Image(25, i, &decorate[7], false, COPY_PUT);
    }

    
    Scroll(true);
    
    // 3D Border Lines from original Set_All
    SetColor(15); // White
    Line(24*10+1, 24*1-1, 24*10+1, 24*12+1);
    SetColor(7); // LightGray
    Line(24*1-1, 24*12+1, 24*10+1, 24*12+1);
    
    SetColor(11);

    SetColor(11);
    bHPrint(260, 313, "  이름           생명력   마력       상 태");
    Display_Condition();
    UpdateScreen();
}


std::string ReturnCondition(int number) { 
    Lore& p = player[number]; 
    if (p.hp <= 0 && p.unconscious == 0) p.unconscious = 1;
    if (p.unconscious > p.endurance * p.level && p.dead == 0) p.dead = 1;
    if (p.dead > 0) return "죽은 상태"; 
    if (p.unconscious > 0) return "의식 불명"; 
    if (p.poison > 0) return "   중독"; 
    return "   좋음"; 
}

void DetectGameOver() { 
    int alive_count = 0; 
    for (int i_idx = 1; i_idx <= 6; i_idx++) if (exist(i_idx)) alive_count++; 
    if (alive_count == 0) { party.etc[6] = 255; GameOver(); } 
}

void GameOver() { 
    if(party.etc[6] == 255) {
        SDL_Event ev_flush; while(SDL_PollEvent(&ev_flush)) {};
        Clear();
        SetColor(13); HPrintXY(280, 50, "일행은 모험중에 모두 목숨을 잃었다.");
        PressAnyKey();
        if(Restore_Game()) return;
    } else if(party.etc[6] != 1) {
        SetColor(10); HPrintXY(300, 50, "정말로 끝내겠습니까 ?");
        m[0] = ""; m[1] = "       << 아니오 >>"; m[2] = "       <<   예   >>";
        if(Select(110, 2, 2, false, true) < 2) return;
    } else {
        SDL_Event ev_flush; while(SDL_PollEvent(&ev_flush)) {};
        Clear();
        SetColor(13); HPrintXY(280, 50, "일행은 모두 전투에서 패했다 !!");
        SetColor(10); HPrintXY(280, 70, "    어떻게 하시겠습니까 ?");
        m[0] = ""; m[1] = "   이전의 게임을 재개한다"; m[2] = "       게임을 끝낸다";
        if(Select(120, 2, 2, false, true) == 1) {
            if(Restore_Game()) return;
        }
    }
    UnSound();
    std::exit(0); 
}

void originposition() { pas_x -= pas_x1; pas_y -= pas_y1; Silent_Scroll(); }

int SelectSub(int yinit, int select_init, int maxsum, int total, bool clean, bool lastclean) {
    if (clean) Clear(); 
    SetColor(12); HPrintXY4Select(275, 30, m[0].c_str()); 
    if (maxsum < 1) select_init = 0;
    for (int i_idx = 1; i_idx <= total; i_idx++) { 
        SetColor(7); if (i_idx == select_init) SetColor(15); if (i_idx > maxsum) SetColor(0); 
        HPrintXY4Select(290, yinit + (i_idx - 1) * 16, m[i_idx].c_str()); 
    }
    if (maxsum <= 0) { return 0; }
    SDL_Event ev_flush; while(SDL_PollEvent(&ev_flush)) {};
    int current_k = select_init; bool select_ok = false;
    while (!select_ok) { 
        SDL_Event ev_sel; 
        while (SDL_PollEvent(&ev_sel)) { 
            if (ev_sel.type == SDL_KEYDOWN) { 
                int y_dir = 0; 
                switch (ev_sel.key.keysym.sym) { 
                    case SDLK_UP: y_dir = -1; break; 
                    case SDLK_DOWN: y_dir = 1; break; 
                    case SDLK_RETURN: case SDLK_SPACE: select_ok = true; break; 
                    case SDLK_ESCAPE: select_ok = true; current_k = 0; break; 
                } 
                if (y_dir != 0) { 
                    SetColor(7); HPrintXY4Select(290, yinit + (current_k - 1) * 16, m[current_k].c_str()); 
                    current_k += y_dir; 
                    if (current_k < 1) current_k = maxsum; 
                    if (current_k > maxsum) current_k = 1; 
                    SetColor(15); HPrintXY4Select(290, yinit + (current_k - 1) * 16, m[current_k].c_str()); 
                } 
            } 
        } 
        UpdateScreen(); SDL_Delay(10);
    }
    if (lastclean) Clear(); 
    return current_k;
}

int Select(int yinit, int maxsum, int total, bool clean, bool lastclean) { return SelectSub(yinit, 1, maxsum, total, clean, lastclean); }

bool Restore_Game() {
	m[0] = "불러 내고 싶은 게임을 선택하십시오.";
	m[1] = "없습니다";
	m[2] = "본 게임 데이타";
	for (int i = 3; i <= 10; i++) m[i] = "게임 데이타 " + std::to_string(i - 2) + " (부)";
	int k_sel = Select(65, 10, 10, true, true);
	if (k_sel > 1) {
		LoadNo = '0' + (k_sel - 1);
        const std::string savefile = "PLAYER" + std::string(1, LoadNo) + ".DAT";
		std::ifstream pf("assets/" + savefile, std::ios::binary);
		if (!pf) {
			SetColor(12); HPrintXY(300, 80, "저장된 데이타가 없습니다 !");
			PressAnyKey(); return false;
		}
		SetColor(11); HPrintXY(280, 80, "저장했던 게임을 다시 불러옵니다");
		LoadFont = false; Load();
		Scroll(true);

		// 3D Border Lines from original Set_All
		SetColor(15); // White
		Line(24 * 10 + 1, 24 * 1 - 1, 24 * 10 + 1, 24 * 12 + 1);
		SetColor(7); // LightGray
		Line(24 * 1 - 1, 24 * 12 + 1, 24 * 10 + 1, 24 * 12 + 1);

		SetColor(11);
		Display_Condition();
		LoadNo = '1'; party.etc[6] = 254; Clear(); Time_Watch = false; return true;
	}
	return false;
}

int ChooseWhomSub(int yinit, bool printing, bool clean) { 
    int active_count = 0; 
    for (int p_idx = 1; p_idx <= 5; p_idx++) { 
        if (player[p_idx].name_len > 0) { 
            active_count++; m[active_count] = FromPas(player[p_idx].name, player[p_idx].name_len); 
        } 
    } 
    if (active_count == 0) return 0; if (active_count == 1) return 1; 
    if (printing) { SetColor(10); HPrintXY(265, 70, "한명을 고르시오 ---"); } 
    m[0] = ""; int sel = Select(yinit, active_count, active_count, clean, true); if (sel == 0) return 0; 
    int current_j = 0; for (int p_idx = 1; p_idx <= 5; p_idx++) { if (player[p_idx].name_len > 0) { current_j++; if (current_j == sel) return p_idx; } } 
    return 0; 
}

int ChooseWhom(bool clean) { return ChooseWhomSub(106, true, clean); }
bool at(int xx, int yy) { return (pas_x + pas_x1 == xx) && (pas_y + pas_y1 == yy); }
bool on(int xx, int yy) { return (pas_x == xx) && (pas_y == yy); }

bool wantexit() { 
    Clear(); Print(11, "여기서 나가기를 원합니까 ?"); 
    m[0] = ""; m[1] = "예, 그렇습니다."; m[2] = "아니오, 원하지 않습니다."; 
    return Select(100, 2, 2, false, true) == 1; 
}

bool wantenter(std::string name) { 
    Clear(); Print(11, name + " 에 들어가기를 원합니까 ?"); 
    m[0] = ""; m[1] = "예, 그렇습니다."; m[2] = "아니오, 원하지 않습니다."; 
    return Select(100, 2, 2, false, true) == 1; 
}

void DefaultEnter() {
    absolute_set_xy = true;
    pas_x = Map_Header.enterX;
    pas_y = Map_Header.enterY;
    mapname = FromPas(Map_Header.entermap, Map_Header.entermap_len);
    Load();
    Scroll(true);
    absolute_set_xy = false;
    pas_x1 = 0; pas_y1 = 0;
}

void DefaultExit() {
    absolute_set_xy = true;
    pas_x = Map_Header.exitX;
    pas_y = Map_Header.exitY;
    mapname = FromPas(Map_Header.exitmap, Map_Header.exitmap_len);
    Load();
    Scroll(true);
    absolute_set_xy = false;
    pas_x1 = 0; pas_y1 = 0;
}

void join(byte num, byte partynum) { 
    if (partynum < 1 || partynum > 7) return; 
    const EnemyData1& ed = EnemyData_Const[num]; Lore& p = player[partynum]; 
    p.name_len = std::min((int)ed.name.length(), 16); std::memcpy(p.name, ed.name.c_str(), p.name_len); 
    p.sex = neutral; p._class = 0; p.classtype = sword; p.strength = ed.strength; p.mentality = ed.mentality; p.endurance = ed.endurance; p.resistance = ed.resistance[0]; p.agility = ed.agility; p.accuracy = ed.accuracy[0]; p.level = ed.level; p.hp = p.endurance * p.level * 10; p.sp = 0; 
}

void joinenemy(byte num, byte j_idx) { 
    if (num < 1 || num > 8) return; if (j_idx < 1 || j_idx > 74) j_idx = 1; 
    const EnemyData1& ed = EnemyData_Const[j_idx]; EnemyData2& e = enemy[num]; 
    e.E_number = j_idx; e.name = ed.name; e.strength = ed.strength; e.mentality = ed.mentality; e.endurance = ed.endurance; e.resistance[0] = ed.resistance[0]; e.resistance[1] = ed.resistance[1]; e.agility = ed.agility; e.accuracy[0] = ed.accuracy[0]; e.accuracy[1] = ed.accuracy[1]; e.ac = ed.ac; e.special = ed.special; e.castlevel = ed.castlevel; e.specialcastlevel = ed.specialcastlevel; e.level = ed.level; e.hp = e.endurance * e.level * 10; e.poison = false; e.unconscious = false; e.dead = false; 
}

void turn_mind(byte j_idx, byte enemy_num) {
    if(enemy_num < 1 || enemy_num > 8) return;
    Lore& p = player[j_idx]; EnemyData2& e = enemy[enemy_num];
    e.E_number = 1;
    e.name = FromPas(p.name, p.name_len);
    e.strength = p.strength; e.mentality = p.mentality; e.endurance = p.endurance;
    e.resistance[0] = p.resistance * 5; e.resistance[1] = p.resistance * 5;
    if(e.resistance[0] > 99) e.resistance[0] = 99;
    if(e.resistance[1] > 99) e.resistance[1] = 99;
    e.agility = p.agility; e.accuracy[0] = p.accuracy; e.accuracy[1] = p.accuracy;
    e.ac = p.ac;
    e.special = (p._class == 7) ? 2 : 0;
    e.castlevel = p.level / 4; e.specialcastlevel = 0; e.level = p.level;
    e.hp = e.endurance * e.level * 10;
    e.poison = false; e.unconscious = false; e.dead = false;
}

int ReturnJoinMember() {
    Clear();
    m[0] = "교체 시킬 인물은 누구입니까 ?";
    for(int i=2; i<=6; i++) {
        if(player[i].name_len > 0) m[i-1] = FromPas(player[i].name, player[i].name_len);
        else m[i-1] = "";
    }
    if(m[5] == "") m[5] = "보조 일원으로 둠";
    return Select(80, 5, 5, false, true) + 1;
}

void Weapon_Shop() { 
    const longint weapon_price[5][8] = { {0, 0, 0, 0, 0, 0, 0, 0}, {0, 500, 3000, 5000, 7000, 12000, 40000, 70000}, {0, 500, 3000, 5000, 10000, 30000, 60000, 100000}, {0, 100, 1000, 1500, 4000, 8000, 35000, 50000}, {0, 200, 300, 800, 2000, 5000, 10000, 30000} }; 
    const longint shield_price[6] = {0, 3000, 15000, 45000, 80000, 150000};
    const longint armor_price[11] = {0, 2000, 5000, 22000, 45000, 75000, 100000, 140000, 200000, 350000, 500000};
    while (true) { 
        Clear(); Print(15, "여기는 무기상점입니다."); Print(15, "우리들은 무기, 방패, 갑옷을 팔고있습니다."); Print(15, "어떤 종류를 원하십니까 ?"); 
        m[0] = ""; m[1] = "베는 무기류"; m[2] = "찍는 무기류"; m[3] = "찌르는 무기류"; m[4] = "쏘는 무기류"; m[5] = "방패류"; m[6] = "갑옷류"; 
        int h_val = Select(120, 6, 6, false, true); 
        if (h_val == 0) { Display_Condition(); Clear(); return; } 
        
        if(h_val >= 1 && h_val <= 4) {
            while(true) {
                hany = 15; Print(15, "어떤 무기를 원하십니까 ?");
                for(int i=1; i<=7; i++) {
                    m[i] = ReturnWeapon((h_val-1)*7+i);
                    HPrintXY4Select(420, 70+(i-1)*16, ("금 " + std::to_string(weapon_price[h_val][i]) + " 개").c_str());
                }
                int k_val = Select(70, 7, 7, false, true); if(k_val == 0) break;
                longint cost = weapon_price[h_val][k_val];
                if(party.gold < cost) { notenoughmoney(); Clear(); continue; }
                if(party.BackPack.data[h_val-1][k_val-1] < 255) party.BackPack.data[h_val-1][k_val-1]++;
                party.gold -= cost; Clear();
            }
        } else if(h_val == 5) {
            while(true) {
                Print(15, "어떤 방패를 원하십니까 ?");
                for(int i=1; i<=5; i++) {
                    m[i] = ReturnShield(i); HPrintXY4Select(420, 90+(i-1)*16, ("금 " + std::to_string(shield_price[i]) + " 개").c_str());
                }
                int k_val = Select(90, 5, 5, false, true); if(k_val == 0) break;
                longint cost = shield_price[k_val];
                if(party.gold < cost) { notenoughmoney(); Clear(); continue; }
                if(party.BackPack.data[h_val-1][k_val-1] < 255) party.BackPack.data[h_val-1][k_val-1]++;
                party.gold -= cost; Clear();
            }
        } else if(h_val == 6) {
            while(true) {
                Print(15, "어떤 갑옷을 원하십니까 ?");
                for(int i=1; i<=10; i++) {
                    m[i] = ReturnArmor(i); HPrintXY4Select(420, 80+(i-1)*16, ("금 " + std::to_string(armor_price[i]) + " 개").c_str());
                }
                int k_val = Select(80, 10, 10, false, true); if(k_val == 0) break;
                longint cost = armor_price[k_val];
                if(party.gold < cost) { notenoughmoney(); Clear(); continue; }
                if(party.BackPack.data[h_val-1][k_val-1] < 255) party.BackPack.data[h_val-1][k_val-1]++;
                party.gold -= cost; Clear();
            }
        }
    } 
}

void Grocery() { 
    Print(15, "여기는 식료품점 입니다."); Print(15, "몇개를 원하십니까 ?");
    m[1] = "필요 없습니다"; 
    for (int i_idx = 1; i_idx <= 5; i_idx++) { 
        m[i_idx + 1] = std::to_string(i_idx * 10) + " 인분 : 금 " + std::to_string(i_idx * 100) + " 개"; 
    } 
    m[0] = ""; int k_val = Select(95, 6, 6, false, true); if (k_val < 2) { asyouwish(); return; } 
    k_val--;
    if(party.gold < k_val * 100) { notenoughmoney(); return; }
    party.gold -= k_val * 100;
    int add_food = k_val * 10;
    party.food = std::min(255, (int)party.food + add_food); 
    thankyou();
}

std::string ReturnWeapon(int number) { 
    const char* data[] = {"불확실한 무기","맨손", "단검","그라디우스","샤벨","신월도","인월도","장검","프렘버그", "곤봉","소형 도끼","프레일","전투용 망치","철퇴","양날 전투 도끼","핼버드", "단도","기병창","단창","레이피어","삼지창","랜서","도끼창", "블로우 파이프","표창","투석기","투창","활","석궁","아르발레스트", "화염","해일","폭풍","지진","이빨","촉수","창", "발톱","바위","화염검","동물의 뼈","번개 마법", "점토","강철 주먹","산성 가스","전광","독가스", "불꽃","염소 가스","한기","냉동 가스"}; 
    std::string w_str = "불확실한 무기";
    if (number >= 0 && number <= 49) w_str = data[number + 1];
    
    if (number == 3 || number == 10 || number == 26 || number == 30 || number == 33) { Josa = ""; Mokjuk = "을"; }
    else if (number == 0 || number == 1 || number == 6 || number == 8 || number == 16 || number == 17 || number == 19 || number == 21 || number == 23 || number == 25 || number == 27 || number == 29 || number == 31 || number == 32 || number == 35 || number == 36 || number == 38 || number == 40 || number == 42 || number == 44 || number == 46) { Josa = "으"; Mokjuk = "을"; }
    else { Josa = ""; Mokjuk = "를"; }
    return w_str;
}

std::string ReturnShield(int number) { 
    const char* data[] = {"불확실함","없음", "가죽 방패","소형 강철 방패","대형 강철 방패","크로매틱 방패", "플래티움 방패"}; 
    std::string s_str = "불확실함";
    if (number >= 0 && number <= 5) s_str = data[number + 1];
    if (number == 0) { Josa = "으"; Mokjuk = "을"; } else { Josa = ""; Mokjuk = "를"; }
    return s_str; 
}

std::string ReturnArmor(int number) { 
    const char* data[] = {"불확실함","없음", "가죽 갑옷","링 메일","체인 메일","미늘 갑옷","브리간디", "큐일보일","라멜라","철판 갑옷","크로매틱 갑옷","플래티움 갑옷"}; 
    std::string a_str = "불확실함";
    if (number >= 0 && number <= 10) a_str = data[number + 1];
    if (number <= 4 || number == 6 || number >= 8) { Josa = "으"; Mokjuk = "을"; } else { Josa = ""; Mokjuk = "를"; }
    return a_str; 
}

std::string ReturnSex(int number) { 
    if (number < 1 || number > 7) return ""; 
    switch(player[number].sex) { case male: return "남성"; case female: return "여성"; case neutral: return "중성"; default: return ""; } 
}

std::string ReturnClass(int number) { 
    if (number < 1 || number > 7) return ""; 
    const char* sword_classes[] = {"", "투사", "기사", "검사", "사냥꾼", "전투승", "암살자", "전사"}; 
    const char* magic_classes[] = {"", "메이지", "컨져러", "주술사", "위저드", "강령술사", "대마법사", "타임워커"}; 
    Lore& p = player[number]; 
    if (p.classtype == sword) { if (p._class >= 1 && p._class <= 7) return sword_classes[p._class]; } 
    else if (p.classtype == magic) { if (p._class >= 1 && p._class <= 7) return magic_classes[p._class]; } 
    return "불확실함"; 
}

std::string ReturnClassType(int number) {
    if (number < 1 || number > 7) return "불확실함";

    switch (player[number].classtype) {
    case sword: return "전투사계";
    case magic: return "마법사계";
    case elemental: return "정령계";
    case giant: return "거인족";
    case golem: return "고렘족";
    case dragon: return "드래곤족";
    default: return "불확실함";
    }
}

bool ReturnClassEnable(int number, int classnumber) {
    if (number >= 1 && number <= 6 && classnumber >= 1 && classnumber <= 7) {
        Lore& p = player[number];
        if (p.classtype == sword) {
            switch (classnumber) {
            case 1: return (p.raw_skills[0] >= 10 && p.raw_skills[1] >= 10 && p.raw_skills[2] >= 10 && p.raw_skills[3] >= 10 && p.raw_skills[4] >= 10);
            case 2: return (p.raw_skills[0] >= 10 && p.raw_skills[1] >= 10 && p.raw_skills[2] >= 5 && p.raw_skills[4] >= 20);
            case 3: return (p.raw_skills[0] >= 40);
            case 4: return (p.raw_skills[1] >= 5 && p.raw_skills[2] >= 5 && p.raw_skills[3] >= 40);
            case 5: return (p.raw_skills[5] >= 40);
            case 6: return (p.raw_skills[0] >= 10 && p.raw_skills[3] >= 10 && p.raw_skills[5] >= 20);
            case 7: return (p.raw_skills[0] >= 25 && p.raw_skills[2] >= 5 && p.raw_skills[4] >= 20 && p.raw_skills[5] >= 10);
            }
        }
        else { // magic
            switch (classnumber) {
            case 1: return (p.raw_skills[0] >= 10 && p.raw_skills[1] >= 10 && p.raw_skills[2] >= 10);
            case 2: return (p.raw_skills[5] >= 10 && p.raw_skills[1] >= 10 && p.raw_skills[2] >= 10);
            case 3: return (p.raw_skills[5] >= 10 && p.raw_skills[2] >= 10);
            case 4: return (p.raw_skills[0] >= 40 && p.raw_skills[1] >= 25 && p.raw_skills[2] >= 25);
            case 5: return (p.raw_skills[0] >= 20 && p.raw_skills[1] >= 20 && p.raw_skills[2] >= 40 && p.raw_skills[5] >= 40);
            case 6: return (p.raw_skills[0] >= 10 && p.raw_skills[1] >= 40 && p.raw_skills[2] >= 30 && p.raw_skills[5] >= 20);
            case 7: return (p.raw_skills[0] >= 40 && p.raw_skills[1] >= 40 && p.raw_skills[2] >= 40 && p.raw_skills[3] >= 20 && p.raw_skills[4] >= 20 && p.raw_skills[5] >= 20);
            }
        }
    }
    return false;
}

void ReturnSexData() { 
    if (person >= 1 && person <= 7 && player[person].sex == female) sexdata = "그녀"; else sexdata = "그"; 
}

void Hospital() {
    Print(15, "여기는 병원입니다."); PressAnyKey();
    while (true) {
        Print(15, "누가 치료를 받겠습니까 ?");
        int k_val = ChooseWhom(false);
        if (k_val == 0) { Display_Condition(); Clear(); return; }

        bool action_taken = false; // 치료나 취소 행동 완료 여부

        while (!action_taken) {
            m[0] = "어떤 치료입니까 ?"; m[1] = "상처를 치료"; m[2] = "독을 제거"; m[3] = "의식의 회복"; m[4] = "부활";
            int j_val = Select(70, 4, 4, false, true);
            if (j_val == 0) {
                Clear();
                action_taken = true; // ESC 눌러서 취소
                continue;
            }

            Lore& p = player[k_val];
            std::string n_str = FromPas(p.name, p.name_len);

            if (j_val == 1) {
                if (p.dead > 0) { Print(7, n_str + ReturnSJosa(n_str) + " 이미 죽은 상태입니다"); PressAnyKey(); continue; }
                if (p.unconscious > 0) { Print(7, n_str + ReturnSJosa(n_str) + " 이미 의식불명입니다"); PressAnyKey(); continue; }
                if (p.poison > 0) { Print(7, n_str + ReturnSJosa(n_str) + " 독이 퍼진 상태입니다"); PressAnyKey(); continue; }
                if (p.hp >= p.endurance * p.level * 10) { Print(15, n_str + ReturnSJosa(n_str) + " 치료할 필요가 없습니다"); PressAnyKey(); continue; }

                int cost = p.endurance * p.level * 10 - p.hp;
                cost = (int)(cost * (p.level / 10.0f)) + 1;
                if (party.gold < cost) { notenoughmoney(); PressAnyKey(); continue; }
                party.gold -= cost; p.hp = p.endurance * p.level * 10;
                Print(15, n_str + ReturnSJosa(n_str) + (p.sex == male ? " 그의" : " 그녀의") + " 모든 건강이 회복되었다");
                DisplayHP(); PressAnyKey();
                action_taken = true;
            }
            else if (j_val == 2) {
                if (p.dead > 0) { Print(7, n_str + ReturnSJosa(n_str) + " 이미 죽은 상태입니다"); PressAnyKey(); continue; }
                if (p.unconscious > 0) { Print(7, n_str + ReturnSJosa(n_str) + " 이미 의식불명입니다"); PressAnyKey(); continue; }
                if (p.poison == 0) { Print(15, n_str + ReturnSJosa(n_str) + " 독에 걸리지 않았습니다"); PressAnyKey(); continue; }

                int cost = p.level * 10;
                if (party.gold < cost) { notenoughmoney(); PressAnyKey(); continue; }
                party.gold -= cost; p.poison = 0;
                Print(15, n_str + ReturnSJosa(n_str) + " 독이 제거 되었습니다");
                DisplayCondition(); PressAnyKey();
                action_taken = true;
            }
            else if (j_val == 3) {
                if (p.dead > 0) { Print(7, n_str + ReturnSJosa(n_str) + " 이미 죽은 상태입니다"); PressAnyKey(); continue; }
                if (p.unconscious == 0) { Print(15, n_str + ReturnSJosa(n_str) + " 의식불명이 아닙니다"); PressAnyKey(); continue; }

                int cost = p.unconscious * 2;
                if (party.gold < cost) { notenoughmoney(); PressAnyKey(); continue; }
                party.gold -= cost; p.unconscious = 0; p.hp = 1;
                Print(15, n_str + ReturnSJosa(n_str) + " 의식을 차렸습니다");
                DisplayCondition(); DisplayHP(); PressAnyKey();
                action_taken = true;
            }
            else if (j_val == 4) {
                if (p.dead == 0) { Print(15, n_str + ReturnSJosa(n_str) + " 죽지 않았습니다"); PressAnyKey(); continue; }

                int cost = p.dead * 100 + 400;
                if (party.gold < cost) { notenoughmoney(); PressAnyKey(); continue; }
                party.gold -= cost; p.dead = 0;
                if (p.unconscious > p.endurance * p.level) p.unconscious = p.endurance * p.level;
                Print(15, n_str + ReturnSJosa(n_str) + " 다시 살아났습니다");
                DisplayCondition(); PressAnyKey();
                action_taken = true;
            }
        } // while(!action_taken) 끝

        // 치료나 취소가 완료된 후, 파티원 수가 1명 뿐이라면 그대로 병원을 나감
        int active_count = 0;
        for (int i = 1; i <= 5; i++) {
            if (player[i].name_len > 0) active_count++;
        }

        if (active_count <= 1) {
            Display_Condition();
            Clear();
            return;
        }
    }
}

void Medicine() { 
    const char* data[] = {"", "체력 회복약","마법 회복약","해독의 약초","의식의 약초","부활의 약초"};
    const longint pricedata[] = {0, 2000, 3000, 1000, 5000, 10000};
    Clear();
    while(true) {
        Print(15, " 여기는 약초를 파는 곳입니다."); Print(7, ""); Print(15, " 당신이 사고 싶은 약이나 약초를 고르십시오.");
        m[0] = ""; for(int i=1; i<=5; i++) m[i] = data[i];
        int k_val = Select(116, 5, 5, false, true); if(k_val == 0) return;
        
        Print(15, " 갯수를 지정 하십시오.");
        for(int i=1; i<=10; i++) {
            m[i] = std::string(data[k_val]) + "  " + std::to_string(i) + "개 : 금 " + std::to_string(pricedata[k_val] * i) + " 개";
        }
        int j_val = Select(100, 10, 10, false, true); if(j_val == 0) continue;
        
        longint cost = pricedata[k_val] * j_val;
        if(party.gold < cost) { Print(7, " 당신에게는 이 것을 살 돈이 없습니다."); PressAnyKey(); continue; }
        party.gold -= cost;
        party.item[k_val-1] = std::min(255, (int)party.item[k_val-1] + j_val);
    }
}

void Sell_Item() { 
    const char* data[] = {"", "화살","소환 문서","대형 횃불","수정 구슬","비행 부츠","이동 구슬"};
    const longint pricedata[] = {0, 500, 4000, 300, 500, 1000, 5000};
    Clear();
    while(true) {
        Print(15, " 여기는 여러가지 물품을 파는 곳입니다."); Print(7, ""); Print(15, " 당신이 사고 싶은 물건을 고르십시오.");
        m[0] = ""; for(int i=1; i<=6; i++) m[i] = data[i];
        int k_val = Select(116, 6, 6, false, true); if(k_val == 0) return;
        
        Print(15, " 갯수를 지정 하십시오.");
        for(int i=1; i<=10; i++) {
            std::string cnt_str = (k_val == 1) ? std::to_string(i*10) : std::to_string(i);
            m[i] = std::string(data[k_val]) + "  " + cnt_str + "개 : 금 " + std::to_string(pricedata[k_val] * i) + " 개";
        }
        int j_val = Select(100, 10, 10, false, true); if(j_val == 0) continue;
        
        longint cost = pricedata[k_val] * j_val;
        if(party.gold < cost) { Print(7, " 당신에게는 이 것을 살 돈이 없습니다."); PressAnyKey(); continue; }
        party.gold -= cost;
        
        if(k_val == 1) {
            party.arrow = std::min(32767, (int)party.arrow + j_val * 10);
        } else {
            party.etc[(k_val - 1) + 4] = std::min(255, (int)party.etc[(k_val - 1) + 4] + j_val);
        }
    }
}

void Train_Center() { 
    const byte enable_class[7][6] = { {50,50,50,50,50,50}, {60,60,50,0,60,50}, {100,0,0,0,30,30}, {0,50,50,100,0,60}, {0,0,0,0,0,100}, {80,0,60,80,0,70}, {60,50,50,30,70,50} };
    Print(15, " 여기는 군사 훈련소 입니다."); Print(15, " 만약 당신이 충분한 전투 경험을 쌓았다면, 당신은 더욱 능숙하게 무기를 다룰것입니다."); PressAnyKey(); 
    while(true) {
        Clear(); SetColor(15); Print(15, "누가 훈련을 받겠습니까 ?");
        person = ChooseWhom(false); Clear();
        if(person == 0) { Display_Condition(); return; }
        if(player[person].classtype != sword) { Print(7, " 당신은 전투사 계열이 아닙니다."); continue; }
        
        bool all_done = true;
        for(int i=0; i<6; i++) {
            if(enable_class[player[person]._class - 1][i] > 0 && player[person].raw_skills[i] < enable_class[player[person]._class - 1][i]) {
                all_done = false; break;
            }
        }
        if(all_done) {
            Clear(); Print(7, " 당신은 모든 과정을 수료했으므로 모든 경험치를 레벨로 바꾸겠습니다.");
            player[person].potential_experience += player[person].experience; player[person].experience = 0; return;
        }
        
        m[0] = ""; m[1] = "베는  무기  기술치"; m[2] = "찍는  무기  기술치"; m[3] = "찌르는 무기 기술치"; m[4] = "쏘는  무기  기술치"; m[5] = "방패  사용  능력치"; m[6] = "맨손  사용  기술치";
        if(player[person]._class == 7) m[2] = "치료  마법  능력치";
        
        int k_sel = 1;
        while(true) {
            Clear();
            std::string n_str = FromPas(player[person].name, player[person].name_len);
            SetColor(15); HPrintXY4Select(260, 30, (n_str + "의 현재 능력치").c_str());
            for(int i=0; i<6; i++) {
                if(enable_class[player[person]._class - 1][i] > 0) SetColor(11); else SetColor(0);
                std::string s_desc = m[i+1] + " : " + std::to_string(player[person].raw_skills[i]);
                HPrintXY4Select(280, 50 + i*16, s_desc.c_str());
            }
            SetColor(10); HPrintXY4Select(270, 150, ("여분의 경험치 : " + std::to_string(player[person].experience)).c_str());
            
            Bar(260, 190, 610, 288);
            SetColor(12); HPrintXY4Select(260, 170, "당신이 수련 하고 싶은 부분을 고르시오.");
            k_sel = SelectSub(190, k_sel, 6, 6, false, false);
            if(k_sel == 0) break;
            
            Bar(260, 190, 610, 288);
            int e_val = enable_class[player[person]._class - 1][k_sel - 1];
            if(e_val == 0) { SetColor(7); HPrintXY4Select(260, 190, "당신의 계급과 맞지 않습니다"); PressAnyKey(); continue; }
            if(player[person].raw_skills[k_sel - 1] >= e_val) { SetColor(7); HPrintXY4Select(260, 190, "이 분야는 더 배울 것이 없습니다"); PressAnyKey(); continue; }
            longint cost = 15 * player[person].raw_skills[k_sel - 1] * player[person].raw_skills[k_sel - 1];
            if(cost > player[person].experience) { SetColor(7); HPrintXY4Select(260, 190, "아직 경험치가 모자랍니다"); PressAnyKey(); continue; }
            
            player[person].experience -= cost; player[person].potential_experience += cost;
            player[person].raw_skills[k_sel - 1]++;
        }
    }
}

void Magic_School() { 
    const byte enable_class[7][6] = { {50,30,30,0,50,0}, {20,50,30,10,50,30}, {20,20,50,10,50,50}, {100,60,60,50,100,0}, {60,70,100,100,100,100}, {60,100,70,100,100,50}, {70,100,70,50,100,100} };
    Print(15, " 여기는 마법 학교 입니다."); Print(15, " 만약 당신이 충분한 실전 경험을 쌓았다면, 당신은 더욱 능숙하게 마법을 다룰것입니다."); PressAnyKey(); 
    while(true) {
        Clear(); SetColor(15); Print(15, "누가 가르침을 받겠습니까 ?");
        person = ChooseWhom(false); Clear();
        if(person == 0) { Display_Condition(); return; }
        if(player[person].classtype != magic) { Print(7, " 당신은 마법사 계열이 아닙니다."); continue; }
        
        bool all_done = true;
        for(int i=0; i<6; i++) {
            if(enable_class[player[person]._class - 1][i] > 0 && player[person].raw_skills[i] < enable_class[player[person]._class - 1][i]) {
                all_done = false; break;
            }
        }
        if(all_done) {
            Clear(); Print(7, " 당신은 모든 과정을 수료했으므로 모든 경험치를 레벨로 바꾸겠습니다.");
            player[person].potential_experience += player[person].experience; player[person].experience = 0; return;
        }
        
        m[0] = ""; m[1] = "공격 마법 능력치"; m[2] = "변화 마법 능력치"; m[3] = "치료 마법 능력치"; m[4] = "특수 마법 능력치"; m[5] = "초 자연력 능력치"; m[6] = "소환 마법 능력치";
        
        int k_sel = 1;
        while(true) {
            Clear();
            std::string n_str = FromPas(player[person].name, player[person].name_len);
            SetColor(15); HPrintXY4Select(260, 30, (n_str + "의 현재 능력치").c_str());
            for(int i=0; i<6; i++) {
                if(enable_class[player[person]._class - 1][i] > 0) SetColor(11); else SetColor(0);
                std::string s_desc = m[i+1] + " : " + std::to_string(player[person].raw_skills[i]);
                HPrintXY4Select(280, 50 + i*16, s_desc.c_str());
            }
            SetColor(10); HPrintXY4Select(270, 150, ("여분의 경험치 : " + std::to_string(player[person].experience)).c_str());
            
            Bar(260, 190, 610, 288);
            SetColor(12); HPrintXY4Select(260, 170, "당신이 배우고 싶은 부분을 고르시오.");
            k_sel = SelectSub(190, k_sel, 6, 6, false, false);
            if(k_sel == 0) break;
            
            Bar(260, 190, 610, 288);
            int e_val = enable_class[player[person]._class - 1][k_sel - 1];
            if(e_val == 0) { SetColor(7); HPrintXY4Select(260, 190, "당신의 계급과 맞지 않습니다"); PressAnyKey(); continue; }
            if(player[person].raw_skills[k_sel - 1] >= e_val) { SetColor(7); HPrintXY4Select(260, 190, "이 분야는 더 배울 것이 없습니다"); PressAnyKey(); continue; }
            longint cost = 10 * player[person].raw_skills[k_sel - 1] * player[person].raw_skills[k_sel - 1];
            if(cost > player[person].experience) { SetColor(7); HPrintXY4Select(260, 190, "아직 경험치가 모자랍니다"); PressAnyKey(); continue; }
            
            player[person].experience -= cost; player[person].potential_experience += cost;
            player[person].raw_skills[k_sel - 1]++;
        }
    }
}

void Change_Job_for_Sword() { 
    const char* classdata[7] = {"투사","기사","검사","사냥꾼","전투승","암살자","전사"};
    const byte enable_class[7][6] = { {10,10,10,10,10,0}, {10,10,5,0,20,0}, {40,0,0,0,0,0}, {0,5,5,40,0,0}, {0,0,0,0,0,40}, {10,0,0,10,0,20}, {25,0,5,0,20,10} };
    Print(15, " 여기는 군사 훈련소 입니다."); Print(15, " 만약 당신이 원한다면 새로운 계급으로 바꿀 수가 있습니다."); PressAnyKey(); Clear();
    if(party.gold < 10000) { Print(7, " 그러나 일행에게는  직업을 바꿀때 드는 비용인 금 10000 개가 없습니다."); PressAnyKey(); return; }
    
    SetColor(15); Print(15, "누가 전투사 계열의 직업을 바꾸겠습니까 ?"); person = ChooseWhom(false); Clear();
    if(person == 0) { Display_Condition(); return; }
    if(player[person].classtype != sword) { Print(7, " 당신은 전투사 계열이 아닙니다."); return; }
    
    Clear();
    bool enable[7];
    std::string n_str = FromPas(player[person].name, player[person].name_len);
    SetColor(15); HPrintXY4Select(260, 30, (n_str + "의 가능한 계급").c_str());
    for(int i=0; i<7; i++) {
        bool cls_ok = true;
        for(int s=0; s<6; s++) { if(enable_class[i][s] > player[person].raw_skills[s]) cls_ok = false; }
        if(cls_ok) { SetColor(11); enable[i] = true; } else { SetColor(0); enable[i] = false; }
        HPrintXY4Select(280, 80 + i*16, classdata[i]);
    }
    
    m[0] = ""; for(int i=1; i<=7; i++) m[i] = std::string("                 ") + classdata[i-1];
    SetColor(12); HPrintXY4Select(260, 220, "당신이 바뀌고 싶은 계급을 고르시오.");
    SetColor(15); HPrintXY4Select(260, 236, "비용 : 금 10000 개");
    int k_sel = SelectSub(80, player[person]._class, 7, 7, false, true);
    
    if(k_sel > 0) {
        if(enable[k_sel - 1]) {
            if(player[person]._class != k_sel) {
                player[person]._class = k_sel;
                Print(10, n_str + ReturnSJosa(n_str) + " 이제 " + classdata[k_sel - 1] + " 계급이 되었다.");
                if(player[person]._class < 7) player[person].sp = 0;
                Modify_Item(person); party.gold -= 10000; DisplaySP();
            }
        } else {
            Print(7, "이 계급은 아직 불가능 합니다.");
        }
    }
}

void Change_Job_for_Magic() { 
    const char* classdata[7] = {"메이지","컨져러","주술사","위저드","강령술사","대마법사","타임워커"};
    const byte enable_class[7][6] = { {10,10,10,0,0,0}, {0,10,10,0,0,10}, {0,0,10,0,0,10}, {40,25,25,0,0,0}, {20,20,40,0,0,40}, {10,40,30,0,0,20}, {40,40,40,20,20,20} };
    Print(15, " 여기는 마법 학교 입니다."); Print(15, " 만약 당신이 원한다면 새로운 계급으로 바꿀 수가 있습니다."); PressAnyKey(); Clear();
    if(party.gold < 10000) { Print(7, " 그러나 일행에게는  직업을 바꿀때 드는 비용인 금 10000 개가 없습니다."); PressAnyKey(); return; }
    
    SetColor(15); Print(15, "누가 마법사 계열의 직업을 바꾸겠습니까 ?"); person = ChooseWhom(false); Clear();
    if(person == 0) { Display_Condition(); return; }
    if(player[person].classtype != magic) { Print(7, " 당신은 마법사 계열이 아닙니다."); return; }
    
    Clear();
    bool enable[7];
    std::string n_str = FromPas(player[person].name, player[person].name_len);
    SetColor(15); HPrintXY4Select(260, 30, (n_str + "의 가능한 계급").c_str());
    for(int i=0; i<7; i++) {
        bool cls_ok = true;
        for(int s=0; s<6; s++) { if(enable_class[i][s] > player[person].raw_skills[s]) cls_ok = false; }
        if(cls_ok) { SetColor(11); enable[i] = true; } else { SetColor(0); enable[i] = false; }
        HPrintXY4Select(280, 80 + i*16, classdata[i]);
    }
    
    m[0] = ""; for(int i=1; i<=7; i++) m[i] = std::string("                 ") + classdata[i-1];
    SetColor(12); HPrintXY4Select(260, 220, "당신이 바뀌고 싶은 계급을 고르시오.");
    SetColor(15); HPrintXY4Select(260, 236, "비용 : 금 10000 개");
    int k_sel = SelectSub(80, player[person]._class, 7, 7, false, true);
    
    if(k_sel > 0) {
        if(enable[k_sel - 1]) {
            if(player[person]._class != k_sel) {
                player[person]._class = k_sel;
                Print(10, n_str + ReturnSJosa(n_str) + " 이제 " + classdata[k_sel - 1] + " 계급이 되었다.");
                Modify_Item(person); party.gold -= 10000;
            }
        } else {
            Print(7, "이 계급은 아직 불가능 합니다.");
        }
    }
}

void About_Class() { 
    m[0] = "어떤 일을 원하십니까 ?"; m[1] = "전투사 계열의 기술을 습득"; m[2] = "마법사 계열의 능력을 습득"; m[3] = "전투사 계열의 계급을 바꿈"; m[4] = "마법사 계열의 계급을 바꿈"; 
    int k_sel = Select(80, 4, 4, true, true); if(k_sel == 1) Train_Center(); else if(k_sel == 2) Magic_School(); else if(k_sel == 3) Change_Job_for_Sword(); else if(k_sel == 4) Change_Job_for_Magic();
}

void Delete_Scroll_Screen() {
    SetColor(0);
    for (int i_idx = 0; i_idx <= 108; i_idx++) {
        Line(i_idx * 2 + 24, 24, i_idx * 2 + 24, 23 + 24 * 11);
        Line(239 - i_idx * 2, 24, 239 - i_idx * 2, 23 + 24 * 11);
        UpdateScreen();
        SDL_Delay(5);
    }
}

// ---------------------------------------------------------------------------
// UTF-8 -> 조합형(Johab) 한글 변환기
// ---------------------------------------------------------------------------
static const uint8_t UnicodeToJohabCho[] = {
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20
};

static const uint8_t UnicodeToJohabJung[] = {
    3, 4, 5, 6, 7,          // ㅏ, ㅐ, ㅑ, ㅒ, ㅓ
    10, 11, 12, 13, 14, 15, // ㅔ, ㅕ, ㅖ, ㅗ, ㅘ, ㅙ
    18, 19, 20, 21, 22, 23, // ㅚ, ㅛ, ㅜ, ㅝ, ㅞ, ㅟ
    26, 27, 28, 29          // ㅠ, ㅡ, ㅢ, ㅣ
};

static const uint8_t UnicodeToJohabJong[] = {
        1,
        2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, // ㄱ~ㅂ (기존에는 1~17 이었음)
        19, 20, 21, 22, 23, 24, 25, 26, 27, 28                      // ㅄ~ㅎ
};

static void ConvertToJohab(const char* src, int src_len, char* dst, uint8_t& dst_len) {
    const char* p = src;
    const char* end = src + src_len;
    int out_idx = 0;

    while (p < end && *p != '\0') {
        uint8_t c1 = (uint8_t)*p;

        if (c1 < 0x80) {
            // ASCII (영어, 숫자, 기호)
            if (out_idx < 16) dst[out_idx++] = c1;
            p++;
        }
        else if ((c1 & 0xF0) == 0xE0) {
            // UTF-8 3-byte (한글)
            if (p + 2 >= end) break;
            uint8_t c2 = (uint8_t) * (p + 1);
            uint8_t c3 = (uint8_t) * (p + 2);
            uint32_t cp = ((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
            p += 3;

            if (cp >= 0xAC00 && cp <= 0xD7A3) {
                int idx = cp - 0xAC00;
                int cho = idx / (21 * 28);
                int jung = (idx % (21 * 28)) / 28;
                int jong = idx % 28;

                uint8_t j_cho = UnicodeToJohabCho[cho];
                uint8_t j_jung = UnicodeToJohabJung[jung];
                uint8_t j_jong = UnicodeToJohabJong[jong];

                // 조합형 16비트 조합 (최상위 비트는 1)
                uint8_t b1 = 0x80 | (j_cho << 2) | (j_jung >> 3);
                uint8_t b2 = ((j_jung & 0x07) << 5) | j_jong;

                if (out_idx < 15) { // 2바이트 공간 확인
                    dst[out_idx++] = b1;
                    dst[out_idx++] = b2;
                }
            }
        }
        else {
            // 이미 조합형이거나 다른 2바이트 인코딩인 경우 (도스 세이브를 로드한 캐릭터)
            if (p + 1 >= end) break;
            if (out_idx < 15) {
                dst[out_idx++] = c1;
                dst[out_idx++] = *(p + 1);
            }
            p += 2;
        }
    }
    dst_len = out_idx;
}

// ---------------------------------------------------------------------------

void Save() {
    // 1. 플레이어 데이터 (PLAYER#.DAT) 저장
    std::string pfile = "PLAYER" + std::string(1, LoadNo) + ".DAT";
    std::ofstream pf("assets/" + pfile, std::ios::binary);
    if (pf) {
        DOS_LorePlayer dp;
        std::memset(&dp, 0, sizeof(dp));

        dp.year = party.year; dp.day = party.day;
        dp.hour = party.hour; dp.min = party.min; dp.sec = party.sec;
        dp.food = party.food; dp.gold = party.gold; dp.arrow = party.arrow;

        for (int i = 0; i < 10; i++) dp.item[i] = party.item[i + 1];
        for (int i = 0; i < 10; i++) dp.crystal[i] = party.crystal[i + 1];
        for (int i = 0; i < 100; i++) dp.etc[i] = party.etc[i + 1];
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 10; j++) dp.backpack[i][j] = party.BackPack.data[i][j];
        }

        pf.write((char*)&dp, sizeof(dp));

        DOS_Lore dl[7];
        std::memset(&dl, 0, sizeof(dl));
        for (int i = 1; i <= 7; i++) {
            int d_idx = i - 1;

            std::memset(dl[d_idx].name.text, 0, 17);
            if (player[i].name_len == 0 && player[i].name_hidden_len > 0) {
                // 숨김 상태: Pascal 원본 호환 형식으로 저장
                // text[0]='\0'(len=0), text[8]=Johab백업길이, text[9..]=Johab백업데이터
                dl[d_idx].name.len = 0;
                char johab_buf[17] = {};
                uint8_t johab_len = 0;
                ConvertToJohab(player[i].name_hidden, player[i].name_hidden_len,
                               johab_buf, johab_len);
                // Pascal 백업 영역은 name[10..17] = 최대 8바이트
                // Johab 문자 경계(한글=2바이트, ASCII=1바이트)에 맞춰 잘라내어
                // text[8]에는 실제 저장된 바이트 수를 기록 (로드 시 버퍼 오버리드 방지)
                uint8_t safe_len = 0;
                for (uint8_t pos = 0; pos < johab_len; ) {
                    uint8_t char_bytes = ((uint8_t)johab_buf[pos] & 0x80) ? 2 : 1;
                    if (safe_len + char_bytes > 8) break;
                    safe_len += char_bytes;
                    pos   += char_bytes;
                }
                dl[d_idx].name.text[8] = (char)safe_len;
                std::memcpy(dl[d_idx].name.text + 9, johab_buf, safe_len);
            } else {
                // 일반 상태
                ConvertToJohab(player[i].name, player[i].name_len,
                               dl[d_idx].name.text, dl[d_idx].name.len);
            }

            dl[d_idx].sex = player[i].sex; dl[d_idx]._class = player[i]._class;
            dl[d_idx].strength = player[i].strength; dl[d_idx].mentality = player[i].mentality;
            dl[d_idx].concentration = player[i].concentration; dl[d_idx].endurance = player[i].endurance;
            dl[d_idx].resistance = player[i].resistance; dl[d_idx].agility = player[i].agility;
            dl[d_idx].accuracy = player[i].accuracy; dl[d_idx].luck = player[i].luck; dl[d_idx].poison = player[i].poison;
            dl[d_idx].unconscious = player[i].unconscious; dl[d_idx].dead = player[i].dead;
            dl[d_idx].hp = player[i].hp; dl[d_idx].sp = player[i].sp;
            dl[d_idx].ac = player[i].ac; dl[d_idx].level = player[i].level;
            dl[d_idx].experience = player[i].experience; dl[d_idx].potential_experience = player[i].potential_experience;
            dl[d_idx].weapon = player[i].weapon; dl[d_idx].shield = player[i].shield; dl[d_idx].armor = player[i].armor;
            dl[d_idx].potential_ac = player[i].potential_ac; dl[d_idx].wea_power = player[i].wea_power;
            dl[d_idx].shi_power = player[i].shi_power; dl[d_idx].arm_power = player[i].arm_power;
            dl[d_idx].classtype = player[i].classtype;

            std::memcpy(dl[d_idx].raw_skills, player[i].raw_skills, 6);
        }
        pf.write((char*)dl, sizeof(dl));
    }

    // 2. 맵 데이터 (SAVE#.M) 저장
    Map_Header.startX = pas_x;
    Map_Header.startY = pas_y;

    std::string mfile = "SAVE" + std::string(1, LoadNo) + ".M";
    std::ofstream mf("assets/" + mfile, std::ios::binary);
    if (mf) {
        mf.write((char*)&Map_Header, sizeof(Map_Header));
        mf.write((char*)map, sizeof(MapType));
    }
}

bool Verify_Weapon(byte p_idx, byte weapon_no) {
    if(player[p_idx].classtype == magic) return false;
    byte cls = player[p_idx]._class;
    if(weapon_no >= 1 && weapon_no <= 7) return (cls == 1 || cls == 2 || cls == 3 || cls == 6 || cls == 7);
    if(weapon_no >= 8 && weapon_no <= 14) return (cls == 1 || cls == 2 || cls == 4);
    if(weapon_no >= 15 && weapon_no <= 21) return (cls == 1 || cls == 2 || cls == 4 || cls == 6 || cls == 7);
    if(weapon_no >= 22 && weapon_no <= 28) return (cls == 1 || cls == 4 || cls == 6 || cls == 7);
    return false;
}

bool Verify_Shield(byte p_idx, byte shield_no) {
    if(player[p_idx].classtype != sword) return false;
    byte cls = player[p_idx]._class;
    return (cls == 1 || cls == 2 || cls == 3 || cls == 7);
}

bool Verify_Armor(byte p_idx, byte armor_no) {
    if(player[p_idx].classtype == magic && armor_no == 1) return true;
    if(player[p_idx].classtype == sword && (armor_no >= 1 && armor_no <= 10 || armor_no == 255)) return true;
    return false;
}

void Modify_Item(byte p_idx) { 
    const byte weapon_data[4][7][7] = {
        { {15,15,15,15,15,25,15}, {30,30,25,25,25,25,30}, {35,40,35,35,35,35,40}, {45,48,50,40,40,40,40}, {50,55,60,50,50,50,55}, {60,70,70,60,60,60,65}, {70,70,80,70,70,70,70} },
        { {15,15,15,15,15,15,15}, {35,30,30,37,30,30,30}, {35,40,35,35,35,35,35}, {52,45,45,45,45,45,45}, {60,60,55,55,55,55,55}, {75,70,70,70,70,70,70}, {80,85,80,80,80,80,80} },
        { {10,10,10,25,10,20,10}, {35,40,35,35,35,35,40}, {35,30,30,35,30,30,30}, {40,40,40,45,40,40,40}, {60,60,60,60,60,60,60}, {80,80,80,80,80,80,80}, {90,90,90,90,90,90,90} },
        { {10,10,10,15,10,15,10}, {10,10,10,10,10,20,10}, {20,20,20,27,20,20,20}, {35,35,35,40,35,38,35}, {45,45,45,55,45,45,45}, {55,55,55,65,55,55,55}, {70,70,70,85,70,70,70} }
    };
    Lore& p = player[p_idx];
    if(Verify_Weapon(p_idx, p.weapon)) {
        if(p.weapon > 0) {
            int sort = (p.weapon - 1) / 7;
            int order = p.weapon - sort * 7 - 1;
            p.wea_power = weapon_data[sort][order][p._class - 1];
        } else p.wea_power = 5;
    } else p.wea_power = 5;
    
    if(Verify_Shield(p_idx, p.shield)) p.shi_power = p.shield; else p.shi_power = 0;
    
    if(Verify_Armor(p_idx, p.armor)) { p.arm_power = p.armor; if(p.armor == 255) p.arm_power = 20; }
    else p.arm_power = 0;
    
    p.ac = p.potential_ac + p.arm_power;
}

std::string ReturnMagic(int _class, int magic_idx) { 
    const char* data[8][10] = {
        {"마법 화살","마법 화구","마법 단창","독 바늘","마법 발화","냉동 광선","춤추는 검","맥동 광선","직격 뇌전","필멸 주문"},
        {"공기 폭풍","열선 파동","초음파","유독 가스","초냉기","화염 지대","브리자드","에너지 장막","인공 지진","차원 이탈"},
        {"마법의 횃불","주시자의 눈","공중 부상","물위를 걸음","늪위를 걸음","기화 이동","지형 변화","공간 이동","식량 제조","대지형 변화"},
        {"한명 치료","한명 독 제거","한명 의식 돌림","한명 부활","한명 복합 치료","모두 치료","모두 독 제거","모두 의식 돌림","모두 부활","모두 복합 치료"},
        {"한명 기술 무력화","한명 방어 무력화","한명 능력 저하","한명 마법 불능","한명 탈 초인화","모두 기술 무력화","모두 방어 무력화","모두 능력 저하","모두 마법 불능","모두 탈 초인화"},
        {"투시","예언","독심","천리안","염력","수소 핵융합","공포 생성","환상 생성","신진 대사 조절","유체 이탈"},
        {"불의 정령 소환","물의 정령 소환","공기의 정령 소환","땅의 정령 소환","죽은 자의 소생","다른 차원 생물 소환","거인을 부름","고렘을 부름","용을 부름","라이칸스로프 소환"},
        {"화염의 크리스탈","한파의 크리스탈","다크 크리스탈","에보니 크리스탈","영혼의 크리스탈","소환의 크리스탈","에너지 크리스탈","크로매틱 크리스탈","크리스탈 볼",""}
    };
    std::string ret = "알수 없는 마법"; byte code = 1;
    if(_class >= 0 && _class <= 7 && magic_idx >= 1 && magic_idx <= 10) {
        ret = data[_class][magic_idx - 1];
        if(_class == 0 && (magic_idx == 2 || magic_idx == 5)) code = 0;
        if(_class == 1 && (magic_idx >= 3 && magic_idx <= 7)) code = 0;
        if(_class == 2 && (magic_idx == 7 || magic_idx >= 9)) code = 0;
        if(_class == 3 && (magic_idx == 1 || magic_idx == 2 || magic_idx == 5 || magic_idx == 6 || magic_idx == 7 || magic_idx == 10)) code = 0;
        if(_class == 4) code = 0;
        if(_class == 5 && magic_idx == 1) code = 0;
    }
    if(code == 0) { Josa = ""; Mokjuk = "를"; } else { Josa = "으"; Mokjuk = "을"; }
    return ret;
}

longint ReturnEXP(byte level) { 
    const longint data[40] = { 0,1500,6000,20000,50000,150000,250000,500000,800000,1050000, 1320000,1620000,1950000,2320000,2700000,3120000,3570000,4050000,4560000,5100000, 6000000,7000000,8000000,9000000,10000000,12000000,14000000,16000000,18000000,20000000, 25000000,30000000,35000000,40000000,45000000,50000000,55000000,60000000,65000000,70000000 };
    if(level >= 1 && level <= 40) return data[level - 1]; return 0;
}

void Sell_EXP() { 
    while(true) {
        Clear();
        m[0] = "";
        for(int i=1; i<=10; i++) { m[i] = "금 " + std::to_string(i*10000) + " 개; 소요시간 : " + std::to_string(i) + " 시간"; }
        Print(15, " 여기에서는 황금의 양만큼  훈련을 시켜서 경험치를 올려 주는 곳입니다."); Print(11, " 원하시는 금액을 고르십시오.");
        int k_sel = Select(120, 10, 10, false, true); if(k_sel == 0) return;
        if(party.gold < k_sel * 10000) { Print(7, " 일행은 충분한 금이 없었다."); PressAnyKey(); continue; }
        
        Print(15, " 일행은 " + std::to_string(k_sel) + " 시간동안 훈련을 받게 되었다.");
        party.gold -= k_sel * 10000;
        for(int i=1; i<=5; i++) { if(exist(i)) player[i].experience += k_sel * 10000; }
        party.min = 0;
        
        int hours_left = k_sel;
        while(hours_left > 0) {
            party.min += 20;
            if(party.min > 59) { party.hour++; hours_left--; party.min -= 60; }
            if(party.hour > 23) { party.day++; party.hour -= 24; }
            if(party.day > 359) { party.year++; party.day -= 360; }
            SDL_Delay(500); 
    Scroll(true);
    
    // 3D Border Lines from original Set_All
    SetColor(15); // White
    Line(24*10+1, 24*1-1, 24*10+1, 24*12+1);
    SetColor(7); // LightGray
    Line(24*1-1, 24*12+1, 24*10+1, 24*12+1);
    
    SetColor(11);

        }
        
    Scroll(true);
    
    // 3D Border Lines from original Set_All
    SetColor(15); // White
    Line(24*10+1, 24*1-1, 24*10+1, 24*12+1);
    SetColor(7); // LightGray
    Line(24*1-1, 24*12+1, 24*10+1, 24*12+1);
    
    SetColor(11);
 Print(7, ""); Print(7, " 일행은 훈련을 끝 마쳤다.");
    SDL_Event ev_flush; while(SDL_PollEvent(&ev_flush)) {}; PressAnyKey(); return;
    }
}

void AuxPrint(int color, std::string s1, bool newline) { 
    SetColor(color); 
    if (newline) { hany += 16; if (hany > 280) hany = 32; HGotoXY(260, hany); } 
    HPrint(s1.c_str()); 
    UpdateScreen();
}
void cPrint(int color1, int color2, std::string s1, std::string s2, std::string s3) { AuxPrint(color1, s1, true); AuxPrint(color2, s2, false); AuxPrint(color1, s3, false); }

void setbit(int bit) { 
    int _byte = (bit / 8) + 50; int _bit = bit % 8; 
    if (_byte < 100) party.etc[_byte] |= (1 << _bit); 
}

bool getbit(int bit) { 
    int _byte = (bit / 8) + 50; int _bit = bit % 8; 
    if (_byte < 100) return (party.etc[_byte] & (1 << _bit)) > 0; return false; 
}

std::string ReturnMessage(int who, int how, int what, int whom) {
    std::string res = FromPas(player[who].name, player[who].name_len); res += ReturnSJosa(res) + " ";
    switch(how) {
        case 1: { 
            int j_val = (player[who].weapon - 1) / 7; 
            std::string weapon_name = ReturnWeapon(player[who].weapon); 
            std::string enemy_name = enemy[whom].name; 
            switch(j_val) { 
                case 1: res += weapon_name + Josa + "로 " + enemy_name + ReturnOJosa(enemy_name) + " 내려쳤다"; break; 
                case 2: res += weapon_name + Josa + "로 " + enemy_name + ReturnOJosa(enemy_name) + " 찔렀다"; break; 
                case 3: if (party.arrow > 0) res += weapon_name + Josa + "로 " + enemy_name + ReturnOJosa(enemy_name) + " 쏘았다"; else res = "화살이 다 떨어져 공격할 수 없었다"; break; 
                default: res += weapon_name + Josa + "로 " + enemy_name + ReturnOJosa(enemy_name) + " 공격했다"; break; 
            } break; 
        }
        case 2: res += "'" + ReturnMagic(0, what) + "'" + Josa + "로 " + enemy[whom].name + "에게 공격했다"; break;
        case 3: res += "'" + ReturnMagic(1, what) + "'" + Josa + "로 " + enemy[whom].name + "에게 공격했다"; break;
        case 4: res += enemy[whom].name + "에게 " + ReturnMagic(4, what) + Josa + "로 특수 공격을 했다"; break;
        case 5: res += FromPas(player[whom].name, player[whom].name_len) + "에게 '" + ReturnMagic(3, what) + "'" + Mokjuk + " 사용했다"; break;
        case 6: if(whom == 0) res += "모든 적에게 " + ReturnMagic(5, what) + Mokjuk + " 사용했다"; else res += enemy[whom].name + "에게 " + ReturnMagic(5, what) + Mokjuk + " 사용했다"; break;
        case 7: res += ReturnMagic(6, what) + Mokjuk + " 시도 했다"; break;
        case 9: res = "일행은 도망을 시도했다"; break;
        default: res += " 잠시 주저했다"; break;
    }
    return res;
}

int SelectEnemy() {
    // 안전 장치: 적이 0명 이하면 선택 불가능 방어 코드
    if (enemynumber <= 0) return 0;

    int number = 1;
    bool sel_ok = false;
    bool needs_redraw = true; // 첫 진입 시 화면에 그리기 위해 true

    // 코드 중복 방지를 위해 특정 위치의 적을 그리는 람다 함수 작성
    auto draw_enemy = [&](int idx, bool selected) {
        if (idx < 1 || idx > enemynumber) return;
        EnemyData2& e = enemy[idx];

        // 파스칼 원본의 색상 결정 로직 완벽 복구
        int color = 10;
        if (e.hp == 0) color = 8;
        else if (e.hp < 200) color = 12;
        else if (e.hp < 500) color = 4;
        else if (e.hp < 1000) color = 6;
        else if (e.hp < 2000) color = 14;
        else if (e.hp < 4000) color = 2;

        if (e.E_number == 0 || (e.specialcastlevel & 0x80) > 0) color = 10;
        if (e.unconscious) color = 8;
        if (e.dead) color = 0;

        if (selected) {
            SetColor(7); // 선택됨: 회색 막대 배경
            Bar(60, 20 * idx + 48, 198, 20 * idx + 64);
            SetColor(color == 0 ? 7 : color);
        }
        else {
            SetColor(0); // 선택 해제: 검은색 배경으로 덮기
            Bar(60, 20 * idx + 48, 198, 20 * idx + 64);
            SetColor(color); // 원래 글자색 복구
        }
        HPrintXY4Select(70, 20 * idx + 50, e.name.c_str());
        };

    // 진입 전 찌꺼기 이벤트 비우기 (오작동 방지)
    SDL_Event ev_flush;
    while (SDL_PollEvent(&ev_flush)) {}

    // 초기 1번 적 선택 상태 그리기
    draw_enemy(number, true);

    while (!sel_ok) {
        SDL_Event ev_step;
        while (SDL_PollEvent(&ev_step)) {
            if (ev_step.type == SDL_KEYDOWN) {
                int y_dir = 0;
                switch (ev_step.key.keysym.sym) {
                case SDLK_UP: y_dir = -1; break;
                case SDLK_DOWN: y_dir = 1; break;
                case SDLK_RETURN:
                case SDLK_SPACE: sel_ok = true; break;
                case SDLK_ESCAPE: sel_ok = true; number = 0; break;
                }

                if (y_dir != 0) {
                    // 1. 이동 전 현재 위치의 선택 막대 해제
                    draw_enemy(number, false);

                    // 2. 인덱스 이동
                    number += y_dir;
                    if (number < 1) number = enemynumber;
                    if (number > enemynumber) number = 1;

                    // 3. 새 위치에 선택 막대 렌더링
                    draw_enemy(number, true);
                    needs_redraw = true; // 변경 사항이 생겼으므로 화면 갱신 예약
                }
            }
            else if (ev_step.type == SDL_QUIT) {
                std::exit(0);
            }
        }

        // 상태가 변경되었을 때만 UpdateScreen 호출 (병목 해소)
        if (needs_redraw) {
            UpdateScreen();
            needs_redraw = false;
        }
        SDL_Delay(10);
    }

    // [파스칼 원본 복구] : 엔터(확정)를 눌렀을 때, 현재 선택되어 있던 적의 회색 막대 배경을 없앰
    if (number > 0) {
        draw_enemy(number, false);
        UpdateScreen();
    }

    SetColor(8); // SetFillStyle(1,8); 파스칼 마무리 대응
    return number;
}

void FindGold(longint money) {
    char buf[128];
    std::snprintf(buf, sizeof(buf), "당신은 금화 %d개를 발견했다.", money);
    message(7, buf);
    party.gold += money;
}

