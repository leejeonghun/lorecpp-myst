#include "uhanx.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cstdint>

const uint8_t johab_leads[19] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
const uint8_t johab_vowels[21] = { 3, 4, 5, 6, 7, 10, 11, 12, 13, 14, 15, 18, 19, 20, 21, 22, 23, 26, 27, 28, 29 };
const uint8_t johab_trails[28] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };

uint8_t FontData[224][32];
uint8_t AsciiFontData[96][16]; 

SDL_Renderer* g_Renderer = nullptr;
SDL_Texture* g_VirtualScreen = nullptr; 
uint8_t g_IndexBuffer[640 * 480]; 
int hanx = 0, hany = 0;
int g_HWinX1 = 0, g_HWinY1 = 0, g_HWinX2 = 640, g_HWinY2 = 480;
int g_CurrentColor = 15;
bool g_ScrollPause = true;

SDL_Color VGA_Palette[16] = {
    {0,0,0,255}, {0,0,170,255}, {0,170,0,255}, {0,170,170,255},
    {170,0,0,255}, {170,0,170,255}, {170,85,0,255}, {170,170,170,255},
    {85,85,85,255}, {85,85,255,255}, {85,255,85,255}, {85,255,255,255},
    {255,85,85,255}, {255,85,255,255}, {255,255,85,255}, {255,255,255,255}
};



void SetFont(int num) {
    std::string filename = "LORE" + std::to_string(num) + ".FNT";
    std::ifstream file("assets/" + filename, std::ios::binary);
    if (file) { 
        file.read(reinterpret_cast<char*>(FontData), sizeof(FontData));
    }
}

#include <ctime>

void InitHan(SDL_Renderer* renderer) {
    std::srand(std::time(NULL));
    g_Renderer = renderer;
    std::ifstream asciiFile("assets/ASCII.FNT", std::ios::binary);
    if (asciiFile) { asciiFile.read(reinterpret_cast<char*>(AsciiFontData), sizeof(AsciiFontData)); }
    if (g_VirtualScreen) SDL_DestroyTexture(g_VirtualScreen);
    g_VirtualScreen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
    std::memset(g_IndexBuffer, 0, 640 * 480);
    SetFont(0);
    Pause(true); // scrollpause := On
    
    // 원본 파스칼 메인 루틴에 정의된 기본 윈도우 크기로 초기화 설정
    HWindow(251, 32, 608, 257);
}

void HWindow(int x1, int y1, int x2, int y2) { g_HWinX1 = x1; g_HWinY1 = y1; g_HWinX2 = x2; g_HWinY2 = y2; hanx = x1; hany = y1; }
void HGotoXY(int x, int y) { hanx = x; hany = y; }
void Pause(bool ok) { g_ScrollPause = ok; }
void SetColor(int colorIndex) { g_CurrentColor = colorIndex % 16; }
void SetPalette(int index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= 0 && index < 16) {
        int ri = (r * 255) / 63;
        int gi = (g * 255) / 63;
        int bi = (b * 255) / 63;
        if (ri < 0) ri = 0; if (ri > 255) ri = 255;
        if (gi < 0) gi = 0; if (gi > 255) gi = 255;
        if (bi < 0) bi = 0; if (bi > 255) bi = 255;
        VGA_Palette[index] = {(uint8_t)ri, (uint8_t)gi, (uint8_t)bi, 255};
    }
}
int GetColor() { return g_CurrentColor; }

void PutPixel(int x, int y, int colorIndex) {
    if (x < 0 || x >= 640 || y < 0 || y >= 480) return;
    g_IndexBuffer[y * 640 + x] = (uint8_t)colorIndex;
}

void Bar(int x1, int y1, int x2, int y2) {
    for(int r = y1; r <= y2; r++) {
        if (r < 0 || r >= 480) continue;
        for(int c_idx = x1; c_idx <= x2; c_idx++) {
            if (c_idx < 0 || c_idx >= 640) continue;
            g_IndexBuffer[r * 640 + c_idx] = (uint8_t)g_CurrentColor;
        }
    }
}

// 화면 스크롤 기능 및 키 대기 (원본 파스칼의 hscroll 어셈블리 루틴 포팅)
void HScroll() {
    if (g_ScrollPause) {
        UpdateScreen(); // 스크롤 대기 전 현재까지의 화면 표시
        SDL_Event ev;
        bool waiting = true;
        while (waiting) {
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_KEYDOWN || ev.type == SDL_QUIT) {
                    waiting = false;
                    break;
                }
            }
            SDL_Delay(10); // CPU 점유율 감소를 위해 10ms 대기
        }
    }

    // 픽셀 데이터 16픽셀 상단으로 시프트 (파스칼 원본 ASM: x:248~631, y:48~310 영역을 복사)
    // 범용성을 위해 텍스트 윈도우 출력 범위를 기준으로 시프트 합니다.
    for (int y = 32; y <= 294; y++) {
        int srcY = y + 16;
        if (srcY < 480) {
            for (int x = 245; x <= 631; x++) {
                g_IndexBuffer[y * 640 + x] = g_IndexBuffer[srcY * 640 + x];
            }
        }
    }

    // 아랫부분을 배경색(8번 색상 Dark Gray)으로 지움 (원본 : setfillstyle(1,8); Bar(245,255,610,288))
    int oldColor = GetColor();
    SetColor(8);
    Bar(245, 255, 610, 288);
    SetColor(oldColor);
}

void PutHanInternal(int x, int y, uint8_t first, uint8_t second, uint8_t third) {
    int num1 = 0;
    if (second < 13 || second == 29) num1 = 0; 
    else if (second == 13 || second == 19 || second == 20 || second == 26 || second == 27) num1 = 1;
    else num1 = 2;
    if (third > 1) num1 += 3;
    int f1 = std::max(0, std::min(223, 6 * (first - 2) + num1));
    int f2 = std::max(0, std::min(223, 2 * (second - 3) + 114 + (third > 1 ? 1 : 0)));
    int n3 = (second == 13 || second == 19 || second == 20 || second == 26 || second == 27) ? 1 : 0;
    int f3 = std::max(0, std::min(223, 2 * (third - 2) + 168 + n3));
    for (int r = 0; r < 16; r++) {
        uint8_t b1 = (first > 1 ? FontData[f1][r*2] : 0) | (second > 2 ? FontData[f2][r*2] : 0) | (third > 1 ? FontData[f3][r*2] : 0);
        uint8_t b2 = (first > 1 ? FontData[f1][r*2+1] : 0) | (second > 2 ? FontData[f2][r*2+1] : 0) | (third > 1 ? FontData[f3][r*2+1] : 0);
        for (int b = 0; b < 8; b++) {
            if ((b1 >> (7-b)) & 1) PutPixel(x + b, y + r, g_CurrentColor);
            if ((b2 >> (7-b)) & 1) PutPixel(x + 8 + b, y + r, g_CurrentColor);
        }
    }
}

// 2배 크기로 출력하는 한글 (puthan_extension 대응)
void PutHanExtensionInternal(int x, int y, uint8_t first, uint8_t second, uint8_t third) {
    int num1 = 0;
    if (second < 13 || second == 29) num1 = 0; 
    else if (second == 13 || second == 19 || second == 20 || second == 26 || second == 27) num1 = 1;
    else num1 = 2;
    if (third > 1) num1 += 3;
    int f1 = std::max(0, std::min(223, 6 * (first - 2) + num1));
    int f2 = std::max(0, std::min(223, 2 * (second - 3) + 114 + (third > 1 ? 1 : 0)));
    int n3 = (second == 13 || second == 19 || second == 20 || second == 26 || second == 27) ? 1 : 0;
    int f3 = std::max(0, std::min(223, 2 * (third - 2) + 168 + n3));
    for (int r = 0; r < 16; r++) {
        uint8_t b1 = (first > 1 ? FontData[f1][r*2] : 0) | (second > 2 ? FontData[f2][r*2] : 0) | (third > 1 ? FontData[f3][r*2] : 0);
        uint8_t b2 = (first > 1 ? FontData[f1][r*2+1] : 0) | (second > 2 ? FontData[f2][r*2+1] : 0) | (third > 1 ? FontData[f3][r*2+1] : 0);
        for (int b = 0; b < 8; b++) {
            if ((b1 >> (7-b)) & 1) {
                PutPixel(x + b*2, y + r*2, g_CurrentColor);         PutPixel(x + b*2 + 1, y + r*2, g_CurrentColor);
                PutPixel(x + b*2, y + r*2 + 1, g_CurrentColor);     PutPixel(x + b*2 + 1, y + r*2 + 1, g_CurrentColor);
            }
            if ((b2 >> (7-b)) & 1) {
                PutPixel(x + 16 + b*2, y + r*2, g_CurrentColor);    PutPixel(x + 16 + b*2 + 1, y + r*2, g_CurrentColor);
                PutPixel(x + 16 + b*2, y + r*2 + 1, g_CurrentColor);PutPixel(x + 16 + b*2 + 1, y + r*2 + 1, g_CurrentColor);
            }
        }
    }
}

void PutAsciiInternal(int x, int y, char c) {
    int order = (unsigned char)c;
    // 파스칼 원본 asciixy 로직 정확히 반영:
    //   if order < 27  then order := 31  → AsciiFont[31-31] = AsciiFont[0]  (폰트 특수문자)
    //   if order > 126 then order := 32  → AsciiFont[32-31] = AsciiFont[1]  (공백)
    int f_idx;
    if (order < 27)       f_idx = 0;        // chr(1)~chr(26): 폰트 index 0의 특수문자로 출력
    else if (order > 126) f_idx = 1;        // 127 이상: 공백(index 1)으로 처리
    else                  f_idx = order - 31;
    f_idx = std::max(0, std::min(95, f_idx));
    for (int r = 0; r < 16; r++) {
        uint8_t row = AsciiFontData[f_idx][r];
        for (int b = 0; b < 8; b++) if ((row >> (7-b)) & 1) PutPixel(x + b, y + r, g_CurrentColor);
    }
}

// 2배 크기로 출력하는 영문/기호 (asciixy_extension 대응)
void PutAsciiExtensionInternal(int x, int y, char c) {
    int order = (unsigned char)c;
    // 파스칼 원본 asciixy_extension 로직 정확히 반영 (asciixy와 동일한 index 계산)
    int f_idx;
    if (order < 27)       f_idx = 0;
    else if (order > 126) f_idx = 1;
    else                  f_idx = order - 31;
    f_idx = std::max(0, std::min(95, f_idx));
    for (int r = 0; r < 16; r++) {
        uint8_t row = AsciiFontData[f_idx][r];
        for (int b = 0; b < 8; b++) {
            if ((row >> (7-b)) & 1) {
                PutPixel(x + b*2, y + r*2, g_CurrentColor);         PutPixel(x + b*2 + 1, y + r*2, g_CurrentColor);
                PutPixel(x + b*2, y + r*2 + 1, g_CurrentColor);     PutPixel(x + b*2 + 1, y + r*2 + 1, g_CurrentColor);
            }
        }
    }
}

uint32_t GetCodepoint(const char*& p) {
    uint8_t c1 = (uint8_t)*p++;
    if (c1 == 0) return 0;
    if (c1 >= 0x80) {
        uint8_t c2 = (uint8_t)*p++;
        if (c2 == 0) return '?';
        if ((c1 & 0xE0) == 0xC0) { uint32_t cp = (c1 & 0x1F) << 6; cp |= (c2 & 0x3F); return cp; }
        if ((c1 & 0xF0) == 0xE0) { uint8_t c3 = (uint8_t)*p++; uint32_t cp = (c1 & 0x0F) << 12; cp |= (c2 & 0x3F) << 6; cp |= (c3 & 0x3F); return cp; }
        uint8_t first = (c1 >> 2) & 0x1F;
        uint8_t second = ((c1 & 0x03) << 3) | ((c2 >> 5) & 0x07);
        uint8_t third = c2 & 0x1F;
        return 0x80000000 | ((uint32_t)first << 16) | ((uint32_t)second << 8) | (uint32_t)third;
    }
    return c1;
}

// 핵심 출력 로직 : 스크롤 판정 추가
void HPrintsub(const char* str, bool scrolling) {
    // 키보드 버퍼 비우기 모방 (while keypressed do c := readkey;)
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {}

    if (scrolling && (hany >= g_HWinY2)) {
        HScroll();
        hany = 256; // 스크롤 후 새로운 라인의 Y축 좌표 설정
    }

    const char* p = str;
    while (*p) {
        uint32_t cp = GetCodepoint(p);
        if (cp == 0) break;

        // 글자가 가로 윈도우 범위를 벗어나면 줄바꿈 처리
        if (hanx + 8 >= g_HWinX2) { 
            hanx = g_HWinX1; 
            hany += 16; 
            // 줄바꿈 시 스크롤 여부 체크
            if (scrolling && (hany >= g_HWinY2)) {
                HScroll();
                hany = 256;
            }
        }

        if (cp >= 0x80000000) {
            uint8_t l = (cp >> 16) & 0xFF; uint8_t v = (cp >> 8) & 0xFF; uint8_t t = cp & 0xFF;
            PutHanInternal(hanx, hany, l, v, t); 
            hanx += 16;
        } else if (cp >= 0xAC00 && cp <= 0xD7A3) {
            int idx = cp - 0xAC00;
            int l = idx / (21 * 28), v = (idx % (21 * 28)) / 28, t = idx % 28;
            if (l < 19 && v < 21 && t < 28) PutHanInternal(hanx, hany, johab_leads[l], johab_vowels[v], johab_trails[t]);
            hanx += 16;
        } else { 
            PutAsciiInternal(hanx, hany, (char)cp); 
            hanx += 8; 
        }
    }
}

void HPrint(const char* str) { 
    HPrintsub(str, true); 
}

void HPrintXY(int x, int y, const char* str) { 
    HGotoXY(x, y); 
    HPrintsub(str, true); 
    hany += 16; 
    if (hany >= g_HWinY2) { 
        HScroll(); 
        hany = 256; 
    } 
    hanx = g_HWinX1; 
}

void HPrintXY4Select(int x, int y, const char* str) { 
    HGotoXY(x, y); 
    HPrintsub(str, false); 
    hany += 16; 
    hanx = g_HWinX1; 
}

void OutHPrintXY(int x, int y, const char* str, bool extension) {
    int pre_x = hanx; 
    int pre_y = hany; 
    HGotoXY(x, y); 
    const char* p = str;
    while (*p) {
        uint32_t cp = GetCodepoint(p);
        if (cp == 0) break;
        
        if (cp >= 0x80000000) {
            uint8_t l = (cp >> 16) & 0xFF; uint8_t v = (cp >> 8) & 0xFF; uint8_t t = cp & 0xFF;
            if (extension) PutHanExtensionInternal(hanx, hany, l, v, t);
            else PutHanInternal(hanx, hany, l, v, t); 
            hanx += (extension ? 32 : 16);
        } else if (cp >= 0xAC00 && cp <= 0xD7A3) {
            int idx = cp - 0xAC00; int l = idx / (21 * 28), v = (idx % (21 * 28)) / 28, t = idx % 28;
            if (extension) PutHanExtensionInternal(hanx, hany, johab_leads[l], johab_vowels[v], johab_trails[t]);
            else PutHanInternal(hanx, hany, johab_leads[l], johab_vowels[v], johab_trails[t]); 
            hanx += (extension ? 32 : 16);
        } else { 
            if (extension) PutAsciiExtensionInternal(hanx, hany, (char)cp);
            else PutAsciiInternal(hanx, hany, (char)cp); 
            hanx += (extension ? 16 : 8); 
        }
    }
    hanx = pre_x; 
    hany = pre_y;
}

void bHPrint(int x, int y, const char* str) { OutHPrintXY(x, y, str, false); OutHPrintXY(x+1, y, str, false); }
void cHPrint(int x, int y, const char* str) { int old = g_CurrentColor; if (g_CurrentColor > 8) { SetColor(g_CurrentColor - 8); OutHPrintXY(x+2, y+1, str, false); SetColor(old); } OutHPrintXY(x, y, str, false); }
void eHPrint(int x, int y, const char* str) { OutHPrintXY(x, y, str, true); }
bool HasBatchim(const std::string& s) { if (s.empty()) return false; const char* p = s.c_str(); uint32_t cp = 0, last_cp = 0; while (*p) { cp = GetCodepoint(p); if (cp >= 0xAC00 && cp <= 0xD7A3) last_cp = cp; } if (last_cp >= 0xAC00 && last_cp <= 0xD7A3) return (last_cp - 0xAC00) % 28 != 0; return false; }

void UpdateScreen() {
    if (!g_Renderer || !g_VirtualScreen) return;
    void* pixels; int pitch;
    if (SDL_LockTexture(g_VirtualScreen, NULL, &pixels, &pitch) < 0) return;
    for(int y=0; y < 480; y++) {
        uint32_t* row = (uint32_t*)((uint8_t*)pixels + y * pitch);
        for(int x=0; x < 640; x++) {
            SDL_Color& c = VGA_Palette[g_IndexBuffer[y * 640 + x] % 16];
            row[x] = (255U << 24) | ((uint32_t)c.r << 16) | ((uint32_t)c.g << 8) | (uint32_t)c.b;
        }
    }
    SDL_UnlockTexture(g_VirtualScreen);
    SDL_SetRenderTarget(g_Renderer, NULL);
    SDL_RenderCopy(g_Renderer, g_VirtualScreen, NULL, NULL);
    SDL_RenderPresent(g_Renderer);
}

void PutHan(int x, int y, int order) {
    int font_idx = std::max(0, std::min(223, order));
    for (int r = 0; r < 16; r++) {
        uint8_t b1 = FontData[font_idx][r*2]; uint8_t b2 = FontData[font_idx][r*2+1];
        for (int b = 0; b < 8; b++) {
            if ((b1 >> (7-b)) & 1) PutPixel(x + b, y + r, g_CurrentColor);
            if ((b2 >> (7-b)) & 1) PutPixel(x + 8 + b, y + r, g_CurrentColor);
        }
    }
}

void AsciiXY(int x, int y, int order) { PutAsciiInternal(x, y, (char)order); }

void Delay(Uint32 delay_ms) {
    const Uint32 start_time = SDL_GetTicks();

    UpdateScreen();
    while (SDL_GetTicks() - start_time < delay_ms) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit(0);
            }
        }
        SDL_Delay(10);
    }
}
void Line(int x1, int y1, int x2, int y2) {
    if (!g_Renderer || !g_VirtualScreen) return;
    int co = GetColor();
    // Simple Bresenham or SDL_RenderDrawLine if using index buffer?
    // Since we use index buffer, we need a custom line drawer or update bar.
    // For horizontal/vertical lines (which these are), Bar works.
    if (x1 == x2) { // Vertical
        Bar(x1, std::min(y1, y2), x1, std::max(y1, y2));
    } else if (y1 == y2) { // Horizontal
        Bar(std::min(x1, x2), y1, std::max(x1, x2), y1);
    } else {
        // Diagonal not needed for this specific task, but good to note.
    }
}