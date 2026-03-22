#include "voice.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// =============================================================================
// Section 1.  VOC format constants
// =============================================================================

static constexpr char   VOC_MAGIC[]   = "Creative Voice File\x1A"; // 20 bytes
static constexpr size_t VOC_MAGIC_LEN = 20;

enum VocBlock : uint8_t {
    VBLK_TERMINATE  = 0x00,
    VBLK_SOUND      = 0x01,
    VBLK_SOUND_CONT = 0x02,
    VBLK_SILENCE    = 0x03,
    VBLK_MARKER     = 0x04,
    VBLK_ASCII      = 0x05,
    VBLK_REPEAT     = 0x06,
    VBLK_REPEAT_END = 0x07,
    VBLK_EXTRA      = 0x08,
    VBLK_SOUND_NEW  = 0x09,
};

enum VocCodec : uint16_t {
    VCODEC_PCM8    = 0x0000,
    VCODEC_ADPCM4  = 0x0001,
    VCODEC_ADPCM26 = 0x0002,
    VCODEC_ADPCM2  = 0x0003,
    VCODEC_ALAW    = 0x0006,
    VCODEC_ULAW    = 0x0007,
    VCODEC_PCM16   = 0x0200,
};

// =============================================================================
// Section 2.  Asset search helper + FileReader (std::ifstream 기반)
// =============================================================================



class FileReader {
public:
    explicit FileReader(std::ifstream f) : f_(std::move(f)) {}

    void read(void* dst, size_t n) {
        if (!f_.read(reinterpret_cast<char*>(dst), static_cast<std::streamsize>(n)))
            throw std::runtime_error("Read error at pos "
                                     + std::to_string(static_cast<long long>(f_.tellg())));
    }

    uint8_t  u8()  { uint8_t  v; read(&v, 1); return v; }
    uint16_t u16() {
        uint8_t b[2]; read(b, 2);
        return static_cast<uint16_t>(b[0]) | (static_cast<uint16_t>(b[1]) << 8);
    }
    uint32_t u32() {
        uint8_t b[4]; read(b, 4);
        return  static_cast<uint32_t>(b[0])
              | (static_cast<uint32_t>(b[1]) <<  8)
              | (static_cast<uint32_t>(b[2]) << 16)
              | (static_cast<uint32_t>(b[3]) << 24);
    }
    // 24-bit little-endian (VOC 블록 크기 필드)
    uint32_t u24() {
        uint8_t b[3]; read(b, 3);
        return  static_cast<uint32_t>(b[0])
              | (static_cast<uint32_t>(b[1]) <<  8)
              | (static_cast<uint32_t>(b[2]) << 16);
    }

    int64_t  tell()              { return static_cast<int64_t>(f_.tellg()); }
    void     seekAbs(int64_t p)  { f_.seekg(p, std::ios::beg); }
    void     seekRel(int64_t o)  { f_.seekg(o, std::ios::cur); }
    bool     eof()               { return f_.peek() == EOF; }

private:
    std::ifstream f_;
};

// =============================================================================
// Section 3.  Creative ADPCM decoders (codec 0x01 / 0x02 / 0x03)
// =============================================================================

// ---- 4-bit ADPCM (codec 0x0001) ---------------------------------------------
static constexpr int8_t kAdj4[16] = {
    -2, -1,  0,  1,  2,  3,  4,  5,
     6,  8, 10, 12, 14, 16, 20, 24
};
struct Adpcm4 { uint8_t ref = 128; int8_t step = 0; };

static uint8_t adpcm4_decode(Adpcm4& s, uint8_t nibble) {
    const int diff = (((nibble & 0x7) * 2 + 1) << s.step) >> 3;
    int val = (nibble & 0x8) ? s.ref - diff : s.ref + diff;
    val = std::clamp(val, 0, 255);
    s.ref  = static_cast<uint8_t>(val);
    s.step = static_cast<int8_t>(std::clamp<int>(s.step + kAdj4[nibble & 0xF], 0, 7));
    return s.ref;
}

// ---- 3-bit ADPCM (codec 0x0002, "2.63 bits/sample") -------------------------
static constexpr int8_t kAdj3[8] = { -2, -1, 0, 1, 2, 3, 4, 5 };
struct Adpcm3 { uint8_t ref = 128; int8_t step = 0; };

static uint8_t adpcm3_decode(Adpcm3& s, uint8_t tribble) {
    const int diff = (((tribble & 0x3) * 2 + 1) << s.step) >> 2;
    int val = (tribble & 0x4) ? s.ref - diff : s.ref + diff;
    val = std::clamp(val, 0, 255);
    s.ref  = static_cast<uint8_t>(val);
    s.step = static_cast<int8_t>(std::clamp<int>(s.step + kAdj3[tribble & 0x7], 0, 7));
    return s.ref;
}

// ---- 2-bit ADPCM (codec 0x0003) ---------------------------------------------
static constexpr int8_t kAdj2[4] = { -1, 2, -1, 2 };
struct Adpcm2 { uint8_t ref = 128; int8_t step = 0; };

static uint8_t adpcm2_decode(Adpcm2& s, uint8_t dibit) {
    const int diff = (((dibit & 0x1) * 2 + 1) << s.step) >> 1;
    int val = (dibit & 0x2) ? s.ref - diff : s.ref + diff;
    val = std::clamp(val, 0, 255);
    s.ref  = static_cast<uint8_t>(val);
    s.step = static_cast<int8_t>(std::clamp<int>(s.step + kAdj2[dibit & 0x3], 0, 7));
    return s.ref;
}

// =============================================================================
// Section 4.  A-law / mu-law -> 16-bit linear PCM  (ITU-T G.711)
// =============================================================================

static int16_t alaw_to_linear(uint8_t a) {
    a ^= 0x55;
    const int exp  = (a >> 4) & 0x07;
    int       mant = (a & 0x0F) << 4;
    if (exp != 0) mant = (mant | 0x100) << (exp - 1);
    return static_cast<int16_t>((a & 0x80) ? mant : -mant);
}

static int16_t ulaw_to_linear(uint8_t u) {
    u ^= 0xFF;
    const int t = (((u & 0x0F) << 3) | 0x84) << ((u >> 4) & 0x07);
    return static_cast<int16_t>((u & 0x80) ? (0x84 - t) : (t - 0x84));
}

// =============================================================================
// Section 5.  VocDecoder
// =============================================================================

struct AudioSpec {
    int     sampleRate = 0;
    uint8_t channels   = 1;
    // 출력 포맷은 항상 AUDIO_S16LSB (16-bit signed LE)
};

// 디코딩된 사운드 이펙트: VOC native 샘플링 레이트 그대로 보존
struct SoundEffect {
    AudioSpec            spec;
    std::vector<uint8_t> pcm; // 16-bit signed LE, 채널 interleaved
};

class VocDecoder {
public:
    struct Result {
        AudioSpec            spec;
        std::vector<uint8_t> pcm;
    };

    static Result decodeFile(const std::string& filename) {
        std::string upper_filename = filename;
        for (auto &c : upper_filename) c = toupper((unsigned char)c);
        std::ifstream f("assets/" + upper_filename, std::ios::binary);
        if (!f.is_open()) {
            std::cerr << "[Voice] Failed to open " << filename << std::endl;
            return {};
        }
        FileReader fr(std::move(f));
        VocDecoder dec(fr);
        dec.readHeader();
        dec.readBlocks();
        return std::move(dec.result_);
    }

private:
    explicit VocDecoder(FileReader& fr) : fr_(fr) {}

    // -------------------------------------------------------------------------
    // 5-A. 헤더 파싱
    // -------------------------------------------------------------------------
    void readHeader() {
        char magic[VOC_MAGIC_LEN];
        fr_.read(magic, VOC_MAGIC_LEN);
        if (std::memcmp(magic, VOC_MAGIC, VOC_MAGIC_LEN) != 0)
            throw std::runtime_error("VOC signature mismatch -- not a Creative Voice File");

        const uint16_t hdrSize = fr_.u16();
        const uint16_t version = fr_.u16();
        const uint16_t vcheck  = fr_.u16();

        if (static_cast<uint16_t>(~version + 0x1234u) != vcheck)
            std::cerr << "[Voice] VOC version checksum mismatch\n";

        fr_.seekAbs(hdrSize); // 첫 번째 블록으로 이동
    }

    // -------------------------------------------------------------------------
    // 5-B. 블록 순회 및 디스패치
    // -------------------------------------------------------------------------
    void readBlocks() {
        int     extFreq = -1; // 0x08 블록이 다음 0x01 블록에 전달하는 freq
        uint8_t extChan =  1;

        while (!fr_.eof()) {
            const uint8_t type = fr_.u8();

            if (type == VBLK_TERMINATE) break;

            const uint32_t blkSize = fr_.u24();
            const int64_t  blkEnd  = fr_.tell() + static_cast<int64_t>(blkSize);

            switch (type) {

            // 0x01 Sound Data -------------------------------------------------
            case VBLK_SOUND: {
                const uint8_t  freqDiv  = fr_.u8();
                const uint8_t  codec8   = fr_.u8();
                const uint32_t dataSize = blkSize - 2;

                int freq; uint8_t chan;
                if (extFreq >= 0) {
                    // 직전 0x08 블록의 고정밀 freq/채널 정보 적용
                    freq = extFreq; chan = extChan; extFreq = -1;
                } else {
                    // SR = 1,000,000 / (256 - freqDiv)
                    freq = 1'000'000 / (256 - freqDiv);
                    chan = 1;
                }
                setSpec(freq, chan);
                decode(static_cast<uint16_t>(codec8), dataSize);
                break;
            }

            // 0x02 Sound Continuation -----------------------------------------
            case VBLK_SOUND_CONT:
                decode(prevCodec_, blkSize); // 이전 코덱을 그대로 재사용
                break;

            // 0x03 Silence ----------------------------------------------------
            case VBLK_SILENCE: {
                const uint16_t silLen  = fr_.u16();
                const uint8_t  freqDiv = fr_.u8();
                const int      freq    = 1'000'000 / (256 - freqDiv);
                const uint32_t nSamp   = static_cast<uint32_t>(silLen) + 1;
                setSpec(freq, 1);
                // 16-bit PCM silence = 0x0000
                result_.pcm.insert(result_.pcm.end(),
                                   static_cast<size_t>(nSamp) * 2, 0x00);
                break;
            }

            // 0x04 Marker -----------------------------------------------------
            case VBLK_MARKER:
                fr_.u16();
                break;

            // 0x05 ASCII Text -------------------------------------------------
            case VBLK_ASCII: {
                std::vector<char> txt(blkSize + 1, '\0');
                fr_.read(txt.data(), blkSize);
                break;
            }

            // 0x06 Repeat Start -----------------------------------------------
            case VBLK_REPEAT:
                fr_.u16(); // count (루프는 미구현 -- 1회 재생)
                break;

            // 0x07 Repeat End -------------------------------------------------
            case VBLK_REPEAT_END:
                break;

            // 0x08 Extended Info ----------------------------------------------
            //  [2B] freq divisor  [1B] codec  [1B] channels-1
            //  다음 0x01 블록에 고해상도 / 스테레오 정보를 전달함
            case VBLK_EXTRA: {
                const uint16_t freqDiv16 = fr_.u16();
                fr_.u8();                          // codec (0x01 블록에서 재독)
                const uint8_t chanExt = fr_.u8();  // 0=mono, 1=stereo
                extChan = chanExt + 1;
                // Creative formula: SR = 256,000,000 / (ch * (65536 - divisor))
                extFreq = static_cast<int>(
                    256'000'000.0 / (extChan * (65536 - freqDiv16)));
                break;
            }

            // 0x09 New Sound Data (VOC v1.20+) --------------------------------
            //  [4B] sample rate  [1B] bits/sample  [1B] channels
            //  [2B] codec        [4B] reserved      data...
            case VBLK_SOUND_NEW: {
                const uint32_t sr    = fr_.u32();
                fr_.u8();                      // bits/sample (정보 참조용)
                const uint8_t  chan  = fr_.u8();
                const uint16_t codec = fr_.u16();
                fr_.seekRel(4);                // reserved 필드 스킵
                const uint32_t dataSize = blkSize - 12;
                setSpec(static_cast<int>(sr), chan);
                decode(codec, dataSize);
                break;
            }

            default:
                std::cerr << "[Voice] Unknown VOC block 0x"
                          << std::hex << (int)type << std::dec
                          << " size=" << blkSize << " -- skipping\n";
                break;
            }

            fr_.seekAbs(blkEnd); // 블록 경계 정렬
        }
    }

    // -------------------------------------------------------------------------
    // 5-C. 오디오 스펙 확정 (최초 호출 시 고정)
    // -------------------------------------------------------------------------
    void setSpec(int sr, uint8_t chan) {
        if (result_.spec.sampleRate == 0) {
            result_.spec.sampleRate = sr;
            result_.spec.channels   = chan;
        } else if (result_.spec.sampleRate != sr || result_.spec.channels != chan) {
            std::cerr << "[Voice] Format mismatch between blocks ("
                      << sr << " Hz / " << (int)chan << "ch)"
                      << " -- keeping first format (no resampling)\n";
        }
    }

    // -------------------------------------------------------------------------
    // 5-D. 코덱 디스패치
    // -------------------------------------------------------------------------
    void decode(uint16_t codec, uint32_t size) {
        prevCodec_ = codec;
        switch (codec) {
        case VCODEC_PCM8:    pcm8(size);    break;
        case VCODEC_ADPCM4:  adpcm4(size);  break;
        case VCODEC_ADPCM26: adpcm3(size);  break;
        case VCODEC_ADPCM2:  adpcm2(size);  break;
        case VCODEC_ALAW:    alaw(size);    break;
        case VCODEC_ULAW:    ulaw(size);    break;
        case VCODEC_PCM16:   pcm16(size);   break;
        default:
            std::cerr << "[Voice] Unsupported codec 0x"
                      << std::hex << codec << std::dec << " -- skipping\n";
            fr_.seekRel(static_cast<int64_t>(size));
            break;
        }
    }

    // -------------------------------------------------------------------------
    // 5-E. 코덱 구현체
    // -------------------------------------------------------------------------

    // 8-bit unsigned PCM -> 16-bit signed PCM
    void pcm8(uint32_t size) {
        grow(size * 2);
        for (uint32_t i = 0; i < size; ++i) push(u8s16(fr_.u8()));
    }

    // 16-bit signed PCM (LE) -> 그대로 통과
    void pcm16(uint32_t size) {
        grow(size);
        for (uint32_t i = 0; i < size / 2; ++i)
            push(static_cast<int16_t>(fr_.u16()));
    }

    // 4-bit Creative ADPCM -> 16-bit signed PCM
    //   첫 번째 바이트: 기준 샘플 / 이후: 상위 nibble, 하위 nibble 순으로 디코딩
    void adpcm4(uint32_t size) {
        if (size == 0) return;
        Adpcm4 st; st.ref = fr_.u8();
        grow((size - 1) * 4);
        for (uint32_t i = 1; i < size; ++i) {
            const uint8_t b = fr_.u8();
            push(u8s16(adpcm4_decode(st, (b >> 4) & 0xF)));
            push(u8s16(adpcm4_decode(st,  b       & 0xF)));
        }
    }

    // 3-bit Creative ADPCM -> 16-bit signed PCM
    //   첫 번째 바이트: 기준 샘플 / 이후 3바이트마다 8개의 tribble 추출 (MSB first)
    void adpcm3(uint32_t size) {
        if (size == 0) return;
        Adpcm3 st; st.ref = fr_.u8();
        uint32_t rem = size - 1;
        grow(rem / 3 * 16 + 32);
        while (rem >= 3) {
            const uint8_t b0 = fr_.u8();
            const uint8_t b1 = fr_.u8();
            const uint8_t b2 = fr_.u8();
            rem -= 3;
            const uint32_t w = (static_cast<uint32_t>(b0) << 16)
                             | (static_cast<uint32_t>(b1) <<  8)
                             |  static_cast<uint32_t>(b2);
            for (int j = 7; j >= 0; --j)
                push(u8s16(adpcm3_decode(st, (w >> (j * 3)) & 0x7)));
        }
        if (rem > 0) fr_.seekRel(static_cast<int64_t>(rem));
    }

    // 2-bit Creative ADPCM -> 16-bit signed PCM
    //   첫 번째 바이트: 기준 샘플 / 이후 바이트당 4개의 dibit (bit7-6, 5-4, 3-2, 1-0)
    void adpcm2(uint32_t size) {
        if (size == 0) return;
        Adpcm2 st; st.ref = fr_.u8();
        grow((size - 1) * 8);
        for (uint32_t i = 1; i < size; ++i) {
            const uint8_t b = fr_.u8();
            push(u8s16(adpcm2_decode(st, (b >> 6) & 0x3)));
            push(u8s16(adpcm2_decode(st, (b >> 4) & 0x3)));
            push(u8s16(adpcm2_decode(st, (b >> 2) & 0x3)));
            push(u8s16(adpcm2_decode(st,  b       & 0x3)));
        }
    }

    // A-law -> 16-bit signed PCM
    void alaw(uint32_t size) {
        grow(size * 2);
        for (uint32_t i = 0; i < size; ++i) push(alaw_to_linear(fr_.u8()));
    }

    // mu-law -> 16-bit signed PCM
    void ulaw(uint32_t size) {
        grow(size * 2);
        for (uint32_t i = 0; i < size; ++i) push(ulaw_to_linear(fr_.u8()));
    }

    // -------------------------------------------------------------------------
    // 5-F. 유틸리티
    // -------------------------------------------------------------------------

    // 8-bit unsigned (0-255) -> 16-bit signed PCM (128 -> 0)
    static int16_t u8s16(uint8_t u) {
        return static_cast<int16_t>((static_cast<int>(u) - 128) * 256);
    }

    // 16-bit 샘플을 little-endian으로 PCM 버퍼에 추가
    void push(int16_t s) {
        result_.pcm.push_back(static_cast<uint8_t>( s        & 0xFF));
        result_.pcm.push_back(static_cast<uint8_t>((s >> 8)  & 0xFF));
    }

    void grow(size_t extra) { result_.pcm.reserve(result_.pcm.size() + extra); }

    // -------------------------------------------------------------------------
    FileReader& fr_;
    Result      result_;
    uint16_t    prevCodec_ = VCODEC_PCM8;
};

// =============================================================================
// Section 6.  사운드 이펙트 저장소 (앱 시작 시 미리 디코딩)
// =============================================================================

static SoundEffect sndScream1;
static SoundEffect sndScream2;
static SoundEffect sndHit;
static bool        g_initialized = false;

static SoundEffect LoadVOC(const std::string& filename) {
    SoundEffect snd;
    try {
        auto result = VocDecoder::decodeFile(filename);
        if (result.pcm.empty() || result.spec.sampleRate == 0) {
            std::cerr << "[Voice] No audio data decoded from " << filename << "\n";
            return snd;
        }
        snd.spec = result.spec;
        snd.pcm  = std::move(result.pcm);
    } catch (const std::exception& e) {
        std::cerr << "[Voice] " << filename << ": " << e.what() << "\n";
    }
    return snd;
}

static void InitSound() {
    if (g_initialized) return;
    g_initialized = true;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cerr << "[Voice] SDL_INIT_AUDIO failed: " << SDL_GetError() << "\n";
        return;
    }

    // VOC 파일을 native 샘플링 레이트 그대로 디코딩 (리샘플링 없음)
    sndScream1 = LoadVOC("scream1.voc");
    sndScream2 = LoadVOC("scream2.voc");
    sndHit     = LoadVOC("hit.voc");
}

// =============================================================================
// Section 7.  SDL2 오디오 재생 (SDL_QueueAudio push 모델)
//
//  vocplayer.cpp의 play() 패턴과 동일:
//    1. VOC native freq/channels 로 오디오 디바이스 오픈
//    2. SDL_QueueAudio 로 PCM 데이터 전달
//    3. SDL_GetQueuedAudioSize 가 0 이 될 때까지 대기 (재생 완료 후 리턴)
//    4. 오디오 디바이스 닫기
// =============================================================================

static void PlaySoundEffect(const SoundEffect& snd) {
    if (snd.pcm.empty() || snd.spec.sampleRate == 0) return;

    SDL_AudioSpec want{}, got{};
    want.freq     = snd.spec.sampleRate;   // VOC 파일의 native 샘플링 레이트
    want.format   = AUDIO_S16LSB;
    want.channels = snd.spec.channels;
    want.samples  = 4096;
    want.callback = nullptr;               // push 모델 -- 콜백 없음

    const SDL_AudioDeviceID dev =
        SDL_OpenAudioDevice(nullptr, 0, &want, &got, 0);
    if (dev == 0) {
        std::cerr << "[Voice] SDL_OpenAudioDevice failed: " << SDL_GetError() << "\n";
        return;
    }

    if (SDL_QueueAudio(dev, snd.pcm.data(),
                       static_cast<uint32_t>(snd.pcm.size())) < 0) {
        std::cerr << "[Voice] SDL_QueueAudio failed: " << SDL_GetError() << "\n";
        SDL_CloseAudioDevice(dev);
        return;
    }

    SDL_PauseAudioDevice(dev, 0); // 재생 시작

    // 재생이 완전히 끝날 때까지 대기한 후 리턴
    while (SDL_GetQueuedAudioSize(dev) > 0)
        SDL_Delay(50);

    SDL_CloseAudioDevice(dev);
}

// =============================================================================
// Section 8.  공개 API
// =============================================================================

void UnSound() {
    if (!g_initialized) return;
    g_initialized = false;
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void scream1() { InitSound(); PlaySoundEffect(sndScream1); }
void scream2() { InitSound(); PlaySoundEffect(sndScream2); }
void hit()     { InitSound(); PlaySoundEffect(sndHit); }
