#include "mystend.h"
#include "mystsub.h"
#include "uhanx.h"
#include "adlib.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <cstdlib>

const int Back_Ground_Color = 8;     // DarkGray
const int Fade_Top_Color = 3;        // Cyan
const int Fade_Bottom_Color = 2;     // Green
const int Main_Scroll_Color = 11;    // LightCyan
const int Str_X_Pos = 71;
const int Str_Y_Pos = 370;
const int Max_Line = 8;
const int Word_Y_Size = 16;
const int Title_Str_Total = 29;

const char* End_Title_Str[Title_Str_Total] = {
    " 새로운 로드안이 된 당신에게.",
    "",
    " 역시 당신은 나의 기대를 저버리지 않았소. 나 알비레오는 그저 로",
    "어세계의 역사에 대해  방관자적인 입장일뿐이었지만  비뚤어져가는",
    "로어세계의 미래를 그냥 보고만 있을 수는 없어서  당신을 선택하게",
    "되었고 당신을 이런 모험에 말려 들게 했던 것이오.  이제 위선자는",
    "사라졌고  로드안의 칭호도 당신에게 돌아갔소.  하지만 당신에게는",
    "아직 2가지의 과제가 더 남아있소.  첫째는 네크로만서가 된 카노푸",
    "스가 이땅에 내려올때 벌어지게 되고  둘째는 멸망당한 네종족의 원",
    "혼이 부활할 때 발생하게 될거요.  이 일들은 약 100년 후에 발생할",
    "것이오. 하지만 당신이라면 충분히 이겨낼 수 있을거라 믿소.",
    "",
    " 나는  이제 이 세계를 떠날 때가 되었소.  아직도 내가 가보아야할",
    "차원이 수없이 많이있기 때문이오.  로어 세계가 있는 이곳도  그저",
    "내가 스쳐갔던 수 많은 차원들 중의 하나로  기억될지도 모르겠지만",
    "쉽게 잊어 버리기에는 너무 아까운 기억이 많이있소. 그중의 하나는",
    "당신의 존재 때문일거라오.  이제 나는 이곳 역사에 더 이상 신경을",
    "쓰지 않으려고 하오. 앞으로 다시 올 계획도 아직은 없소.  그저 영",
    "원의 차원으로 나의 여행을 계속 할 뿐이라오.",
    "",
    "                                      타임워커 알비레오로 부터.",
    "","","","","","","",""
};

extern uint8_t g_IndexBuffer[640 * 480];

// 화면의 일부분을 1픽셀 위로 복사하여 스크롤 효과를 내는 어셈블리(rep movsb) 대응 함수
static void Scroll_Sub(int X_First, int X_Length, int Y_Start, int Y_End) {
    int offset_x = X_First * 8; 
    int pixel_width = X_Length * 8;
    
    for (int y = Y_Start; y <= Y_End; y++) {
        if (y - 1 >= 0) {
            std::memcpy(&g_IndexBuffer[(y - 1) * 640 + offset_x], &g_IndexBuffer[y * 640 + offset_x], pixel_width);
        }
    }
    // 복사 후 맨 아랫줄 배경색으로 지우기
    std::memset(&g_IndexBuffer[Y_End * 640 + offset_x], Back_Ground_Color, pixel_width);
}

// 팔레트 6번 색상을 조작하여 번개 치는 효과 구현
static void ThunderEffect() {
    SetPalette(6, 40, 20, 0); // 기본 갈색/주황색 톤
    SDL_Event ev;
    bool quit = false;

    // 버퍼 비우기
    while(SDL_PollEvent(&ev)) {}

    while (!quit) {
        if (std::rand() % 1000 == 0) {
            if (std::rand() % 100 == 0) {
                SetPalette(6, 10, 20, 63); // 번개 (밝은 파랑/흰색)
                UpdateScreen();
                SDL_Delay((std::rand() % 100) + 20);
                SetPalette(6, 40, 20, 0);    // 원상 복구
                UpdateScreen();
            }
        }
        while (SDL_PollEvent(&ev)) { 
            if (ev.type == SDL_KEYDOWN) { quit = true; break; } 
        }
        SDL_Delay(10);
    }
}

// 도스 텍스트 모드 전환을 SDL 그래픽 상에서 재현하는 엔딩 메시지 함수
static void End_Message() {
     if (AdLibOn) PlayOff();
     UnSound();

    // 화면 흑백 클리어
    int old_color = GetColor();
    SetColor(0);
    Bar(0, 0, 639, 479);
    SetColor(old_color);

    // 텍스트를 출력할 색상의 팔레트를 완전한 검은색으로 초기화
    SetPalette(7, 0, 0, 0);
    SetPalette(15, 0, 0, 0);

    // 텍스트 배치
    SetColor(15);
    OutHPrintXY(200, 150, "             << The End >>", false);
    OutHPrintXY(90, 200, "\" Into the MYSTIC  vol. #3 \" is made by AHN YOUNG-KI", false);
    SetColor(7);
    OutHPrintXY(200, 250, "        You must be a genius !!!", false);

    // 텍스트 서서히 밝게 (Fade In)
    for (int i = 1; i <= 63; i++) {
        SetPalette(7, i, i, i);
        SetPalette(15, i, i, i);
        UpdateScreen();
        SDL_Delay(10);
    }
    
    SDL_Delay(500);
    
    // 텍스트 서서히 어둡게 (Fade Out - 7번 색상만)
    for (int i = 62; i >= 42; i--) {
        SetPalette(7, i, i, i);
        UpdateScreen();
        SDL_Delay(15);
    }

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN) {
                quit = true;
                break;
            }
        }
        UpdateScreen();
        SDL_Delay(10);
    }

    // 프로그램 완전 종료
    std::exit(0);
}

// 화면 전체 페이드 아웃 (기존 파스칼의 All_Fade_Out 어셈블리 루틴 대체)
static void End_Demo_Sub1() {
    Screen_Fade_Out(2000); 
    int old_color = GetColor();
    SetColor(0);
    Bar(0, 0, 639, 479);
    SetColor(old_color);
    UpdateScreen();
}

// 크레딧 텍스트 스크롤 연출
static void End_Demo_Sub2() {
    End_Demo_Sub1();
    int j = 0;
    
    SetPalette(Back_Ground_Color, 0, 0, 0);
    SetPalette(Main_Scroll_Color, 63, 0, 0);
    
    bool skip = false;
    while (j < Title_Str_Total && !skip) {
        j++;
        SetPalette(Fade_Bottom_Color, 0, 0, 0);
        SetPalette(Fade_Top_Color, 63, 0, 0);
        
        int add = 1;
        for (add = 1; add <= Word_Y_Size; add++) {
            Scroll_Sub(Str_X_Pos / 8, 64, Str_Y_Pos - Word_Y_Size * (Max_Line + 1), Str_Y_Pos + 20);
            
            if (add == 1) {
                SetColor(Fade_Bottom_Color);
                OutHPrintXY(Str_X_Pos, Str_Y_Pos - add, End_Title_Str[j - 1], false);
                SetColor(Fade_Top_Color);
                if (j - 1 >= Max_Line) {
                    OutHPrintXY(Str_X_Pos, Str_Y_Pos - Word_Y_Size * (Max_Line + 1) + (Word_Y_Size - add), End_Title_Str[j - 1 - Max_Line], false);
                }
            }
            
            SetPalette(Fade_Bottom_Color, (int)(add), 0, 0);
            SetPalette(Fade_Top_Color, (int)(64 - add), 0, 0);
            
            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_KEYDOWN) {
                    skip = true;
                    break;
                }
            }
            
            if (skip) break;

            UpdateScreen();
            SDL_Delay(150);
        }
        
        if (!skip) {
            // C++ for 루프 종료 후 add는 17이 되므로 파스칼과 동일하게 맞추기 위해 1을 뺌
            int final_add = add - 1;

            SetColor(Main_Scroll_Color);
            OutHPrintXY(Str_X_Pos, Str_Y_Pos - final_add, End_Title_Str[j - 1], false);

            SetColor(Back_Ground_Color);
            if (j - 1 >= Max_Line) {
                OutHPrintXY(Str_X_Pos, Str_Y_Pos - Word_Y_Size * (Max_Line + 1) + (Word_Y_Size - final_add), End_Title_Str[j - 1 - Max_Line], false);
            }
        }
    }
}

// 최종 보스 클리어 후 나오는 장문의 에필로그 및 번개 이펙트 연출
static void End_Demo_Sub3() {
    End_Demo_Sub1();
    int x_pos = 40, y_pos = 60;
    
    SetColor(14);
    cHPrint(x_pos, y_pos, " 나는 로드안에게 마지막 일격을 가하려 검을 쳐 들었다. 하지만 쉽게"); y_pos += 20;
    cHPrint(x_pos, y_pos, "내리칠 수 있을것 같았지만  마음 속에서는 심한 동요가 일고 있었다."); y_pos += 20;
    cHPrint(x_pos, y_pos, "로드안 그는  나에게 돌이킬 수 없는 실수를 저지르게 했지만  이제는"); y_pos += 20;
    cHPrint(x_pos, y_pos, "다 끝난 일이었다.  네크로만서나 다크 메이지도 이제는 다 지나간 기"); y_pos += 20;
    cHPrint(x_pos, y_pos, "억 속으로 사라져 가고 있으니까 ... ..."); y_pos += 20;
    cHPrint(x_pos, y_pos, " 나는 검을 거두고는 뒤돌아섰다. 로드안은 너무나 인간을 사랑했기에"); y_pos += 20;
    cHPrint(x_pos, y_pos, "인간을 이롭게 하고자한 죄 밖에는 없다.  나 역시 보잘것없는 한명의"); y_pos += 20;
    cHPrint(x_pos, y_pos, "인간로써 과연 그를 심판할 자격이 있을까?"); y_pos += 20;
    cHPrint(x_pos, y_pos, ""); y_pos += 20;
    cHPrint(x_pos, y_pos, ""); y_pos += 20;
    cHPrint(x_pos, y_pos, " 나는 로어성을 뒤로한채 혼자만의 여행을 떠나고있다."); y_pos += 20;
    cHPrint(x_pos, y_pos, " 여태껏 잊고 지냈던 자신만의 시간을 가지고 싶었기 때문이다.  내가"); y_pos += 20;
    cHPrint(x_pos, y_pos, "어릴적에 하나의 꿈이 있었다.  그것은 저 하늘의 새처럼 자유롭게 먼"); y_pos += 20;
    cHPrint(x_pos, y_pos, "곳으로 여행을 떠나는 것이었다."); y_pos += 20;
    cHPrint(x_pos, y_pos, " 내 이름이 저들에게 잊혀질때 쯤이면  나는 여지껏 누구도 간적이 없"); y_pos += 20;
    cHPrint(x_pos, y_pos, "는 세상의 끝에 도달해 있을것이다. 그리고 거기서 나는 새로운 나 자"); y_pos += 20;
    cHPrint(x_pos, y_pos, "신을 찾고 또 다른 삶을 살아가는 나를 발견할 것이다."); y_pos += 20;
    
    UpdateScreen();
    ThunderEffect();
}

// 분기 컨트롤 메인 함수
void End_Demo(byte no) {
    switch (no) {
        case 1: End_Demo_Sub1(); break;
        case 2: End_Demo_Sub2(); break;
        case 3: End_Demo_Sub3(); break;
    }
    End_Message();
}
