#include "mysttitl.h"
#include "mystsub.h"
#include "uhanx.h"
#include "mystcret.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>

const int Image_Size = 14526;
const int Back_Ground_Color = 8;
const int Fade_Top_Color = 3;    
const int Fade_Bottom_Color = 2; 
const int Main_Scroll_Color = 11; 
const int Str_X_Pos = 71;
const int Str_Y_Pos = 370;
const int Max_Line = 8;
const int Word_Y_Size = 16;
const int Title_Str_Total = 29;
const int Select_Number = 5;

const char* Title_Str[] = {
    " 오래 전부터 만날 운명이었던 당신에게.",
    "",
    " 나는 시공의 여행자인 타임워커 알비레오라고 하오.  내가 로어 세",
    "계가 있는 공간으로 접어 들었을 때  마침 이 세계는 로드안의 통치",
    "에 있었소.  그 때 내가 그대들에게 닥쳐 올 불확실한 미래를  나의",
    "다섯 가지 예언으로 계시해 주었던 것을 기억 할거요. 그리고 그 예",
    "언이 너무나도 큰 희생을 강요했던 것도 기억하고 있을 거라 믿소.",
    "나는 세상을 구하기 위해 자신을 희생 해야만 했던 그를 이해하오.",
    "또한 숙명의 적이었던 네크로만서와 불멸의 다크메이지 실리안 카미",
    "너스 역시 나는 용서 할 수 있었소.  하지만 그들을 용서하는데에는",
    "대단한 용기를 감수해야만 했다오. 그들의 진심을 이해하기 위해 나",
    "는 서둘러 로어 세계의 과거로 도약을 시작했고 거기서 내가 보았던",
    "것은 네크로만서와 다크메이지의 계획된 탄생이었소.  내가 그 계기",
    "로 진정한 이 세계의 진실을 알게 되었을 때,  나 스스로가 이 세계",
    "에 대한 경멸로 마음을 닫아 버렸소.  그래서 나를 이해 할 수 있을",
    "것같은 당신에게 이렇게 도움을 청하는 거요.  당신이 네크로만서와",
    "다크메이지를 완전히 소멸 시키는 그 날, 이 세계는 다시 원래의 운",
    "명의 궤도로 접어들게 되고 나는 그때야 비로소 다른 공간으로의 여",
    "행을 다시 시작하게 될거요.",
    "",
    "                                     타임워커 알비레오로 부터",
    "","","","","","","",""
};

const char* Select_Str[] = {
    "새로운 캐릭터 만들기",
    "이전의 게임 계속하기",
    "전편의  줄거리  요약",
    "이 게임 제작의  개요",
    "  도스로  돌아가기  "
};

extern SDL_Renderer* g_Renderer;
extern SDL_Texture* g_VirtualScreen;
extern uint8_t g_IndexBuffer[640 * 480];
extern bool Loading_SaveGame; // 이어하기/새로하기 구분을 위한 전역 변수

static void Scroll_Sub(int x8, int width_bytes, int y1, int y2) {
    for (int r = y1; r < y2; r++) {
        std::memcpy(&g_IndexBuffer[r * 640 + x8 * 8], &g_IndexBuffer[(r + 1) * 640 + x8 * 8], width_bytes * 8);
    }
    int old = GetColor(); SetColor(Back_Ground_Color);
    Bar(x8 * 8, y2, x8 * 8 + width_bytes * 8 - 1, y2);
    SetColor(old);
}

static void PutImageRaw(int X, int Y, void* BitMap) {
    uint8_t* data = (uint8_t*)BitMap;
    uint16_t w_minus_1 = data[0] | (data[1] << 8);
    uint16_t h_minus_1 = data[2] | (data[3] << 8);
    int w = w_minus_1 + 1; int h = h_minus_1 + 1;
    if (w <= 0 || h <= 0 || w > 640 || h > 480) return;
    int bpr = (w + 7) / 8;
    for(int r=0; r<h; r++) {
        for(int c=0; c<w; c++) {
            int bit_idx = 7 - (c % 8);
            int offset = 4 + r * (bpr * 4) + (c / 8);
            uint8_t p3 = (data[offset + 0 * bpr] >> bit_idx) & 1;
            uint8_t p2 = (data[offset + 1 * bpr] >> bit_idx) & 1;
            uint8_t p1 = (data[offset + 2 * bpr] >> bit_idx) & 1;
            uint8_t p0 = (data[offset + 3 * bpr] >> bit_idx) & 1;
            uint8_t color = (p3 << 3) | (p2 << 2) | (p1 << 1) | p0;
            if(color > 0) PutPixel(X + c, Y + r, color);
        }
    }
}

static void Box(int x1, int y1, int x2, int y2, int color, bool shadow, bool bold) {
    // 1. 내부 바탕색 채우기
    SetColor(color);
    Bar(x1, y1, x2, y2);

    // 2. 외부 양각 테두리 (빛이 좌측 상단에서 오는 효과)
    // 좌측, 상단은 밝게 (15: White)
    SetColor(15);
    Bar(x1, y1, x1, y2); // 좌측 테두리
    Bar(x1, y1, x2, y1); // 상단 테두리

    // 우측, 하단은 어둡게 (8: DarkGray - 기존의 0(Black)에서 수정됨)
    SetColor(8);
    Bar(x2, y1, x2, y2); // 우측 테두리
    Bar(x1, y2, x2, y2); // 하단 테두리

    // 두꺼운 테두리일 경우 1픽셀 안쪽에 한 번 더 그림
    if (bold) {
        SetColor(15);
        Bar(x1 + 1, y1 + 1, x1 + 1, y2 - 1); // 좌측 안쪽
        Bar(x1 + 1, y1 + 1, x2 - 1, y1 + 1); // 상단 안쪽

        SetColor(8);
        Bar(x2 - 1, y1 + 1, x2 - 1, y2 - 1); // 우측 안쪽
        Bar(x1 + 1, y2 - 1, x2 - 1, y2 - 1); // 하단 안쪽
    }

    // 3. 내부 음각 프레임 (원본 파스칼 코드의 shadow 구현 방식)
    if (shadow) {
        int tab = 4;
        int pt = tab - 1; // 파스칼의 pred(tab) = 3

        // 음각이므로 빛의 반대 방향: 좌측/상단이 어둡고(8), 우측/하단이 밝음(15)
        SetColor(8);
        Bar(x1 + pt, y1 + pt, x1 + pt, y2 - pt); // 음각 좌측
        Bar(x1 + pt, y1 + pt, x2 - pt, y1 + pt); // 음각 상단

        SetColor(15);
        Bar(x2 - pt, y1 + pt, x2 - pt, y2 - pt); // 음각 우측
        Bar(x1 + pt, y2 - pt, x2 - pt, y2 - pt); // 음각 하단

        // 두꺼운 음각 프레임
        if (bold) {
            SetColor(8);
            Bar(x1 + tab, y1 + tab, x1 + tab, y2 - tab); // 음각 안쪽 좌측
            Bar(x1 + tab, y1 + tab, x2 - tab, y1 + tab); // 음각 안쪽 상단

            SetColor(15);
            Bar(x2 - tab, y1 + tab, x2 - tab, y2 - tab); // 음각 안쪽 우측
            Bar(x1 + tab, y2 - tab, x2 - tab, y2 - tab); // 음각 안쪽 하단
        }
    }
}

static void MessageBox(int x1, int y1, int x2, int y2, int color, bool shadow) {
    if(shadow) {
        Box(x1, y1, x2, y2, 8, shadow, false);
        Box(x1 + 2, y1 + 2, x2 - 2, y2 - 2, color, false, false);
    } else {
        Box(x1, y1, x2, y2, color, shadow, false);
    }
    SetColor(0);
    Bar(x1+1, y2+1, x2, y2+1);
    Bar(x2+1, y1+1, x2+1, y2+1);
}



static void Introduce_preGame() {
    // 원본 파스칼에서도 구현 내용이 없는 빈 프로시저입니다.
}

static void p(int x, int y, std::string s) {
    eHPrint(x * 16, y * 32, s.c_str());
}

static void w(int x, int y, std::string s) {
    cHPrint(x * 8, y * 16, s.c_str());
}

static void Introduce_Staff() {
    int old_color = GetColor();
    SetColor(0); Bar(0, 0, 639, 479); // ClearDevice 대체
    Restore_Palette();
    
    SetPalette(1, 10, 20, 40); // 파란색 계열 커스텀
    SetColor(1); Bar(0, 0, 640, 480);
    
    SetColor(15); p(2, 1, "시나리오 & 프로그래밍 & 그래픽");
    SetColor(14); p(3, 2, ": 안 영기");
    
    SetColor(15); p(2, 4, "배경 음악 & 음향 효과");
    SetColor(14); p(3, 5, ": 국내외의 실력있는 편곡자님들");
                  p(3, 6, "       ( 이름은 잘 모름 )");
    
    SetColor(15); p(2, 8, "도움을 주신분");
    SetColor(14); p(3, 9, ": 안 영훈");
    
    w(5, 25, "제작기간 : 1995 년 2 월 1 일 저녁부터     2 월 12 일 새벽까지");
    
    UpdateScreen();
    
    // 키 입력 대기
    SDL_Event ev;
    bool skip = false;
    while (!skip) {
        while(SDL_PollEvent(&ev)) {
            if(ev.type == SDL_KEYDOWN || ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_QUIT) {
                skip = true; break;
            }
        }
        SDL_Delay(10);
    }
    SetColor(old_color);
}

void Title_Menu() {
    bool program_exit = false;
    int select = 2; // Default: 이전의 게임 계속하기

    while (!program_exit) {
        SetFont(0);
        
        // 뒷 배경 박스 그리기
        SetPalette(1, 15, 5, 25); // Blue Custom
        for (int i = 0; i <= 10; i++) {
            Box(200 - i * 20, 200 - i * 20, 438 + i * 20, 278 + i * 20, 8, true, true);

            // 추가된 부분: 도스 시절의 렌더링 지연 효과 모방
            UpdateScreen(); // 지금까지 그린 박스를 화면에 출력
            SDL_Delay(30);  // 30ms 대기 (0.03초). 취향에 따라 20~50 정도로 조절하세요.
        }

        // 초기 팔레트 어둡게 설정
        SetPalette(0, 0, 0, 0); // Black
        SetPalette(Back_Ground_Color, 15, 15, 15); 
        SetPalette(4, 10, 10, 10); // Red Custom
        SetPalette(5, 15, 15, 15); // Magenta
        SetPalette(13, 15, 15, 15); // LightMagenta

        std::ifstream f("assets/TITLE.DAT", std::ios::binary);
        if(f) {
            std::vector<uint8_t> data(Image_Size);
            f.read((char*)data.data(), Image_Size);
            PutImageRaw(85, 100, data.data());
        }

        // 이미지 팔레트 페이드 인 연출
        for (int i = 100; i >= 40; i--) {
            Scroll_Sub(9, 63, 40, 180);
            SetPalette(5, ((100 - i) / 4 + 15), 15, ((100 - i) / 4 + 15));
            SetPalette(13, ((100 - i) * 3 / 4 + 15), ((100 - i) / 4 + 15), ((100 - i) * 3 / 4 + 15));
            UpdateScreen();
            SDL_Delay(10);
        }

        // 텍스트 스크롤 및 페이드 연출
        bool skip_text = false;
        for(int j = 0; j < Title_Str_Total; j++) {
            SetPalette(Fade_Bottom_Color, 15, 15, 15);
            SetPalette(Fade_Top_Color, 0, 63, 63);

            int add = 1;
            for(add = 1; add <= Word_Y_Size; add++) {
                Scroll_Sub(Str_X_Pos / 8, 64, Str_Y_Pos - Word_Y_Size * (Max_Line + 1), Str_Y_Pos + 20);
                
                if (add == 1) {
                    SetColor(Fade_Bottom_Color);
                    OutHPrintXY(Str_X_Pos, Str_Y_Pos - add, Title_Str[j], false);
                    if (j >= Max_Line) {
                        SetColor(Fade_Top_Color);
                        OutHPrintXY(Str_X_Pos, Str_Y_Pos - Word_Y_Size * (Max_Line + 1) + (Word_Y_Size - add), Title_Str[j - Max_Line], false);
                    }
                }
                
                SetPalette(Fade_Bottom_Color, (16 - add), (15 + add * 3), (15 + add * 3));
                SetPalette(Fade_Top_Color, add, (63 - add * 3), (63 - add * 3));
                
                UpdateScreen();
                
                SDL_Event ev;
                while(SDL_PollEvent(&ev)) {
                    if(ev.type == SDL_KEYDOWN || ev.type == SDL_MOUSEBUTTONDOWN) { 
                        skip_text = true; break; 
                    }
                }
                if (skip_text) break;
                SDL_Delay(60); 
            }
            if (skip_text) break;

            // C++ for 루프 종료 후 add는 17이 되므로 파스칼과 동일하게 맞추기 위해 1을 뺌
            int final_add = add - 1;

            SetColor(Main_Scroll_Color);
            if (j < Title_Str_Total) OutHPrintXY(Str_X_Pos, Str_Y_Pos - final_add, Title_Str[j], false);
            if (j >= Max_Line && (j - Max_Line) < Title_Str_Total) {
                SetColor(Back_Ground_Color);
                OutHPrintXY(Str_X_Pos, Str_Y_Pos - Word_Y_Size * (Max_Line + 1) + (Word_Y_Size - final_add), Title_Str[j - Max_Line], false);
            }
        }

        // 메뉴창 띄우기
        SDL_Event ev_flush; while(SDL_PollEvent(&ev_flush)) {};
        
        MessageBox(40, 210, 600, 400, 4, true); 
        
        auto RedrawMenu = [&](int sel) {
            SetColor(4); Bar(43, 213, 597, 397); // 내부 클리어
            
            SetColor(0); // Deep Black Shadow
            for (int i = 0; i < Select_Number; i++) {
                OutHPrintXY(230, i * 20 + 260, Select_Str[i], false);
            }

            for (int i = 0; i < Select_Number; i++) {
                if (i + 1 == sel) SetColor(14); // Yellow
                else SetColor(9); // LightBlue
                cHPrint(225, i * 20 + 253, Select_Str[i]);
            }
            UpdateScreen();
        };

        RedrawMenu(select);

        bool selection_made = false;
        while(!selection_made) {
            SDL_Event ev;
            while(SDL_PollEvent(&ev)) {
                if(ev.type == SDL_KEYDOWN) {
                    switch(ev.key.keysym.sym) {
                        case SDLK_UP: select = (select > 1) ? select - 1 : Select_Number; break;
                        case SDLK_DOWN: select = (select < Select_Number) ? select + 1 : 1; break;
                        case SDLK_RETURN: case SDLK_SPACE:
                            selection_made = true;
                            break;
                    }
                    RedrawMenu(select);
                } else if (ev.type == SDL_QUIT) std::exit(0);
            }
            SDL_Delay(10);
        }

        // 메뉴 동작 분기 (원본 파스칼 구조 완벽 호환)
        if (select == 1) {
            // 새로운 캐릭터 만들기
            if (!CreateCharacter()) {
                // 취소 시 타이틀 루프 처음으로 돌아감
                continue; 
            }
            Loading_SaveGame = false;
            break; // 루프를 빠져나가면 main() 으로 돌아가 Set_All() 호출
        } 
        else if (select == 2) {
            // 이전의 게임 계속하기 (로딩)
            Loading_SaveGame = true; // Set_All() 에서 save1.m / player1.dat 를 불러오도록 지시
            break; 
        } 
        else if (select == 3) {
            Introduce_preGame();
            continue;
        } 
        else if (select == 4) {
            Introduce_Staff();
            continue;
        } 
        else if (select == 5) {
            std::exit(0);
        }
    }
    
    // 타이틀 루프 종료 시 화면 정리
    SetColor(0); Bar(0, 0, 639, 479);
    Restore_Palette();
    UpdateScreen();
}
