#pragma once
/*
 * AdLib.h
 *
 * Public interface for AdLib ROL file playback.
 *
 * ─── DOS 원본 PlaySong(Keybd) 동작 재현 ────────────────────────────
 *
 * 원본 Pascal 구조:
 *   PlaySong 내부에서 매 틱마다 Sound.com 드라이버의 노트 이벤트 큐를
 *   미리 채우고(선행 기록), Keybd() 콜백으로 게임 로직을 실행.
 *   Keybd 블로킹 시 → 시퀀서 정지 → 드라이버가 큐를 소비 → 수 초 후 무음.
 *
 * C++ 대응:
 *   Fill 스레드가 PCM 링버퍼에 RING_LATENCY_SEC 초 분량을 선행 생성.
 *   SDL 콜백은 링버퍼에서만 소비.
 *
 *   AudioLock()   → fill 스레드 일시정지 (링버퍼 선행 생성 중단)
 *                   SDL 콜백은 계속 링버퍼 소비 → 수 초 후 무음
 *   AudioUnlock() → fill 스레드 재개 → 링버퍼 재충전 → 재생 재개
 *
 * 사용 방법:
 *
 *   InitSound();
 *   LoadBank("STANDARD.BNK");
 *   LoadSong("music.rol");
 *   PlaySong();             // 비동기 시작 (fill 스레드 + SDL 콜백)
 *
 *   // 게임 로직 블로킹 구간 (dialog, 키 대기 등)
 *   AudioLock();            // fill 스레드 일시정지 → 수 초 후 무음
 *   MainLoopStep(event);    // 블로킹 가능한 게임 로직
 *   AudioUnlock();          // fill 스레드 재개 → 재생 재개
 *
 *   PlayOff();              // 즉시 정지
 *
 * Required:
 *   Adlib.cpp    - implementation
 *   SoundDrv.h   - OPL2 emulator (OPLChipClass)
 *   SDL2         - audio output
 */

#include <string>

// SDL 오디오 서브시스템 및 내부 플레이어 초기화.
// 다른 함수 호출 전 반드시 한 번 호출해야 한다.
void InitSound();

// 이후 LoadSong에서 사용할 AdLib 악기 뱅크(.BNK) 파일 경로를 설정한다.
void LoadBank(const std::string& BankFile);

// ROL 파일과 설정된 뱅크를 로드하여 재생 준비.
// LoadBank()가 먼저 호출되어 있어야 한다.
void LoadSong(const std::string& FileSpec);

// 비동기 재생 시작 (즉시 반환).
// fill 스레드가 PCM 링버퍼를 선행 생성하고, SDL 콜백이 재생한다.
void PlaySong();

// 재생 즉시 정지, 모든 OPL 보이스 소거.
void PlayOff();

// 곡이 아직 재생 중이면 true.
// 링버퍼까지 완전히 소진된 후 false가 된다.
// (Pascal의 GetState=OFF 판정에 해당)
bool IsSongPlaying();

// DOS 원본의 Keybd 블로킹 동작 재현용.
//
//   AudioLock()   → fill 스레드(PCM 선행 생성) 일시정지.
//                   SDL 콜백은 계속 실행되므로 링버퍼가 RING_LATENCY_SEC 초에
//                   걸쳐 소진된 후 무음이 된다.
//                   (DOS: Keybd 블로킹 → 드라이버 노트 큐 소진 → 수초 후 무음)
//
//   AudioUnlock() → fill 스레드 재개. 링버퍼가 빠르게 재충전되어 음악 재개.
//
// 사용 패턴:
//   AudioLock();
//   블로킹_가능한_게임_로직();
//   AudioUnlock();
void AudioLock();
void AudioUnlock();
