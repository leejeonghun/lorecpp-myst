#ifndef MYSTSUB_H
#define MYSTSUB_H

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <SDL2/SDL.h>

// Pascal types mapping
using byte = uint8_t;
using word = uint16_t;
using integer = int16_t; 
using longint = int32_t;
using pathstr = std::string;

// Enums
enum ScrollTypes { normal, snow, rain, autumn, wilderness, strongrain };
enum PositionType { town, ground, den, keep };
enum Classes { sword, magic, elemental, unknown, giant, golem, dragon };
enum SexType { male, female, neutral };

#if defined(__GNUC__) || defined(__clang__)
#define PACKED __attribute__((packed))
#else
#define PACKED
#pragma pack(push, 1)
#endif

struct PACKED FontType {
    uint8_t data[300]; 
};

struct PACKED Map_Header_Type {
    uint8_t ID_len;
    char ID[10];
    byte xmax, ymax;
    uint8_t Tile_Type;
    uint8_t encounter, handicap;
    byte startX, startY;
    uint8_t exitmap_len;
    char exitmap[10];
    byte exitX, exitY;
    uint8_t entermap_len;
    char entermap[10];
    byte enterX, enterY, default_val, handicap_bit;
    uint8_t unused_len;
    char unused[49];
};

struct PACKED Lore {
    uint8_t name_len;
    char name[17];
    uint8_t sex;
    byte _class;
    byte strength;
    byte mentality;
    byte concentration;
    byte endurance;
    byte resistance;
    byte agility;
    byte accuracy;
    byte luck;
    byte poison;
    integer unconscious;
    integer dead;
    integer hp;
    integer sp;
    byte ac;
    byte level;
    longint experience;
    longint potential_experience;
    byte weapon;
    byte shield;
    byte armor;
    byte potential_ac;
    byte wea_power;
    byte shi_power;
    byte arm_power;
    
    uint8_t classtype;
    union {
        struct PACKED {
            byte sword_skill, axe_skill, spear_skill, bow_skill, shield_skill, fist_skill;
        } sword_data;
        struct PACKED {
            byte attack_magic, pheno_magic, cure_magic, special_magic, esp_magic, summon_magic;
        } magic_data;
        byte raw_skills[6];
    };

    // UTF-8 환경에서 이름 숨김/복원을 위한 백업 필드.
    // DOS 세이브 파일(DOS_Lore)에는 저장되지 않으며, 런타임 전용입니다.
    // Pascal 원본의 name[9..17] 트릭을 UTF-8 안전하게 대체합니다.
    char    name_hidden[17];
    uint8_t name_hidden_len;
};

struct PACKED BackPackType {
    uint8_t data[6][10];
};

struct PACKED LorePlayer {
    integer year, day;
    byte hour, min, sec;
    byte food;
    longint gold;
    word arrow;
    byte checksum[2];
    byte item[10];
    byte crystal[10];
    BackPackType BackPack;
    byte etc[100];
};

struct PACKED MapType {
    uint8_t data[150][120];
};

// ---------------------------------------------------------------------------
// 원본 DOS 파스칼 파일 바이너리 매핑용 구조체 (1바이트 정렬 필수)
// ---------------------------------------------------------------------------
struct PACKED DOS_PascalString17 { uint8_t len; char text[17]; };

struct PACKED DOS_Lore {
    DOS_PascalString17 name;
    uint8_t sex;
    uint8_t _class;
    uint8_t strength, mentality, concentration, endurance, resistance, agility, accuracy, luck, poison;
    int16_t unconscious, dead, hp, sp;
    uint8_t ac, level;
    int32_t experience, potential_experience;
    uint8_t weapon, shield, armor, potential_ac, wea_power, shi_power, arm_power;
    uint8_t classtype;
    uint8_t raw_skills[6];
};

struct PACKED DOS_LorePlayer {
    int16_t year, day;
    uint8_t hour, min, sec, food;
    int32_t gold;
    uint16_t arrow;
    uint8_t checksum[2];
    uint8_t item[10];
    uint8_t crystal[10];
    uint8_t backpack[6][10];
    uint8_t etc[100];
};

// ---------------------------------------------------------------------------

#if !defined(__GNUC__) && !defined(__clang__)
#pragma pack(pop)
#endif
#undef PACKED

struct EnemyData1 {
    std::string name;
    byte strength;
    byte mentality;
    byte endurance;
    byte resistance[2];
    byte agility;
    byte accuracy[2];
    byte ac;
    byte special;
    byte castlevel;
    byte specialcastlevel;
    byte level;
};

struct EnemyData2 {
    byte E_number;
    std::string name;
    byte strength;
    byte mentality;
    byte endurance;
    byte resistance[2];
    byte agility;
    byte accuracy[2];
    byte ac;
    byte special;
    byte castlevel;
    byte specialcastlevel;
    byte level;
    integer hp;
    byte aux_hp;
    bool poison;
    bool unconscious;
    bool dead;
};

// Global Variables (extern)
extern bool absolute_set_xy;
extern bool ebony;
extern bool Time_Watch;
extern int i, j, k;
extern byte b;
extern word h;
extern longint long_val;
extern char c, LoadNo;
extern std::string Josa, Mokjuk; 
extern std::string sexdata;
extern std::string s_val;
extern std::string m[11];
extern int pas_x, pas_y, pas_x1, pas_y1;
extern bool ok, Allright, AutoBattle, aux, LoadFont, Loading_SaveGame, AdLibOn, SoundOn;
extern bool Cruel;
extern ScrollTypes scrolltype;
extern PositionType position;
extern Map_Header_Type Map_Header;

extern byte* encounter;
extern byte* maxenemy;
extern pathstr existfile;
enum ImageDirection { _DIRECT, _STORE };
extern ImageDirection _IMAGE_DIRECTION;

extern byte scrollform, scrollcolor, putstyle, yline, face, Rest_Time;
extern int person;
extern bool QuitPlay;

extern std::vector<FontType> font;
extern std::vector<FontType> chara;
extern MapType* map;

extern std::string mapname;
extern byte battle[7][4];
extern integer watch_year, watch_day;
extern byte watch_hour, watch_min, watch_sec, time_event_no;

extern LorePlayer party;
extern Lore player[8];
extern byte enemynumber;
extern EnemyData2 enemy[9];

// Functions
std::string FromPas(const char* buf, uint8_t len);
void Restore_Palette();
void Clear();
void PressAnyKey();
void Screen_Fade_Out(int delay_ms);
void Screen_Fade_In(int delay_ms);
void OverLap(int delay_ms, bool character);
enum PutMode { COPY_PUT, AND_PUT, OR_PUT, XOR_PUT };
void Image(int X, int Y, void* BitMap, bool Darkness, PutMode mode = COPY_PUT);
byte Real_Map(byte x, byte y);
byte GetMap(byte x, byte y);
void PutMap(byte x, byte y, byte value);
void PutSprite(int x, int y, int number);
void PutTile(int x, int y, int number);
void setscrolltype(ScrollTypes form);
void Clear_View_Screen();
void Scroll(bool character);
void Silent_Scroll();
void Store_Scroll(bool character);
bool exist(int person_idx);
void PlusTime(int _hour, int _min, int _sec);
void Print(int color, std::string s1);
void AuxPrint(int color, std::string s1, bool newline);
void cPrint(int color1, int color2, std::string s1, std::string s2, std::string s3);
void message(int color, std::string s_msg);
int SelectSub(int yinit, int select_init, int maxsum, int total, bool clean, bool lastclean);
int Select(int yinit, int maxsum, int total, bool clean, bool lastclean);
bool Restore_Game();
void GameOver();
void DetectGameOver();
int SelectEnemy();
int ChooseWhomSub(int yinit, bool printing, bool clean);
int ChooseWhom(bool clean);
std::string ReturnSex(int number);
std::string ReturnClass(int number);
std::string ReturnClassType(int number);
bool ReturnClassEnable(int number, int classnumber);
std::string ReturnWeapon(int number);
std::string ReturnShield(int number);
std::string ReturnArmor(int number);
std::string ReturnCondition(int number);
std::string ReturnMagic(int _class, int magic);
std::string ReturnSJosa(std::string s);
std::string ReturnBJosa(std::string s);
std::string ReturnOJosa(std::string s);
std::string ReturnMessage(int who, int how, int what, int whom);
void ReturnSexData();
void DisplayHP();
void DisplaySP();
void DisplayCondition();
void SimpleDisCond();
void Display_Condition();
bool at(int xx, int yy);
bool on(int xx, int yy);
void originposition();
bool wantenter(std::string name);
bool wantexit();
void DefaultEnter();
void DefaultExit();
void FindGold(longint money);
void setbit(int bit);
bool getbit(int bit);
void asyouwish();
void notenoughmoney();
void thankyou();
void join(byte num, byte partynum);
void joinenemy(byte num, byte j);
void turn_mind(byte j, byte enemy_num);
void Grocery();
void Weapon_Shop();
void About_Class();
void Train_Center();
void Magic_School();
void Change_Job_for_Sword();
void Change_Job_for_Magic();
bool Verify_Weapon(byte player_no, byte weapon_no);
bool Verify_Shield(byte player_no, byte shield_no);
bool Verify_Armor(byte player_no, byte armor_no);
void Modify_Item(byte player_no);
void Hospital();
void Medicine();
void Sell_Item();
void Load();
void Save();
void Set_All();
void UnSound();

void Delete_Scroll_Screen();
longint ReturnEXP(byte level);
void Sell_EXP();
void InitMystSub();
extern const EnemyData1 EnemyData_Const[75];

#endif

extern int g_HWinX1, g_HWinY1, g_HWinX2, g_HWinY2;
