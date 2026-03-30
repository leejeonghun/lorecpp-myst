#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include "uhanx.h"
#include "mystsub.h"
#include "mystmain.h"
#include "mysttitl.h"
#include "adlib.h"

// ============================================================
//  DOS 원본 PlaySong(Keybd) 동작 재현 — 전체 구조 설명
// ============================================================
//
//  ┌─ Pascal 원본 (MYSTIC.PAS / ADLIB.PAS) ─────────────────────┐
//  │                                                             │
//  │  while TRUE do begin                                        │
//  │    LoadSong(target);                                        │
//  │    PlaySong(Main);   ← Main을 Keybd 콜백으로 전달          │
//  │    PlayOff;                                                 │
//  │  end;                                                       │
//  │                                                             │
//  │  PlaySong 내부 repeat-until 루프:                           │
//  │    1) Sound.com 드라이버 노트 이벤트 큐 선행 채우기        │
//  │       (cnt>200 채운 뒤 PlayState(ON) → 실제 재생 시작)     │
//  │    2) Keybd()  ← 게임 전체 로직(전투/대화/화면 갱신)       │
//  │                                                             │
//  │  Keybd(=Main) 블로킹 시:                                    │
//  │    시퀀서 루프 정지 → 드라이버가 기존 큐를 소비            │
//  │    → 큐 소진까지 수 초간 음악 재생 → 이후 무음             │
//  └─────────────────────────────────────────────────────────────┘
//
//  ┌─ C++ 대응 ─────────────────────────────────────────────────┐
//  │                                                             │
//  │  구성 요소:                                                 │
//  │    Fill 스레드  : OPL 샘플을 PCM 링버퍼에 3초 선행 생성    │
//  │                   ↔ Sound.com 드라이버 노트 이벤트 큐      │
//  │    SDL 콜백     : 링버퍼에서만 읽어 재생                    │
//  │                   ↔ DOS 타이머 ISR + OPL 하드웨어          │
//  │                                                             │
//  │  AudioLock()   → fill 스레드 일시정지                      │
//  │                  SDL 콜백은 계속 링버퍼 소비               │
//  │                  → 3초 후 링버퍼 소진 → 무음               │
//  │                  ↔ Keybd 블로킹 시 드라이버 큐 소진        │
//  │                                                             │
//  │  AudioUnlock() → fill 스레드 재개 → 링버퍼 재충전          │
//  │                  → 음악 재개                                │
//  └─────────────────────────────────────────────────────────────┘

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Into the Mystic",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    InitHan(renderer);

    // ------------------------------------------------------------------
    // [1] 사운드 시스템 초기화
    // ------------------------------------------------------------------
    InitSound();

    AdLibOn = true;
    LoadBank(std::string("assets/") + "MYSTIC.CMD");

    // Init game logic
    InitMystSub();
    Restore_Palette();
    Title_Menu();

    if (mapname.empty()) mapname = "MENACE";
    Set_All();
    Scroll(true);
    Display_Condition();
    UpdateScreen();

    std::string current_bgm = "";
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        // ------------------------------------------------------------------
        // [2] 이벤트 처리
        //
        // AudioLock / AudioUnlock 으로 MainLoopStep 을 감싼다.
        //
        // MainLoopStep 이 짧게 끝나면 → AudioUnlock 즉시 호출 → 음악 계속
        // MainLoopStep 이 dialog 등으로 블로킹되면
        //   → fill 스레드 정지 → SDL 콜백이 링버퍼를 소진
        //   → ~3초(RING_LATENCY_SEC) 후 무음
        //   → AudioUnlock 시 fill 스레드 재개 → 음악 재개
        //
        // 이것이 DOS에서 Keybd(=Main) 블로킹 시 드라이버 노트 큐가
        // 수 초에 걸쳐 소진되며 무음이 되던 동작과 동일하다.
        // ------------------------------------------------------------------
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                AudioLock();        // fill 스레드 일시정지 (링버퍼 선행 생성 중단)
                MainLoopStep(e);    // 게임 로직 — 블로킹 가능
                AudioUnlock();      // fill 스레드 재개
            }
        }

        // ------------------------------------------------------------------
        // [3] 배경음악 재생 제어
        //
        // Pascal 원본 while TRUE 루프와의 대응:
        //   - QuitPlay      : 맵 이동 등 강제 교체 (Pascal의 QuitPlay := true)
        //   - bgmChanged    : 맵 변경으로 대상 BGM 파일이 달라진 경우
        //   - !IsSongPlaying: 곡이 자연 종료됨
        //                     (Pascal의 repeat-until 탈출 후 루프 재진입)
        // ------------------------------------------------------------------
        if (AdLibOn) {
            std::string target_bgm;

            // 전용 BGM이 있는 맵 목록
            bool hasMapBgm =
                (mapname == "Lore"     || mapname == "Ground2"  ||
                 mapname == "OrcTown"  || mapname == "DracTown" ||
                 mapname == "Ground4"  || mapname == "UnderGrd" ||
                 mapname == "TrolTown" || mapname == "Valhalla" ||
                 mapname == "Ground5");

            target_bgm = hasMapBgm ? (mapname + ".BGM") : existfile;
            for (auto& c : target_bgm) c = (char)toupper((unsigned char)c);

            const bool bgmChanged  = (current_bgm != target_bgm);
            const bool songEnded   = !IsSongPlaying() && !current_bgm.empty();

            if (QuitPlay || bgmChanged || songEnded) {
                // 곡 교체는 fill 스레드를 완전히 정지시킨 후 수행한다.
                // AudioLock 상태에서 PlayOff → LoadSong → PlaySong 순서로
                // 실행하면 플레이어 내부 상태 경합(race condition) 없이 안전하다.
                AudioLock();
                PlayOff();
                LoadSong("assets/" + target_bgm);
                AudioUnlock();      // PlaySong 전에 먼저 unlock

                PlaySong();         // 비동기 재생 시작 (fill 스레드 + SDL 콜백 활성화)
                current_bgm = target_bgm;
                QuitPlay = false;
            }
        }

        UpdateScreen();
        SDL_Delay(10);
    }

    // 종료 시 사운드 정리
    if (AdLibOn) {
        PlayOff();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
