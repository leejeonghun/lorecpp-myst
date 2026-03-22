/*
 * rolplayer.cpp
 *
 * AdLib ROL file player -- implementation.
 *
 * The RolPlayer class is kept entirely private to this translation unit.
 * External callers interact only through the five free functions declared
 * in rolplayer.h:
 *
 *   InitSound()
 *   LoadBank(BankFile)
 *   LoadSong(FileSpec)
 *   PlaySong()
 *   PlayOff()
 *
 * Uses:
 *   OPLChipClass (woodyopl.h / woodyopl.cpp) - OPL2 emulation
 *   SDL2                                      - audio output
 *
 * Build (Windows / MinGW-w64 + SDL2):
 *   g++ -O2 -std=c++14 -o rolplay ^
 *       main.cpp rolplayer.cpp woodyopl.cpp ^
 *       -I. -lSDL2 -lSDL2main
 *
 * Build (Linux / GCC + SDL2):
 *   g++ -O2 -std=c++14 -o rolplay \
 *       main.cpp rolplayer.cpp woodyopl.cpp \
 *       -I. $(sdl2-config --cflags --libs)
 *
 * Format references:
 *   ROL - https://moddingwiki.shikadi.net/wiki/ROL_Format
 *   BNK - https://moddingwiki.shikadi.net/wiki/AdLib_Instrument_Bank_Format
 */

#include "adlib.h"   // only the 5 public function declarations

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cctype>
#include <algorithm>
#include <climits>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "sounddrv.h"    // defines OPLChipClass

// ============================================================
//  Internal RolPlayer class
//  (identical to the original class; private to this .cpp)
// ============================================================

class RolPlayer
{
public:
    // Number of voice groups in a ROL file
    static const int NUM_VOICES     = 11;
    // OPL generation buffer in stereo pairs
    static const int OPL_BUF_PAIRS  = 2048;
    // Number of pitch-bend steps per semitone
    static const int kPitchSteps    = 25;
    // Percussion channel indices
    static const int kBassDrumCh    = 6;
    static const int kSnareCh       = 7;
    static const int kTomCh         = 8;
    static const int kCymbalCh      = 9;
    static const int kHiHatCh       = 10;

    // OPL frequency/octave tables -- public so the file-scope calcBlockFnum()
    // helper can access them.
    static const uint8_t  kNoteOctave[96];              // OPL block per note
    static const uint8_t  kNoteIndex[96];               // semitone index per note
    static const uint16_t kFNumNotes[kPitchSteps][12];  // FNum, 25 pitch rows x 12 semitones

    // BNK operator/instrument data (public so BNK-file helpers can use them)
    struct OplRegs {
        uint8_t ksl, multiple, feedback;
        uint8_t attack, sustain, eg, decay, releaseRate;
        uint8_t totalLevel, am, vib, ksr, con;
    };
    struct Instrument {
        uint8_t percussive, voiceNum;
        OplRegs mod, car;
        uint8_t modWave, carWave;
    };

    // ---------------------------------------------------------------
    // DOS Sound.com 드라이버의 노트 이벤트 큐를 PCM 링버퍼로 재현.
    //
    // DOS 드라이버 분석 결과 (INIT.COM 역어셈블):
    //   - 이벤트 버퍼: 409 노드 (4090 bytes ÷ 10 bytes/node, file:0x070A)
    //   - PutNote 1개 = 노트 1개 = NoteL 틱 분량의 음악
    //   - 버퍼 깊이(시간) = (409/활성보이스) × 평균_노트_틱 / 틱_Hz
    //   - 실측: DOSBox에서 10초 이상 재생 확인
    //
    // ringCapacity_ 는 load() 후 calcRingLatency() 로 곡마다 동적 계산한다.
    // ---------------------------------------------------------------
    static const int kDosEventBufNodes = 409; // INIT.COM 역어셈블 확인값

    // outFreq : SDL output sample rate (44100 typical)
    // bufSize : SDL audio callback buffer size in frames
    // loop    : loop song indefinitely
    explicit RolPlayer(int outFreq = 44100, int bufSize = 1024, bool loop = true);
    ~RolPlayer();

    bool load(const char* rolPath, const char* bnkPath);

    void play();        // blocks until done
    void playAsync();   // non-blocking start
    // seqDone_: 시퀀서(fill 스레드)가 모든 틱 생성 완료
    // songDone_: 링버퍼까지 완전 소진됨 (게임 측이 감지하는 "곡 종료")
    bool isPlaying() const { return !songDone_; }
    void stop();

    // DOS 원본의 Keybd 블로킹 동작 재현용:
    //   audioLock()   → fill 스레드 일시정지 → 링버퍼 서서히 소진 → 무음
    //   audioUnlock() → fill 스레드 재개 → 음악 재생 재개
    void audioLock();
    void audioUnlock();

    float basicTempo() const { return basicTempo_; }
    int   tickBeat()   const { return tickBeat_;   }
    int   totalTicks() const { return totalTicks_;  }
    bool  isMelodic()  const { return isMelodic_;  }

private:
    // -- ROL events --------------------------------------------
    struct TempoEvent  { uint16_t tick; float    mult;     };
    struct NoteEvent   { uint16_t note; uint16_t duration; };
    struct TimbreEvent { uint16_t tick; char     name[9];  };
    struct VolumeEvent { uint16_t tick; float    volume;   };
    struct PitchEvent  { uint16_t tick; float    pitch;    };

    struct VoiceTrack {
        uint16_t                 totalTicks;
        std::vector<NoteEvent>   notes;
        std::vector<TimbreEvent> timbres;
        std::vector<VolumeEvent> volumes;
        std::vector<PitchEvent>  pitches;
    };

    // -- Per-voice runtime state -------------------------------
    struct VoiceState {
        int  noteIdx = 0, timbreIdx = 0, volumeIdx = 0, pitchIdx = 0;
        int  nextNoteTick = 0;
        int  currentNote  = 0;
        float volume = 1.0f;
        float pitch  = 1.0f;
        std::string currentTimbre;
        int      oplBlock = 0;
        uint16_t oplFnum  = 0;
        bool     keyOn    = false;
    };

    // -- Song data ---------------------------------------------
    std::map<std::string, Instrument> bank_;
    float    basicTempo_;
    uint16_t tickBeat_, beatMeasure_;
    bool     isMelodic_;
    int      totalTicks_;
    std::vector<TempoEvent> tempoEvents_;
    VoiceTrack voices_[NUM_VOICES];

    // -- Playback state ----------------------------------------
    std::atomic<bool> seqDone_;   // 시퀀서가 모든 틱 생성 완료 (fill 스레드 측)
    volatile bool     songDone_;  // 링버퍼까지 소진 완료 (SDL 콜백 측에서 셋)
    bool          loop_;
    int           currentTick_;
    float         tempoMult_;
    int           tempoIdx_;
    long          samplesTodo_;
    // Cached value of OPL register 0xBD (AM depth / Vibrato / Rhythm control).
    // Bit 5 = rhythm mode enable; bits 4-0 = per-instrument key-on bits.
    uint8_t       mAMVibRhythm_;

    int16_t oplBuf_[OPL_BUF_PAIRS * 2]; // stereo interleaved (fill 스레드 전용 임시 버퍼)
    int     oplBufReady_;
    int     oplBufRead_;

    // -- PCM 링버퍼 (DOS Sound.com 노트 이벤트 큐 대응) --------
    // fill 스레드가 DOS 드라이버 버퍼 깊이만큼 PCM 샘플을 선행 저장.
    // SDL 콜백은 여기서만 읽고, OPL 칩에 직접 접근하지 않는다.
    // ringCapacity_ 는 곡마다 calcRingLatency() 로 동적 결정된다.
    std::vector<int16_t>  ringBuf_;
    int                   ringCapacity_; // ringBuf_.size() 캐시 (원자적 접근용)
    std::atomic<int>      ringRead_;     // SDL 콜백 스레드가 갱신
    std::atomic<int>      ringWrite_;    // fill 스레드가 갱신

    // -- Fill 스레드 제어 --------------------------------------
    std::thread             fillThread_;
    std::mutex              fillMutex_;
    std::condition_variable fillCv_;
    std::atomic<bool>       fillRunning_;  // false → 스레드 종료 요청
    std::atomic<bool>       fillPaused_;   // true → 생성 일시정지 (Keybd 블로킹 대응)

    // -- Hardware ----------------------------------------------
    OPLChipClass opl_;   // WoodyOPL chip - direct value (no pointer)
    SDL_AudioSpec spec_;

    // -- Per-voice runtime state array -------------------------
    VoiceState vstate_[NUM_VOICES];

    // -- OPL cell offset tables --------------------------------
    static const int     kModCell[9];
    static const int     kCarCell[9];
    // Percussion operator cell offsets (indexed by voice - kSnareCh).
    static const uint8_t kDrumOpCell[4];

    // -- Internals ---------------------------------------------
    bool   loadBnk(const char* path);
    bool   loadRol(const char* path);
    void   initPlayback();
    bool   advanceTick();
    double getRefresh() const;

    void setInstrument(int ch, const std::string& name, float volume);
    void noteOn (int ch, int note, float pitch, float vol, const std::string& timbre);
    void noteOff(int ch);
    void applyVolume(int ch, float volume);
    void applyPitch (int ch, float pitch, int note);
    void writeOpRegs(int cell, const OplRegs& r,
                     uint8_t waveSel, int volOverrideTL = -1);

    void generateOplBlock();
    void fillLoop();          // fill 스레드 본체: OPL 샘플을 링버퍼에 선행 저장
    void fillAudio(int16_t* out, int nShorts);
    int  calcRingCapacity() const;  // DOS 드라이버 버퍼 깊이 → PCM 샘플 수 동적 계산

    static void    sdlCallback(void* ud, Uint8* buf, int len);
    static std::string toLower(const std::string& s);
    static int16_t     clampShort(int v);
};

// ============================================================
// OPL cell-offset tables
// ============================================================

// Modulator cell register offset for OPL2 channels 0-8
const int RolPlayer::kModCell[9] = {  0,  1,  2,  8,  9, 10, 16, 17, 18 };
// Carrier  cell register offset for OPL2 channels 0-8
const int RolPlayer::kCarCell[9] = {  3,  4,  5, 11, 12, 13, 19, 20, 21 };

// Percussion operator cell offsets, indexed by (voice - kSnareCh):
//   voice 7 (snare drum)  -> cell 0x14 = 20  (carrier  of OPL ch 7)
//   voice 8 (tom-tom)     -> cell 0x12 = 18  (modulator of OPL ch 8)
//   voice 9 (cymbal)      -> cell 0x15 = 21  (carrier  of OPL ch 8)
//   voice 10 (hi-hat)     -> cell 0x11 = 17  (modulator of OPL ch 7)
const uint8_t RolPlayer::kDrumOpCell[4] = { 0x14, 0x12, 0x15, 0x11 };

// Octave (OPL block) and semitone index for notes 0..95 (8 octaves x 12 semitones).
const uint8_t RolPlayer::kNoteOctave[96] = {
    0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,
    4,4,4,4,4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7
};
const uint8_t RolPlayer::kNoteIndex[96] = {
    0,1,2,3,4,5,6,7,8,9,10,11,
    0,1,2,3,4,5,6,7,8,9,10,11,
    0,1,2,3,4,5,6,7,8,9,10,11,
    0,1,2,3,4,5,6,7,8,9,10,11,
    0,1,2,3,4,5,6,7,8,9,10,11,
    0,1,2,3,4,5,6,7,8,9,10,11,
    0,1,2,3,4,5,6,7,8,9,10,11,
    0,1,2,3,4,5,6,7,8,9,10,11
};

// FNum table: 25 pitch-bend steps x 12 semitones.
// Row 0 = no pitch bend.  Rows 1-24 = increasing upward bend within one semitone.
const uint16_t RolPlayer::kFNumNotes[RolPlayer::kPitchSteps][12] = {
    { 343, 364, 385, 408, 433, 459, 486, 515, 546, 579, 614, 650 },
    { 344, 365, 387, 410, 434, 460, 488, 517, 548, 581, 615, 652 },
    { 345, 365, 387, 410, 435, 461, 489, 518, 549, 582, 617, 653 },
    { 346, 366, 388, 411, 436, 462, 490, 519, 550, 583, 618, 655 },
    { 346, 367, 389, 412, 437, 463, 491, 520, 551, 584, 619, 657 },
    { 347, 368, 390, 413, 438, 464, 492, 522, 553, 586, 621, 658 },
    { 348, 369, 391, 415, 439, 466, 493, 523, 554, 587, 622, 660 },
    { 349, 370, 392, 415, 440, 467, 495, 524, 556, 589, 624, 661 },
    { 350, 371, 393, 416, 441, 468, 496, 525, 557, 590, 625, 663 },
    { 351, 372, 394, 417, 442, 469, 497, 527, 558, 592, 627, 665 },
    { 351, 372, 395, 418, 443, 470, 498, 528, 559, 593, 628, 666 },
    { 352, 373, 396, 419, 444, 471, 499, 529, 561, 594, 630, 668 },
    { 353, 374, 397, 420, 445, 472, 500, 530, 562, 596, 631, 669 },
    { 354, 375, 398, 421, 447, 473, 502, 532, 564, 597, 633, 671 },
    { 355, 376, 398, 422, 448, 474, 503, 533, 565, 599, 634, 672 },
    { 356, 377, 399, 423, 449, 475, 504, 534, 566, 600, 636, 674 },
    { 356, 378, 400, 424, 450, 477, 505, 535, 567, 601, 637, 675 },
    { 357, 379, 401, 425, 451, 478, 506, 537, 569, 603, 639, 677 },
    { 358, 379, 402, 426, 452, 479, 507, 538, 570, 604, 640, 679 },
    { 359, 380, 403, 427, 453, 480, 509, 539, 571, 606, 642, 680 },
    { 360, 381, 404, 428, 454, 481, 510, 540, 572, 607, 643, 682 },
    { 360, 382, 405, 429, 455, 482, 511, 541, 574, 608, 645, 683 },
    { 361, 383, 406, 430, 456, 483, 512, 543, 575, 610, 646, 685 },
    { 362, 384, 407, 431, 457, 484, 513, 544, 577, 611, 648, 687 },
    { 363, 385, 408, 432, 458, 485, 514, 545, 578, 612, 649, 688 }
};

// ============================================================
// Helpers
// ============================================================

std::string RolPlayer::toLower(const std::string& s)
{
    std::string r = s;
    for (char& c : r) c = (char)std::tolower((unsigned char)c);
    return r;
}

int16_t RolPlayer::clampShort(int v)
{
    if (v >  32767) return  32767;
    if (v < -32768) return -32768;
    return (int16_t)v;
}

/*
 * 0xBD key-on bit for each percussion channel:
 *   ch 6 (Bass Drum) -> bit 4 = 0x10
 *   ch 7 (Snare)     -> bit 3 = 0x08
 *   ch 8 (Tom-tom)   -> bit 2 = 0x04
 *   ch 9 (Cymbal)    -> bit 1 = 0x02
 *   ch 10 (Hi-hat)   -> bit 0 = 0x01
 */
static uint8_t rhythmBit(int ch)
{
    return (uint8_t)(1 << (4 - ch + 6));
}

// ============================================================
// Constructor / Destructor
// ============================================================

RolPlayer::RolPlayer(int outFreq, int bufSize, bool loop)
    : basicTempo_(120.0f), tickBeat_(15), beatMeasure_(4)
    , isMelodic_(true), totalTicks_(0)
    , seqDone_(false), songDone_(true), loop_(loop)
    , currentTick_(0), tempoMult_(1.0f), tempoIdx_(0), samplesTodo_(0)
    , mAMVibRhythm_(0)
    , oplBufReady_(0), oplBufRead_(0)
    , ringCapacity_(0)
    , ringRead_(0), ringWrite_(0)
    , fillRunning_(false), fillPaused_(false)
{
    memset(oplBuf_, 0, sizeof(oplBuf_));
    for (int i = 0; i < NUM_VOICES; i++) vstate_[i] = VoiceState{};

    // -- SDL audio ---------------------------------------------
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "[RolPlayer] SDL_Init error: %s\n", SDL_GetError());
        return;
    }

    memset(&spec_, 0, sizeof(spec_));
    spec_.freq     = outFreq;
    spec_.format   = AUDIO_S16SYS;
    spec_.channels = 2;
    spec_.samples  = (uint16_t)bufSize;
    spec_.callback = &RolPlayer::sdlCallback;
    spec_.userdata = this;

    if (SDL_OpenAudio(&spec_, nullptr) < 0) {
        fprintf(stderr, "[RolPlayer] SDL_OpenAudio error: %s\n", SDL_GetError());
        return;
    }
}

RolPlayer::~RolPlayer()
{
    // fill 스레드를 먼저 종료해야 SDL 오디오 콜백과의 경합을 방지할 수 있다.
    fillRunning_.store(false);
    fillPaused_.store(false);
    fillCv_.notify_all();
    if (fillThread_.joinable()) fillThread_.join();

    SDL_PauseAudio(1);
    SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

// ============================================================
// BNK loading
// ============================================================

#pragma pack(push, 1)
struct BnkFileHeader {
    uint8_t  verMajor, verMinor;
    char     signature[6];   // "ADLIB-"
    uint16_t numUsed;
    uint16_t numInstruments;
    uint32_t offsetName;
    uint32_t offsetData;
    uint8_t  pad[8];
};
struct BnkNameEntry {
    uint16_t index;
    uint8_t  flags;           // 1 = in use
    char     name[9];
};
struct BnkOplRegs {
    uint8_t ksl, multiple, feedback;
    uint8_t attack, sustain, eg, decay, releaseRate;
    uint8_t totalLevel, am, vib, ksr, con;
};
struct BnkInstData {
    uint8_t    percussive, voiceNum;
    BnkOplRegs mod, car;
    uint8_t    modWave, carWave;
};
#pragma pack(pop)

static void copyBnkRegs(RolPlayer::OplRegs& dst, const BnkOplRegs& src)
{
    dst.ksl=src.ksl; dst.multiple=src.multiple; dst.feedback=src.feedback;
    dst.attack=src.attack; dst.sustain=src.sustain; dst.eg=src.eg;
    dst.decay=src.decay; dst.releaseRate=src.releaseRate;
    dst.totalLevel=src.totalLevel; dst.am=src.am;
    dst.vib=src.vib; dst.ksr=src.ksr; dst.con=src.con;
}

bool RolPlayer::loadBnk(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "[RolPlayer] Cannot open BNK: %s\n", path); return false; }

    BnkFileHeader hdr;
    if (fread(&hdr, sizeof(hdr), 1, f) != 1) { fclose(f); return false; }
    if (strncmp(hdr.signature, "ADLIB-", 6) != 0) {
        fprintf(stderr, "[RolPlayer] Not a valid BNK file: %s\n", path);
        fclose(f); return false;
    }

    int n = hdr.numInstruments;
    fseek(f, (long)hdr.offsetName, SEEK_SET);
    std::vector<BnkNameEntry> names(n);
    for (int i = 0; i < n; i++) fread(&names[i], sizeof(BnkNameEntry), 1, f);

    fseek(f, (long)hdr.offsetData, SEEK_SET);
    std::vector<BnkInstData>  data(n);
    for (int i = 0; i < n; i++) fread(&data[i],  sizeof(BnkInstData),  1, f);
    fclose(f);

    bank_.clear();
    for (int i = 0; i < n; i++) {
        if (!names[i].flags) continue;
        int idx = names[i].index;
        if (idx < 0 || idx >= n) continue;
        Instrument inst;
        inst.percussive = data[idx].percussive;
        inst.voiceNum   = data[idx].voiceNum;
        inst.modWave    = data[idx].modWave;
        inst.carWave    = data[idx].carWave;
        copyBnkRegs(inst.mod, data[idx].mod);
        copyBnkRegs(inst.car, data[idx].car);
        char name[9] = {};
        strncpy(name, names[i].name, 8);
        bank_[toLower(std::string(name))] = inst;
    }
    fprintf(stderr, "[RolPlayer] BNK: %zu instruments from '%s'\n", bank_.size(), path);
    return true;
}

// ============================================================
// ROL loading
// ============================================================

static uint16_t readU16(FILE* f)
{
    uint8_t b[2] = {};
    fread(b, 1, 2, f);
    return (uint16_t)(b[0] | (b[1] << 8));
}

static float readF32(FILE* f)
{
    uint8_t b[4] = {};
    fread(b, 1, 4, f);
    uint32_t bits = (uint32_t)(b[0] | (b[1]<<8) | (b[2]<<16) | (b[3]<<24));
    float v; memcpy(&v, &bits, 4);
    return v;
}

bool RolPlayer::loadRol(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "[RolPlayer] Cannot open ROL: %s\n", path); return false; }

    // -- Header (182 bytes) ------------------------------------
    uint16_t majorVer = readU16(f);
    uint16_t minorVer = readU16(f);
    char sig[40]; fread(sig, 1, 40, f); sig[39] = '\0';
    tickBeat_    = readU16(f);
    beatMeasure_ = readU16(f);
    readU16(f);  // scaleY
    readU16(f);  // scaleX
    uint8_t reserved = 0, isMel = 0;
    fread(&reserved, 1, 1, f);
    fread(&isMel,    1, 1, f);
    isMelodic_ = (isMel != 0);

    // counters[45] + filler[38] = 128 bytes skipped
    uint8_t skip[128]; fread(skip, 1, sizeof(skip), f);

    // -- Tempo track -------------------------------------------
    char trackName[15];
    fread(trackName, 1, 15, f);
    basicTempo_ = readF32(f);
    uint16_t nEv = readU16(f);
    tempoEvents_.clear();
    for (int i = 0; i < nEv; i++) {
        TempoEvent te;
        te.tick = readU16(f);
        te.mult = readF32(f);
        tempoEvents_.push_back(te);
    }

    // -- Voice groups ------------------------------------------
    // Melodic ROL files contain only 9 voice tracks; reading 11 would misalign the file offset.
    int numVoices = isMelodic_ ? 9 : NUM_VOICES;  // 9 melodic, 11 percussive

    // Clear all 11 runtime tracks first; unused slots (9-10 in melodic mode)
    // remain empty and produce silence during playback.
    totalTicks_ = 0;
    for (int v = 0; v < NUM_VOICES; v++) {
        voices_[v].notes.clear();   voices_[v].timbres.clear();
        voices_[v].volumes.clear(); voices_[v].pitches.clear();
        voices_[v].totalTicks = 0;
    }

    for (int v = 0; v < numVoices; v++) {
        VoiceTrack& vt = voices_[v];

        // Voice (note) track
        fread(trackName, 1, 15, f);
        uint16_t nTicks = readU16(f);
        vt.totalTicks = nTicks;
        uint32_t acc = 0;
        while (acc < (uint32_t)nTicks) {
            NoteEvent ne;
            ne.note     = readU16(f);
            ne.duration = readU16(f);
            vt.notes.push_back(ne);
            acc += ne.duration;
            if (ne.duration == 0) break;
        }
        if ((int)nTicks > totalTicks_) totalTicks_ = nTicks;

        // Timbre track
        fread(trackName, 1, 15, f);
        nEv = readU16(f);
        for (int i = 0; i < nEv; i++) {
            TimbreEvent te;
            te.tick = readU16(f);
            fread(te.name, 1, 9, f); te.name[8] = '\0';
            uint8_t  fill = 0; fread(&fill, 1, 1, f);
            uint16_t unk  = 0; (void)unk; unk = readU16(f);
            vt.timbres.push_back(te);
        }

        // Volume track
        fread(trackName, 1, 15, f);
        nEv = readU16(f);
        for (int i = 0; i < nEv; i++) {
            VolumeEvent ve;
            ve.tick   = readU16(f);
            ve.volume = readF32(f);
            vt.volumes.push_back(ve);
        }

        // Pitch track
        fread(trackName, 1, 15, f);
        nEv = readU16(f);
        for (int i = 0; i < nEv; i++) {
            PitchEvent pe;
            pe.tick  = readU16(f);
            pe.pitch = readF32(f);
            vt.pitches.push_back(pe);
        }
    }

    fclose(f);
    fprintf(stderr,
        "[RolPlayer] ROL v%d.%d  sig='%s'\n"
        "            tickBeat=%u  tempo=%.1f BPM  melodic=%s  voices=%d  ticks=%d\n",
        majorVer, minorVer, sig, tickBeat_, basicTempo_,
        isMelodic_ ? "yes" : "no", numVoices, totalTicks_);
    return true;
}

// ============================================================
// Public: load
// ============================================================

bool RolPlayer::load(const char* rolPath, const char* bnkPath)
{
    return loadBnk(bnkPath) && loadRol(rolPath);
}

// ============================================================
// OPL register helpers
// ============================================================

/*
 * Write one operator's registers into the OPL via adlib_write().
 *
 *  cell          : operator cell offset (0-21)
 *  r             : BNK operator data
 *  waveSel       : wave-select (0-3)
 *  volOverrideTL : if >= 0, used instead of r.totalLevel (volume scaling)
 */
void RolPlayer::writeOpRegs(int cell, const OplRegs& r,
                              uint8_t waveSel, int volOverrideTL)
{
    // 0x20: AM | VIB | EG | KSR | MULT
    opl_.adlib_write(0x20 + cell,
        (uint8_t)(((r.am & 1) << 7) | ((r.vib & 1) << 6) |
                  ((r.eg & 1) << 5) | ((r.ksr & 1) << 4) |
                  (r.multiple & 0x0F)));

    // 0x40: KSL | TL (total level; higher = quieter)
    int tl = (volOverrideTL >= 0) ? volOverrideTL : (int)(r.totalLevel & 0x3F);
    opl_.adlib_write(0x40 + cell,
        (uint8_t)(((r.ksl & 3) << 6) | (tl & 0x3F)));

    // 0x60: AR | DR
    opl_.adlib_write(0x60 + cell,
        (uint8_t)(((r.attack & 0xF) << 4) | (r.decay & 0xF)));

    // 0x80: SL | RR
    opl_.adlib_write(0x80 + cell,
        (uint8_t)(((r.sustain & 0xF) << 4) | (r.releaseRate & 0xF)));

    // 0xE0: wave select (only effective after reg 0x01 bit 5 is set)
    opl_.adlib_write(0xE0 + cell, waveSel & 0x03);
}

/*
 * Compute volume-adjusted TL value.
 *
 *   1. ROL volume float multiplier -> uint8: vol_int = (uint8_t)(127 * mult)  [truncate]
 *   2. KSL/TL formula:
 *        amplitude = 63 - (baseTL & 0x3F)
 *        kslTL     = vol_int * amplitude
 *        kslTL     = kslTL + kslTL + 127    // 2*vol*ampl + 127
 *        kslTL     = 63 - kslTL / 254        // integer division = round-down
 */
static int calcVolTL(int baseTL, float volumeFloat)
{
    // Step 1: convert float [0,1] to uint8 [0,127]; the cast truncates (does not round).
    int vol = (int)(127.0f * volumeFloat);
    if (vol < 0)   vol = 0;
    if (vol > 127) vol = 127;

    // Step 2: KSL/TL integer arithmetic
    uint16_t amplitude = (uint16_t)(63 - (baseTL & 0x3F));
    uint16_t kslTL     = (uint16_t)(vol * amplitude);    // vol_int * amplitude
    kslTL = kslTL + kslTL + 127;                         // 2*vol*ampl + 127
    kslTL = 63 - kslTL / 254;                            // 63 - (2*v*a+127)/254

    return (int)kslTL;
}

/*
 * Load a BNK instrument into an OPL channel's registers.
 * Volume is applied by scaling TL field(s).
 *
 * BNK 'con' field -> OPL 0xC0 bit0 (Synthesis Type):
 *   BNK con=0 -> OPL additive (bit0=1)
 *   BNK con=1 -> OPL FM       (bit0=0)
 */
void RolPlayer::setInstrument(int ch, const std::string& name, float volume)
{
    if (ch < 0 || ch >= NUM_VOICES) return;

    // Zero-initialized fallback: AR=0 -> envelope never attacks -> silent.
    static const Instrument kZeroInst = {};

    auto it = bank_.find(toLower(name));
    const Instrument& inst = (it != bank_.end()) ? it->second : kZeroInst;

    // ---- Percussion path (voices 7-10 in rhythm mode) ------------------
    // Only ONE operator per drum instrument.  The MODULATOR's BNK data is
    // written to the drum operator cell (not the standard mod/car cells).
    // No 0xC0 feedback/connection register is written.
    // Volume scaling uses the modulator's totalLevel.
    if (!isMelodic_ && ch >= kSnareCh && ch <= kHiHatCh) {
        int drumCell = kDrumOpCell[ch - kSnareCh];
        int volTL    = calcVolTL(inst.mod.totalLevel & 0x3F, volume);
        opl_.adlib_write(0x20 + drumCell,
            (uint8_t)(((inst.mod.am  & 1) << 7) | ((inst.mod.vib & 1) << 6) |
                      ((inst.mod.eg  & 1) << 5) | ((inst.mod.ksr & 1) << 4) |
                       (inst.mod.multiple & 0x0F)));
        opl_.adlib_write(0x40 + drumCell,
            (uint8_t)(((inst.mod.ksl & 3) << 6) | (volTL & 0x3F)));
        opl_.adlib_write(0x60 + drumCell,
            (uint8_t)(((inst.mod.attack  & 0xF) << 4) | (inst.mod.decay       & 0xF)));
        opl_.adlib_write(0x80 + drumCell,
            (uint8_t)(((inst.mod.sustain & 0xF) << 4) | (inst.mod.releaseRate & 0xF)));
        opl_.adlib_write(0xE0 + drumCell, inst.modWave & 0x03);
        return;
    }

    // ---- Melodic path (voices 0-8, and bass drum ch 6 in rhythm mode) --
    // Bass drum (ch 6) follows this path; key-on is controlled via 0xBD.

    // Modulator: written without volume scaling
    writeOpRegs(kModCell[ch], inst.mod, inst.modWave);

    // Carrier: volume-scaled
    writeOpRegs(kCarCell[ch], inst.car, inst.carWave,
                calcVolTL(inst.car.totalLevel & 0x3F, volume));

    // 0xC0: [bits 3:1] = feedback factor, [bit 0] = synthesis type (BNK con=0 -> additive=1)
    uint8_t synthType = (inst.mod.con == 0) ? 1 : 0;
    opl_.adlib_write(0xC0 + ch,
        (uint8_t)(((inst.mod.feedback & 0x7) << 1) | synthType));
}

// ============================================================
// Note on / off / volume / pitch
// ============================================================

void RolPlayer::noteOff(int ch)
{
    if (ch < 0 || ch >= NUM_VOICES) return;
    VoiceState& vs = vstate_[ch];
    vs.keyOn = false;

    if (!isMelodic_ && ch >= kBassDrumCh) {
        // Percussion key-off: clear the channel bit in 0xBD.
        mAMVibRhythm_ &= ~rhythmBit(ch);
        opl_.adlib_write(0xBD, mAMVibRhythm_);
    } else if (ch < 9) {
        // Melodic key-off: clear bit5 (keyOn) in 0xBx, keep block+fnum.
        opl_.adlib_write(0xB0 + ch,
            (uint8_t)(((vs.oplBlock & 7) << 2) | ((vs.oplFnum >> 8) & 0x03)));
    }
}

/*
 * Convert a ROL pitch variation (1.0 = no bend) to an OPL (block, fnum) pair.
 *
 *   pitchBend       = (variation == 1.0) ? kMidPitch : (kMidPitch - 1) * variation
 *   pitchBendLength = (pitchBend - kMidPitch) * kPitchSteps
 *   halfToneOffset  = pitchBendLength / kMidPitch     (signed int division)
 *   delta           = pitchBendLength % kPitchSteps   (FNum row index)
 *
 * FNum is looked up in kFNumNotes[delta][biased_semi] where
 *   biased_note = clamp(note + halfToneOffset, 0, 95)
 *   block       = kNoteOctave[biased_note]
 *   semi        = kNoteIndex[biased_note]
 */
static void calcBlockFnum(int note, float variation,
                           int& block, uint16_t& fnum)
{
    static const int kMidPitch   = 0x2000;
    static const int kPitchSteps = 25;

    uint16_t pitchBend;
    if (variation == 1.0f) {
        pitchBend = (uint16_t)kMidPitch;
    } else {
        int pv = (int)((kMidPitch - 1) * variation);
        pitchBend = (uint16_t)(pv < 0 ? 0 : pv > 0x3FFF ? 0x3FFF : pv);
    }

    int32_t pitchBendLength = (int32_t)(pitchBend - kMidPitch) * kPitchSteps;

    int halfToneOffset, delta;
    if (pitchBendLength < 0) {
        int stepDown = kPitchSteps - 1 - pitchBendLength / kMidPitch;
        halfToneOffset = -(stepDown / kPitchSteps);
        delta = (stepDown - kPitchSteps + 1) % kPitchSteps;
        if (delta) delta = kPitchSteps - delta;
    } else {
        halfToneOffset = pitchBendLength / kMidPitch / kPitchSteps;
        delta = (pitchBendLength / kMidPitch) % kPitchSteps;
    }

    int biased = note + halfToneOffset;
    if (biased < 0)  biased = 0;
    if (biased > 95) biased = 95;

    block = RolPlayer::kNoteOctave[biased];
    fnum  = RolPlayer::kFNumNotes[delta][RolPlayer::kNoteIndex[biased]];
}

/*
 * Write OPL frequency registers for a channel without setting the keyOn bit.
 * Used for bass drum and tom-tom in rhythm mode: 0xBx bit5 is always 0 --
 * key-on is controlled exclusively through 0xBD.
 */
static void writeFreqRegs(OPLChipClass& opl, int ch, int block, uint16_t fnum)
{
    opl.adlib_write(0xA0 + ch,
        (uint8_t)(fnum & 0xFF));
    opl.adlib_write(0xB0 + ch,
        (uint8_t)(((block & 7) << 2) | ((fnum >> 8) & 0x03)));
}

void RolPlayer::noteOn(int ch, int note, float pitch,
                        float volume, const std::string& timbre)
{
    if (ch < 0 || ch >= NUM_VOICES) return;

    // ROL note 0 = silence. ROL notes 1-11 yield internal notes -11..-1;
    // clamp negatives to 0 to avoid out-of-range table access.
    int internalNote = note - 12;
    if (internalNote < 0) internalNote = 0;

    // Key-off first to re-trigger envelope
    noteOff(ch);

    // Load instrument registers
    if (!timbre.empty())
        setInstrument(ch, timbre, volume);

    if (!isMelodic_ && ch >= kBassDrumCh) {
        // ---- Percussion path ----------------------------------------
        // Update frequency only for BD and TT, then set the 0xBD key-on bit.
        // Pitch bend is ignored for all percussion voices.

        if (ch == kBassDrumCh) {
            // Bass drum uses OPL ch 6 frequency registers (0xA6/0xB6).
            // keyOn bit in 0xB6 is NOT set -- controlled via 0xBD.
            int block; uint16_t fnum;
            calcBlockFnum(internalNote, 1.0f, block, fnum);
            vstate_[ch].oplBlock = block;
            vstate_[ch].oplFnum  = fnum;
            writeFreqRegs(opl_, kBassDrumCh, block, fnum);

        } else if (ch == kTomCh) {
            // Tom-tom: update ch8 frequency AND ch7 (snare) frequency
            // (snare pitch tracks tom + 7 semitones).
            int block; uint16_t fnum;
            calcBlockFnum(internalNote, 1.0f, block, fnum);
            writeFreqRegs(opl_, kTomCh, block, fnum);
            int block2; uint16_t fnum2;
            calcBlockFnum(std::min(internalNote + 7, 95), 1.0f, block2, fnum2);
            writeFreqRegs(opl_, kSnareCh, block2, fnum2);
        }
        // Snare (ch7), Cymbal (ch9), Hi-hat (ch10): only 0xBD key-on bit is toggled.

        vstate_[ch].keyOn = true;
        mAMVibRhythm_ |= rhythmBit(ch);
        opl_.adlib_write(0xBD, mAMVibRhythm_);

    } else if (ch < 9) {
        // ---- Melodic path -------------------------------------------
        int      block;
        uint16_t fnum;
        calcBlockFnum(internalNote, pitch, block, fnum);

        VoiceState& vs = vstate_[ch];
        vs.oplBlock = block;
        vs.oplFnum  = fnum;
        vs.keyOn    = true;

        opl_.adlib_write(0xA0 + ch, (uint8_t)(fnum & 0xFF));
        opl_.adlib_write(0xB0 + ch,
            (uint8_t)(0x20 | ((block & 7) << 2) | ((fnum >> 8) & 0x03)));
    }
}

void RolPlayer::applyVolume(int ch, float volume)
{
    if (ch < 0 || ch >= NUM_VOICES) return;
    auto it = bank_.find(toLower(vstate_[ch].currentTimbre));
    if (it == bank_.end()) return;
    const Instrument& inst = it->second;

    if (!isMelodic_ && ch >= kSnareCh && ch <= kHiHatCh) {
        // Percussion voices 7-10: update the drum operator cell's TL
        // using the modulator's totalLevel for volume scaling.
        int drumCell = kDrumOpCell[ch - kSnareCh];
        int volTL    = calcVolTL(inst.mod.totalLevel & 0x3F, volume);
        opl_.adlib_write(0x40 + drumCell,
            (uint8_t)(((inst.mod.ksl & 3) << 6) | (volTL & 0x3F)));
        return;
    }

    if (ch >= 9) return;  // ch 9-10 in melodic mode: no OPL channel

    // Melodic (and bass drum ch 6 in rhythm mode): scale the carrier TL.
    int volTL = calcVolTL(inst.car.totalLevel & 0x3F, volume);
    opl_.adlib_write(0x40 + kCarCell[ch],
        (uint8_t)(((inst.car.ksl & 3) << 6) | (volTL & 0x3F)));
}

void RolPlayer::applyPitch(int ch, float pitch, int note)
{
    // Pitch bend is ignored for percussion channels in rhythm mode.
    if (!isMelodic_ && ch >= kBassDrumCh) return;

    if (ch < 0 || ch >= 9 || note <= 0) return;
    VoiceState& vs = vstate_[ch];

    // ROL note n -> internal note (n - 12); clamp negatives to 0.
    int internalNote = note - 12;
    if (internalNote < 0) internalNote = 0;

    int      block;
    uint16_t fnum;
    calcBlockFnum(internalNote, pitch, block, fnum);

    vs.oplBlock = block;
    vs.oplFnum  = fnum;
    // Write 0xA0/0xB0 regardless of keyOn state; keyOn bit reflects actual key state.
    opl_.adlib_write(0xA0 + ch, (uint8_t)(fnum & 0xFF));
    opl_.adlib_write(0xB0 + ch,
        (uint8_t)((vs.keyOn ? 0x20 : 0) | ((block & 7) << 2) | ((fnum >> 8) & 0x03)));
}

// ============================================================
// Playback initialisation
// ============================================================

void RolPlayer::initPlayback()
{
    opl_.adlib_init(spec_.freq, 2, 2);

    // Enable OPL2 waveform-select feature (register 0x01, bit 5)
    opl_.adlib_write(0x01, 0x20);

    currentTick_ = 0;
    tempoMult_   = 1.0f;
    tempoIdx_    = 0;
    samplesTodo_ = 0;
    songDone_    = false;
    oplBufReady_ = 0;
    oplBufRead_  = 0;

    // -- Rhythm mode setup ----------------------------------------
    mAMVibRhythm_ = 0;
    if (!isMelodic_) {
        mAMVibRhythm_ = 0x20;  // bit 5 = rhythm mode enable
        opl_.adlib_write(0xBD, mAMVibRhythm_);

        // Pre-set default frequencies for tom-tom (ch8) and snare (ch7).
        int block; uint16_t fnum;
        calcBlockFnum(24, 1.0f, block, fnum);
        writeFreqRegs(opl_, kTomCh,   block, fnum);
        calcBlockFnum(31, 1.0f, block, fnum);
        writeFreqRegs(opl_, kSnareCh, block, fnum);
    }

    for (int v = 0; v < NUM_VOICES; v++) {
        vstate_[v] = VoiceState{};  // reset to default-initialized values
    }
}

// ============================================================
// Tick processing
// ============================================================

double RolPlayer::getRefresh() const
{
    // Cap ticks_per_beat at 60 to prevent excessive tempo on high-tickBeat files.
    double tickBeat = (tickBeat_ < 60) ? (double)tickBeat_ : 60.0;
    double tps = (basicTempo_ / 60.0) * tickBeat * tempoMult_;
    return tps > 0.0 ? tps : 1.0;
}

bool RolPlayer::advanceTick()
{
    // Process tick totalTicks_, then stop when we exceed it.
    if (currentTick_ > totalTicks_) {
        for (int v = 0; v < NUM_VOICES; v++) noteOff(v);
        return false;
    }

    // Tempo events
    while (tempoIdx_ < (int)tempoEvents_.size() &&
           tempoEvents_[tempoIdx_].tick <= (uint16_t)currentTick_)
    {
        tempoMult_ = tempoEvents_[tempoIdx_++].mult;
    }

    // Per-voice events
    for (int v = 0; v < NUM_VOICES; v++) {
        VoiceTrack& vt = voices_[v];
        VoiceState& vs = vstate_[v];

        // Timbre (instrument) events
        while (vs.timbreIdx < (int)vt.timbres.size() &&
               vt.timbres[vs.timbreIdx].tick <= (uint16_t)currentTick_)
        {
            vs.currentTimbre = toLower(std::string(vt.timbres[vs.timbreIdx].name));
            setInstrument(v, vs.currentTimbre, vs.volume);
            vs.timbreIdx++;
        }

        // Volume events
        while (vs.volumeIdx < (int)vt.volumes.size() &&
               vt.volumes[vs.volumeIdx].tick <= (uint16_t)currentTick_)
        {
            vs.volume = vt.volumes[vs.volumeIdx].volume;
            applyVolume(v, vs.volume);
            vs.volumeIdx++;
        }

        // Pitch events: apply unconditionally when a note has been loaded.
        while (vs.pitchIdx < (int)vt.pitches.size() &&
               vt.pitches[vs.pitchIdx].tick <= (uint16_t)currentTick_)
        {
            vs.pitch = vt.pitches[vs.pitchIdx].pitch;
            if (vs.currentNote > 0)
                applyPitch(v, vs.pitch, vs.currentNote);
            vs.pitchIdx++;
        }

        // Note events (streaming: each note has a duration in ticks)
        while (vs.noteIdx < (int)vt.notes.size() &&
               vs.nextNoteTick <= currentTick_)
        {
            NoteEvent& ne = vt.notes[vs.noteIdx++];
            if (ne.note == 0) {
                noteOff(v);
                vs.currentNote = 0;
                vs.keyOn = false;
            } else {
                vs.currentNote = ne.note;
                noteOn(v, ne.note, vs.pitch, vs.volume, vs.currentTimbre);
            }
            vs.nextNoteTick += ne.duration;
            if (ne.duration > 0) break;
        }
    }

    currentTick_++;
    return true;
}

// ============================================================
// OPL 오디오 블록 생성 (fill 스레드 전용)
// ============================================================
/*
 * OPL_BUF_PAIRS 스테레오 프레임 분량의 PCM 샘플을 oplBuf_에 생성한다.
 * 이 함수는 fill 스레드에서만 호출되며 OPL 칩 상태에 독점 접근한다.
 * SDL 오디오 콜백은 oplBuf_에 접근하지 않는다.
 */
void RolPlayer::generateOplBlock()
{
    int   remaining = OPL_BUF_PAIRS;
    int16_t* ptr    = oplBuf_;

    while (remaining > 0) {
        if (samplesTodo_ <= 0) {
            if (!advanceTick()) {
                // 시퀀서 완료 - 나머지를 무음으로 채움
                memset(ptr, 0, remaining * 4);
                remaining = 0;
                // seqDone_ 셋: 시퀀서가 모든 틱을 생성 완료.
                // 실제 "곡 종료"(songDone_)는 SDL 콜백이 링버퍼까지
                // 완전히 소진한 후에 별도로 셋된다.
                seqDone_.store(true, std::memory_order_release);
                break;
            }
            samplesTodo_ = (long)((double)spec_.freq / getRefresh());
            if (samplesTodo_ < 1) samplesTodo_ = 1;
        }

        long toFill = std::min((long)remaining, samplesTodo_);
        opl_.adlib_getsample(ptr, (int)toFill);

        ptr          += toFill * 2;   // stereo: 2 shorts per pair
        samplesTodo_ -= toFill;
        remaining    -= (int)toFill;
    }

    oplBufReady_ = OPL_BUF_PAIRS;
    oplBufRead_  = 0;
}

// ============================================================
// DOS 드라이버 버퍼 깊이 → PCM 링버퍼 샘플 수 동적 계산
// ============================================================
/*
 * INIT.COM 역어셈블로 확인된 드라이버 구조:
 *   - 이벤트 노드 풀: 409개 (4090 bytes ÷ 10 bytes/node, 0x070A)
 *   - PutNote 이벤트 1개 = NoteL 틱 분량의 음악
 *
 * 버퍼 커버 시간:
 *   (409 / 활성_보이스수) × 평균_노트_지속_틱 / (BPM/60 × TPB)
 *
 * 범위 클램프:
 *   최소 8초  - 매우 빠른 곡 / 짧은 노트 보호
 *   최대 90초 - 매우 느린 곡 / 긴 노트 보호 (메모리 ~16MB 이내)
 */
int RolPlayer::calcRingCapacity() const
{
    static const double kMinSec = 8.0;
    static const double kMaxSec = 90.0;

    // 활성 보이스 수 및 노트 이벤트 통계
    int    activeVoices    = 0;
    int    totalNoteEvents = 0;
    double totalNoteTicks  = 0.0;

    for (int v = 0; v < NUM_VOICES; v++) {
        bool hasNote = false;
        for (const auto& ne : voices_[v].notes) {
            if (ne.note != 0) {           // rest(note=0) 제외
                hasNote = true;
                totalNoteEvents++;
                totalNoteTicks += ne.duration;
            }
        }
        if (hasNote) activeVoices++;
    }

    if (activeVoices == 0 || totalNoteEvents == 0) {
        return (int)(kMinSec * spec_.freq * 2) + OPL_BUF_PAIRS * 4;
    }

    double avgNoteTicks = totalNoteTicks / totalNoteEvents;
    double tickHz       = (basicTempo_ / 60.0) * (double)tickBeat_;
    if (tickHz <= 0.0) tickHz = 20.0;

    // DOS 드라이버 409 노드가 커버하는 실제 시간
    double latencySec = ((double)kDosEventBufNodes / activeVoices)
                        * avgNoteTicks / tickHz;

    latencySec = std::max(kMinSec, std::min(latencySec, kMaxSec));

    // stereo int16_t 샘플 수 + OPL 블록 1개 여유
    int cap = (int)(latencySec * spec_.freq * 2) + OPL_BUF_PAIRS * 4;

    fprintf(stderr,
        "[RolPlayer] ring latency: %.1fs  "
        "(voices=%d avgNoteTicks=%.1f tickHz=%.1f)\n",
        latencySec, activeVoices, avgNoteTicks, tickHz);

    return cap;
}

// ============================================================
// Fill 스레드 본체
// ============================================================
/*
 * DOS Sound.com 드라이버의 노트 이벤트 큐 선행 채우기를 재현.
 *
 * 동작:
 *   - fillPaused_=false인 동안 OPL 샘플 블록을 생성하여 링버퍼에 기록.
 *   - fillPaused_=true(audioLock)이면 condvar에서 대기.
 *     → SDL 콜백이 링버퍼를 RING_LATENCY_SEC 초에 걸쳐 소진 → 무음.
 *   - seqDone_이 true(시퀀서 완료)이고 loop_=true이면 재시작.
 *   - fillRunning_=false이면 종료.
 */
void RolPlayer::fillLoop()
{
    while (fillRunning_.load(std::memory_order_relaxed)) {

        // ── 일시정지 대기 ───────────────────────────────────────
        if (fillPaused_.load(std::memory_order_acquire)) {
            std::unique_lock<std::mutex> lk(fillMutex_);
            fillCv_.wait(lk, [this] {
                return !fillPaused_.load(std::memory_order_relaxed)
                    || !fillRunning_.load(std::memory_order_relaxed);
            });
            if (!fillRunning_.load(std::memory_order_relaxed)) break;
        }

        // ── 시퀀서 완료 처리 ──────────────────────────────────
        if (seqDone_.load(std::memory_order_relaxed)) {
            if (loop_) {
                // loop_=true: 링버퍼가 소진되기 전에 시퀀서를 재시작하여
                // 끊김 없이 루프 재생한다.
                initPlayback();
                seqDone_.store(false, std::memory_order_release);
            } else {
                SDL_Delay(5);
                continue;
            }
        }

        // ── 링버퍼 여유 공간 확인 ─────────────────────────────
        if (ringCapacity_ <= 0) { SDL_Delay(5); continue; }  // 미초기화 방어

        int rd    = ringRead_.load(std::memory_order_acquire);
        int wr    = ringWrite_.load(std::memory_order_relaxed);
        int used  = (wr - rd + ringCapacity_) % ringCapacity_;
        int space = ringCapacity_ - used - 1;  // 1 슬롯 gap: full/empty 구분

        if (space < OPL_BUF_PAIRS * 2) {
            // 링버퍼가 가득 참 → 잠시 대기 후 재시도
            SDL_Delay(1);
            continue;
        }

        // ── OPL 블록 생성 → 링버퍼 기록 ──────────────────────
        // generateOplBlock()은 OPL 칩과 틱 시퀀서에만 접근하고
        // ringBuf_에는 접근하지 않으므로 fill 스레드 단독 실행 안전.
        generateOplBlock();  // 결과를 oplBuf_[OPL_BUF_PAIRS*2]에 저장

        int newWr = wr;
        const int blockSamples = OPL_BUF_PAIRS * 2;  // stereo int16_t 개수
        for (int i = 0; i < blockSamples; i++) {
            ringBuf_[newWr] = oplBuf_[i];
            if (++newWr >= ringCapacity_) newWr = 0;
        }
        ringWrite_.store(newWr, std::memory_order_release);
    }
}

// ============================================================
// audioLock / audioUnlock  (DOS Keybd 블로킹 동작 재현)
// ============================================================
/*
 * audioLock():
 *   fill 스레드의 PCM 선행 생성을 일시 정지한다.
 *   SDL 오디오 콜백은 계속 실행되며 링버퍼를 소진한다.
 *   RING_LATENCY_SEC 초 후 링버퍼가 비면 무음이 된다.
 *   → DOS에서 Keybd가 블로킹되어 드라이버 큐가 소진되는 동작과 동일.
 *
 * audioUnlock():
 *   fill 스레드를 재개한다. 링버퍼에 즉시 선행 생성이 재시작되므로
 *   다음 RING_LATENCY_SEC 초 분량이 빠르게 채워지고 재생이 재개된다.
 */
void RolPlayer::audioLock()
{
    fillPaused_.store(true, std::memory_order_release);
    // fill 스레드가 현재 블록 생성 중이면 완료 후 대기에 들어간다(최대 ~23ms).
    // condvar notify는 불필요하나 안전을 위해 호출.
    fillCv_.notify_one();
}

void RolPlayer::audioUnlock()
{
    fillPaused_.store(false, std::memory_order_release);
    fillCv_.notify_one();
}

// ============================================================
// SDL callback → 링버퍼에서 읽기
// ============================================================
/*
 * SDL 오디오 콜백은 링버퍼에서만 읽는다. OPL 칩에는 절대 접근하지 않는다.
 * 링버퍼가 비었을 때 seqDone_이 true이면 songDone_을 셋한다.
 */
void RolPlayer::fillAudio(int16_t* out, int nShorts)
{
    int rd    = ringRead_.load(std::memory_order_relaxed);
    int wr    = ringWrite_.load(std::memory_order_acquire);
    int avail = (wr - rd + ringCapacity_) % ringCapacity_;

    int toRead = std::min(avail, nShorts);
    for (int i = 0; i < toRead; i++) {
        out[i] = ringBuf_[rd];
        if (++rd >= ringCapacity_) rd = 0;
    }
    ringRead_.store(rd, std::memory_order_release);

    // 링버퍼가 부족한 경우 나머지를 무음으로 채움
    if (toRead < nShorts) {
        memset(out + toRead, 0, (nShorts - toRead) * sizeof(int16_t));
        // 링버퍼도 소진되고 시퀀서도 완료됐으면 → 진짜 곡 종료
        if (seqDone_.load(std::memory_order_acquire)) {
            songDone_ = true;
        }
    }
}

void RolPlayer::sdlCallback(void* ud, Uint8* buf, int len)
{
    RolPlayer* self = static_cast<RolPlayer*>(ud);
    memset(buf, 0, len);
    if (!self->songDone_)
        self->fillAudio(reinterpret_cast<int16_t*>(buf), len / 2);
}

// ============================================================
// Public play interface
// ============================================================

void RolPlayer::playAsync()
{
    // 기존 fill 스레드 종료 대기
    if (fillThread_.joinable()) {
        fillRunning_.store(false);
        fillPaused_.store(false);
        fillCv_.notify_all();
        fillThread_.join();
    }

    initPlayback();

    // ------------------------------------------------------------------
    // DOS 드라이버 버퍼 깊이를 바탕으로 링버퍼 크기 동적 계산
    // (INIT.COM 역어셈블: 버퍼=409 이벤트 노드, 노드 크기=10 bytes)
    // ------------------------------------------------------------------
    ringCapacity_ = calcRingCapacity();
    ringBuf_.assign(ringCapacity_, 0);

    // 링버퍼 및 상태 초기화
    ringRead_.store(0,  std::memory_order_relaxed);
    ringWrite_.store(0, std::memory_order_relaxed);
    seqDone_.store(false, std::memory_order_relaxed);
    songDone_ = false;

    // Fill 스레드 시작 (링버퍼에 선행 생성 시작)
    fillRunning_.store(true);
    fillPaused_.store(false);
    fillThread_ = std::thread(&RolPlayer::fillLoop, this);

    // SDL 오디오 콜백 활성화
    SDL_PauseAudio(0);
}

void RolPlayer::play()
{
    playAsync();
    while (!songDone_) {
        SDL_Delay(50);
    }
    SDL_Delay(300);
    SDL_PauseAudio(1);
}

void RolPlayer::stop()
{
    SDL_PauseAudio(1);

    // Fill 스레드 종료
    fillRunning_.store(false);
    fillPaused_.store(false);
    fillCv_.notify_all();
    if (fillThread_.joinable()) fillThread_.join();

    seqDone_.store(true,  std::memory_order_relaxed);
    songDone_ = true;

    // 모든 OPL 보이스 오프
    for (int v = 0; v < NUM_VOICES; v++) noteOff(v);
    if (!isMelodic_) {
        mAMVibRhythm_ &= 0x20;
        opl_.adlib_write(0xBD, mAMVibRhythm_);
    }
}

// ============================================================
//  Global state for the free-function API
// ============================================================

static RolPlayer*  g_player   = nullptr;  // single shared player instance
static std::string g_bankFile;            // bank path set by LoadBank()

// ============================================================
//  Public free-function API  (declared in rolplayer.h)
// ============================================================

/*
 * InitSound()
 *
 * Creates (or re-creates) the internal RolPlayer, which initialises SDL2
 * audio.  Must be called once before LoadBank / LoadSong / PlaySong.
 * Calling it again tears down the previous instance cleanly.
 */
void InitSound()
{
    delete g_player;
    g_player = new RolPlayer();
}

/*
 * LoadBank(BankFile)
 *
 * Stores the path to the AdLib instrument bank (.BNK) file.
 * The bank is not parsed here; parsing happens inside LoadSong()
 * when load() is called on the player.
 */
void LoadBank(const std::string& BankFile)
{
    g_bankFile = BankFile;
}

/*
 * LoadSong(FileSpec)
 *
 * Parses the bank stored by LoadBank() and the ROL file named by FileSpec,
 * preparing the player for playback.  InitSound() and LoadBank() must have
 * been called first.
 */
void LoadSong(const std::string& FileSpec)
{
    if (!g_player) {
        fprintf(stderr, "[rolplayer] LoadSong: InitSound() has not been called.\n");
        return;
    }
    if (g_bankFile.empty()) {
        fprintf(stderr, "[rolplayer] LoadSong: LoadBank() has not been called.\n");
        return;
    }
    g_player->load(FileSpec.c_str(), g_bankFile.c_str());
}

/*
 * PlaySong()
 *
 * Starts playback asynchronously -- audio runs in the SDL callback thread
 * and this function returns immediately.  Call PlayOff() to stop.
 */
void PlaySong()
{
    if (!g_player) {
        fprintf(stderr, "[rolplayer] PlaySong: InitSound() has not been called.\n");
        return;
    }
    g_player->playAsync();
}

/*
 * PlayOff()
 *
 * Stops playback and silences all OPL voices.
 */
void PlayOff()
{
    if (!g_player) return;
    g_player->stop();
}

/*
 * IsSongPlaying()
 *
 * 링버퍼가 완전히 소진된 후 false를 반환한다.
 * DOS 원본에서 드라이버 큐가 소진된 후 GetState=OFF가 되는 시점과 동일.
 */
bool IsSongPlaying()
{
    if (!g_player) return false;
    return g_player->isPlaying();
}

/*
 * AudioLock() / AudioUnlock()
 *
 * DOS 원본의 Keybd(=Main) 블로킹 동작을 재현한다.
 *
 *   AudioLock()   → fill 스레드(PCM 선행 생성) 일시 정지
 *                   SDL 콜백은 계속 실행 → 링버퍼를 RING_LATENCY_SEC 초에
 *                   걸쳐 소진 → 무음 (DOS 드라이버 큐 소진과 동일)
 *
 *   AudioUnlock() → fill 스레드 재개 → 링버퍼 빠르게 채워짐 → 재생 재개
 *
 * 사용 방법 (main.cpp):
 *   AudioLock();
 *   MainLoopStep(e);   // 블로킹 가능한 게임 로직
 *   AudioUnlock();
 */
void AudioLock()
{
    if (!g_player) return;
    g_player->audioLock();
}

void AudioUnlock()
{
    if (!g_player) return;
    g_player->audioUnlock();
}