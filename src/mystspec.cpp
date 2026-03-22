#include <cstring>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cstdio>

#include "mystspec.h"
#include "mystsub.h"
#include "uhanx.h"
#include "mystbatt.h"
#include "mystend.h"
#include "voice.h"

// ---------------------------------------------------------------------------
// 공통 이벤트 및 능력치 상승 서브루틴
// ---------------------------------------------------------------------------

static void Plus_Ability_Sub(std::string st) {
    const long price_data_sub[30] = {
        30,60,125,250,500,1000,2000,3000,4000,6000,
        8000,12000,16000,24000,32000,64000,96000,128000,192000,256000,
        384000,512000,770000,1000000,1500000,2000000,2500000,3000000,3500000,4000000
    };
    
    Print(7, " 당신은 나의 문제를 모두 맞추었소.  그 보답");
    Print(7, "으로,  당신이 황금을 나에게 바친다면 당신들");
    Print(7, "중의 한사람의 " + st + "을 한 단계 올려 주겠소.");
    
    int k = ChooseWhomSub(130, false, false);
    if (k == 0) return;
    
    byte* ability = nullptr;
    int max_val = 0;
    int price_offset = 0;
    
    if (st == "정신력") { max_val = 30; ability = &player[k].mentality; }
    else if (st == "인내력") { max_val = 30; ability = &player[k].endurance; }
    else if (st == "체력") { max_val = 30; ability = &player[k].strength; }
    else if (st == "민첩성") { max_val = 20; ability = &player[k].agility; }
    else if (st == "정확성") { max_val = 20; ability = &player[k].accuracy; price_offset = 1; }

    if (!ability) return;

    std::string name = player[k].name;
    if (*ability >= max_val) {
        Print(7, name + ReturnSJosa(name) + " 지금 최고의 " + st + " 수치입니다.");
        return;
    }
    
    long required_gold = price_data_sub[*ability + price_offset];
    
    if (party.gold < required_gold) { 
        Print(7, "일행은 " + std::to_string(required_gold) + " 개의 황금이 필요합니다.");
        return;
    }
    
    party.gold -= required_gold;
    (*ability)++;
    
    Print(15, name + ReturnSJosa(name) + "의 " + st + "은 이제 " + std::to_string(*ability) + "입니다.");
}

void Plus_Mentality() { Plus_Ability_Sub("정신력"); }
void Plus_Endurance() { Plus_Ability_Sub("인내력"); }
void Plus_Strength() { Plus_Ability_Sub("체력"); }
void Plus_Agility() { Plus_Ability_Sub("민첩성"); }
void Plus_Accuracy() { Plus_Ability_Sub("정확성"); }

void Find_Gold(int bit, long value) {
    if (!getbit(bit)) {
        FindGold(value);
        setbit(bit);
    }
}

void Find_Item(uint8_t& param, int bit, int item_number, int value) {
    const char* data[10] = {
        "체력 회복약을", "마법 회복약을", "해독의 약초를",
        "의식의 약초를", "부활의 약초를", "소환 문서를",
        "대형 횃불을", "수정 구슬을", "비행 부츠를", "이동 구슬을"
    };
    if ((param & bit) == 0) {
        Clear();
        Print(7, "일행은 " + std::string(data[item_number - 1]) + " " + std::to_string(value) + "개 발견했다.");
        if (party.item[item_number] + value < 256)
            party.item[item_number] += value;
        else
            party.item[item_number] = 255;
        param |= bit;
    }
}

int sgn(int eq) {
    if (eq > 0) return 1;
    else if (eq < 0) return -1;
    else return 0;
}

bool Random_Order(int max_no) {
    std::vector<int> data(max_no + 1);
    for (int i = 0; i <= max_no; i++) data[i] = i;
    
    for (int i = max_no; i >= 1; i--) {
        int temp = (std::rand() % max_no) + 1;
        std::swap(data[i], data[temp]);
    }
    
    std::string aux_m[11];
    for(int i = 0; i <= 10; i++) aux_m[i] = m[i];
    
    for(int i = 1; i <= max_no; i++) {
        m[i] = aux_m[data[i]];
    }
    
    int k = Select(180, max_no, max_no, false, true);
    return (data[k] == max_no);
}

// ---------------------------------------------------------------------------
// 맵 전용 이벤트 분기 1 (필드 및 일부 던전)
// ---------------------------------------------------------------------------
static void SpecialEvent_Sub1() {
    Scroll(true);

    if (mapname == "Ground1" || mapname == "GROUND1") {
        if (on(30, 20) && (!getbit(51))) {
            Clear();
            enemynumber = 7;
            joinenemy(1, 36);
            enemy[1].name = "폴록스";
            for (int i = 2; i <= 7; i++) {
                joinenemy(i, 34);
                enemy[i].name = "강도";
            }
            DisplayEnemies(true);
            Print(7, " 당신이 길을 가던 도중에 갑자기 강도가 나타");
            Print(7, "났다.  그리고 제일 앞에 있는 여자 강도가 말");
            Print(7, "을 꺼냈다.");
            Print(7, "");
            Print(13, " 이런 곳에 겁도 없이 돌아다니다니, 하하하.");
            Print(13, " 우리 7 명의 능력을 당해내지 못할 것 같으면");
            Print(13, "순순히 가진 돈을 다 내놓아라.");
            setbit(51);
            m[0] = "";
            m[1] = "가진 돈을 모두 준다";
            m[2] = "선제 공격을 가한다";
            if (Select(190, 2, 2, false, true) == 1) {
                party.gold = 0;
                Scroll(true);
                Print(7, " 당신은 강도에게 모든 돈을 빼았겼다.");
                return;
            }
            BattleMode(true, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) {
                Silent_Scroll();
                return;
            }
            Clear();
            Print(13, " 윽!! 나의 패배를 인정하겠다.  나는 원래 암");
            cPrint(13, 11, "살자로 일하던 ", "폴록스", "라고한다. 수련을 목적으");
            Print(13, "로 당신과 동행하고 싶다. 당신은 어떤가?");
            m[0] = "";
            m[1] = "좋소, 허락하겠소";
            m[2] = "그렇게는 않되오";
            if (Select(150, 2, 2, false, true) == 1) {
                int k = 1;
                while (k <= 6 && exist(k)) k++;
                if (k <= 6) {
                    strcpy(player[k].name, "폴록스");
                    player[k].name_len = strlen("폴록스");
                    player[k].sex = 2; // female
                    player[k]._class = 6;
                    player[k].classtype = 2; // sword
                    player[k].level = 5;
                    player[k].strength = 15;
                    player[k].mentality = 19;
                    player[k].concentration = 20;
                    player[k].endurance = 11;
                    player[k].resistance = 18;
                    player[k].agility = 20;
                    player[k].accuracy = 17;
                    player[k].luck = 10;
                    player[k].poison = 0;
                    player[k].unconscious = 0;
                    player[k].dead = 0;
                    player[k].hp = 1;
                    player[k].sp = 0;
                    player[k].experience = 0;
                    player[k].potential_experience = ReturnEXP(player[k].level);
                    player[k].weapon = 18;
                    player[k].shield = 0;
                    player[k].armor = 1;
                    player[k].potential_ac = 2;
                    player[k].sword_data.sword_skill = 10;
                    player[k].sword_data.axe_skill = 0;
                    player[k].sword_data.spear_skill = 35;
                    player[k].sword_data.bow_skill = 10;
                    player[k].sword_data.shield_skill = 0;
                    player[k].sword_data.fist_skill = 20;
                    Modify_Item(k);
                    setbit(45);
                    Silent_Scroll();
                    Display_Condition();
                    Clear();
                } else {
                    Print(7, " 벌써 사람이 모두 채워져 있군.  다음 기회를");
                    Print(7, "를 기다리지.");
                    return;
                }
            } else {
                asyouwish();
            }
        }
        if (on(84, 86)) {
            if (party.year > 699) return;
            if (wantenter("하데스 게이트")) {
                mapname = "HdsGate";
                Load();
                Scroll(true);
                return;
            }
        }
    }

    if (mapname == "Lore" || mapname == "LORE") {
        if (on(44, 10) && (party.etc[20] == 0)) {
            for (int i = 9; i >= 7; i--) {
                if ((i + 1) <= 9) PutTile(i + 1, 6, 44);
                PutTile(i, 6, 53);
                Delay(1000);
            }
            Clear();
            Print(9, " 어이, " + FromPas(player[1].name, player[1].name_len) + ". 로드안님이 널 찾으신다. 따라와!");
            PressAnyKey();
            PutTile(6, 6, 44);
            Delay(2000);
            int j = 2; if (player[1].classtype == 1) j += 8; // magic = 1
            for (int i = 6; i <= 10; i++) {
                if (i < 10) PutTile(i, 6, 44);
                PutTile(i - 1, 6, 44);
                if (i < 9) PutTile(i + 1, 6, 53);
                if (i < 10) PutSprite(i, 6, j);
                Delay(800);
            }
            Delete_Scroll_Screen();
            Delay(2000);
            pas_x = 51; pas_y = 31;
            bool aux = SoundOn;
            SoundOn = false;
            Scroll(false);
            SoundOn = aux;
            int b = 1; if (player[1].classtype == 1) b += 8;
            int tile_i, tile_j;
            for (int k = 11; k >= 6; k--) {
                if ((k + 1) <= 11) {
                    PutTile(4, k + 1, tile_i); PutTile(5, k + 1, tile_j);
                }
                tile_i = Real_Map(50, k + 25); tile_j = Real_Map(51, k + 25);
                PutSprite(4, k, 13); PutSprite(5, k, b);
                Delay(700);
            }
            std::string s = ReturnBJosa(player[1].name);
            if (s != "이") s = "";
            Print(10, " 음.. 당신이 " + FromPas(player[1].name, player[1].name_len) + s + "라는 사람이오?");
            Print(10, " 내가 당신을 특별히 부른 이유는  다름이 아");
            Print(10, "니라  당신을 특별 사면 시켜주려는 의도에서");
            Print(10, "라오.");
            PressAnyKey();
            Print(10, " 당신이  살인 죄로 잡혀오기 전까지는  주위");
            Print(10, "사람들에게 매우 인정받은 용감한 사람이었다");
            Print(10, "는 것을 알고 있소.  내가 지금 부탁하는  몇");
            Print(10, "가지를 들어 준다면 당신에게 그 댓가로 자유");
            Print(10, "로운 삶을 보장하겠소.");
            PressAnyKey();
            for (int k = 7; k <= 12; k++) {
                PutTile(4, k - 1, tile_i);
                tile_i = Real_Map(50, k + 25);
                if (k < 12) PutSprite(4, k, 13);
                Delay(700);
            }
            Print(7, " 674년 1월 17일 날, 로어 세계에서는 커다란");
            Print(7, "참변이 있었소. 당신은 이전에도 인류와 타종");
            Print(7, "족간에 사이가 않좋았다는 것을 기억 할 거요.");
            Print(7, "또한  사소한 영토분쟁이 많았던 것도 사실이");
            Print(7, "오.  그런데 바로 이날 수백년의 역사를 지님");
            Print(7, "아프로디테 테라의 베스퍼성이 트롤족에 의해");
            Print(7, "완전히 함락 당했던 것이오. 거기서 가까스로");
            Print(7, "탈출한 사람들의 말을 빌면 그 날 베스퍼성은");
            Print(7, "처절한 절규와  아비규환 속에서  아이들이나");
            Print(7, "부녀자 할것없이 모두 처참히 죽어 갔다고 하");
            Print(7, "오.  트롤족은 시체가 즐비한 거리에서  환각");
            Print(7, "작용제를 맞으며  광적인 살인을 계속 해대었");
            Print(7, "다고 하는 탈출자의 말을 듣고  나는 분을 참");
            Print(7, "지 못해 모든 성주들을 불러서 타종족과의 전");
            Print(7, "쟁을 결의 했소. 이렇게 함으로서 베스퍼성에");
            Print(7, "서 죽어간 사람들의 원혼을 달래 주려는 의도");
            Print(7, "라오.");
            Print(7, "");
            PressAnyKey();
            party.etc[20] = 2;
            setbit(0);
        }
        
        if (on(46, 10) && !(getbit(35) || getbit(50))) {
            int k = 1;
            while (k <= 6 && exist(k)) k++;
            if (k > 6) return;

            for (int i = 1; i <= 3; i++) {
                if ((i - 1) >= 1) PutTile(i - 1, 6, 44);
                PutTile(i, 6, 53);
                Delay(500);
            }
            Clear();
            Print(9, " " + FromPas(player[1].name, player[1].name_len) + ".");
            Print(9, " 나도 역시 자네처럼 베스퍼성으로 가고 싶네.");
            Print(9, "이런 감옥에서 평생을 지내느니 한 번 모험을");
            Print(9, "해보는게 좋을 것 같아서 말이지.  나는 전사");
            Print(9, "로서의 교육도 받았으니 아마 자네 일원 중에");
            Print(9, "서는 가장 전투력이 뛰어날 걸세.");
            m[0] = "";
            m[1] = "역시 카노푸스 자네 뿐이야";
            m[2] = "자네 도움까지는 필요 없다네";
            if (Select(200, 2, 2, false, true) == 1) {
                for (int i = 8; i >= 5; i--) {
                    PutTile(i + 1, 7, 44);
                    PutTile(i, 7, 53);
                    Delay(1500);
                }
                Print(7, " 그렇다면 카노푸스도 당신과 같은 목적을 가");
                Print(7, "진다는 이유로  사면 시켜 달라는  애기군요.");
                Print(7, "그럼 로드안 님에게 물어보고 오겠습니다.");
                PressAnyKey();
                for (int i = 6; i <= 10; i++) {
                    PutTile(i - 1, 7, 44);
                    if (i < 10) PutTile(i, 7, 53);
                    Delay(500);
                }
                Delete_Scroll_Screen();
                Print(7, " 그로부터 30분이 지났다.");
                PlusTime(0, 30, 0);
                PressAnyKey();
                Scroll(true);
                PutTile(3, 6, 53);
                for (int i = 9; i >= 5; i--) {
                    if ((i + 1) <= 9) PutTile(i + 1, 7, 44);
                    PutTile(i, 7, 53);
                    Delay(1000);
                }
                Print(7, " 카노푸스를 풀어줘도 좋다고  로드안님이 허");
                Print(7, "락하셨습니다. 정말 희안한 일입니다. 예전에");
                Print(7, "는 전혀 없던 일인데 ... 어쨌든 카노푸는 이");
                Print(7, "제 자유의 몸입니다.");
                PressAnyKey();
                PutTile(4, 6, 44); Delay(1000);
                PutTile(3, 6, 44); PutTile(4, 6, 53); Delay(1000);
                PutTile(4, 6, 44);

                k = 1;
                while (k <= 6 && exist(k)) k++;
                if (k <= 6) {
                    strcpy(player[k].name, "카노푸스");
                    player[k].name_len = strlen("카노푸스");
                    player[k].sex = 1; // male
                    player[k]._class = 7;
                    player[k].classtype = 2; // sword
                    player[k].level = 5;
                    player[k].strength = 19;
                    player[k].mentality = 6;
                    player[k].concentration = 4;
                    player[k].endurance = 18;
                    player[k].resistance = 17;
                    player[k].agility = 20;
                    player[k].accuracy = 17;
                    player[k].luck = 12;
                    player[k].poison = 0;
                    player[k].unconscious = 0;
                    player[k].dead = 0;
                    player[k].hp = player[k].endurance * player[k].level * 10;
                    player[k].sp = 0;
                    player[k].experience = 0;
                    player[k].potential_experience = ReturnEXP(player[k].level);
                    player[k].weapon = 0;
                    player[k].shield = 0;
                    player[k].armor = 0;
                    player[k].potential_ac = 2;
                    player[k].sword_data.sword_skill = 20;
                    player[k].sword_data.axe_skill = 10;
                    player[k].sword_data.spear_skill = 5;
                    player[k].sword_data.bow_skill = 0;
                    player[k].sword_data.shield_skill = 50;
                    player[k].sword_data.fist_skill = 10;
                    Modify_Item(k);
                    PutMap(41, 10, 44);
                    setbit(35);
                    Display_Condition();
                    Clear();
                }
                for (int i = 6; i <= 9; i++) {
                    PutTile(i - 1, 7, 44);
                    PutTile(i, 7, 53);
                    Delay(500);
                }
                Silent_Scroll();
            } else {
                for (int i = 2; i >= 0; i--) {
                    PutTile(i + 1, 6, 44);
                    if (i > 0) PutTile(i, 6, 53);
                    Delay(1000);
                }
            }
        }
        if (pas_y == 95) {
            if (wantexit()) DefaultExit();
            else { pas_y--; Silent_Scroll(); }
        }
    }

    if (mapname == "LastDtch" || mapname == "LASTDTCH") {
        if (pas_y == 70) {
            if (wantexit()) DefaultExit();
            else { pas_y--; Silent_Scroll(); }
        }
    }

    if (mapname == "Menace" || mapname == "MENACE") {
        if (on(14, 27)) Find_Gold(138, 1000);
        if (on(6, 39)) Find_Gold(139, 1500);
        if (on(42, 36)) Find_Gold(140, 2000);
        if (on(44, 41)) Find_Gold(141, 500);
        if (on(27, 6)) {
            Clear();
            Print(7, " 당신은 여기서 밀랍으로 봉인된 작은 상자를");
            Print(7, "발견하였다. 조심해서 열어보니 예언서 한 권");
            Print(7, "이 그 속에 들어 있었다.");
            Print(7, " 당신은 호기심에 그 책을 펼쳤다.");
            PressAnyKey();
            Print(14, "INDEX :");
            Print(7, "");
            Print(15, "  Chapter 1. 지식의 성전");
            Print(15, "  Chapter 2. 다크메이지의 탄생");
            Print(15, "  Chapter 3. 제작자의 차원");
            Print(15, "  Chapter 4. 위선자의 최후");
            Print(15, "  Chapter 5. 이 예언의 결말");
            PressAnyKey();
            Print(15, "Chapter 1.");
            Print(7, "");
            cPrint(7, 11, " 약 150년 뒤, 지식의 성전의 북쪽 해안에 ", "또", "");
            cPrint(11, 7, "다른 지식의 성전", "이라는 피라미드가 생긴다.", "");
            Print(7, "그곳에서는 이 세계의 진실된 의미를 발견 할");
            Print(7, "수 있게 될 것이며  '또 다른 지식의 성전'이");
            Print(7, "라는 뜻을 이해 할 수 있게 할 것이다.");
            PressAnyKey();
            Print(7, " 당신이 다음 장을 넘겼지만 Chapter 2.의 내");
            Print(7, "용은 누군가에 의하여 찢겨져 나갔다.");
            PressAnyKey();
            Print(15, "Chapter 3.");
            Print(7, "");
            Print(7, " 이곳 3차원에서 4차원의 축으로 조금만 이동");
            Print(7, "하면  전혀 다른 새로운 3차원의 세계가 펼쳐");
            Print(7, "진다.  대부분은 텅 빈 공간 속이거나 생명체");
            Print(7, "가 없는 거대한 땅덩이가 있기도 하지만 때때");
            Print(7, "로 생명체가 있는 곳일 수도 있다.  그 중 하");
            cPrint(7, 11, "나는 자신의 별을 스스로 ", "지구", "라고 부르는 종");
            Print(7, "족이 사는 곳이 있다. 그곳에는 이 공간의 창");
            Print(7, "조자인 안영기님이  지금도 열심히 게임 시나");
            Print(7, "리오와 시스템을 구상하고 있을 것이다.");
            PressAnyKey();
            Print(15, "Chapter 4.");
            Print(7, "");
            Print(7, " 795년 위선자는 사라지고  로어 세계는 진정");
            Print(7, "한 평화의 길로 접어든다.  이후의 세계는 안");
            Print(7, "영기님이 창조하지 않아도  우리 스스로가 미");
            Print(7, "래를 개척해 나가게 되고,  안영기님 역시 로");
            Print(7, "어 세계에 대한 일들을 우리에게 모두 위임하");
            Print(7, "고 더 이상 로어 역사에 개입하지 않게 된다.");
            PressAnyKey();
            Print(15, "Chapter 5.");
            Print(7, "");
            Print(7, " 795년 이후의 미래는 더 이상 창조되지 않는");
            Print(7, "다. 그때부터는 우리 스스로가 개척해 나가는");
            Print(7, "미래가 있을 뿐이다.  그리고  나의 예언서도");
            Print(7, "이것으로 끝을 맺게 된다.");
            PressAnyKey();
            Print(7, " 그리고 마지막 장에 이렇게 써 있었다.");
            Print(7, "");
            Print(15, " 이 책은 내가 죽은 다음 날");
            Print(15, "              세상에 나타 날 것이다.");
        }
        if (pas_y == 45) {
            if (getbit(0)) {
                if (wantexit()) DefaultExit();
                else { pas_y--; Scroll(true); }
            } else {
                Clear();
                Print(7, " 당신이 광산을 나가려 했을때  당신은  당신");
                Print(7, "앞에 벌어져 있는 광경을 보고 섬뜩함을 느꼈");
                Print(7, "다. 조금 전에 당신에게 일당을 주었던 그 광");
                Print(7, "산업자가 창에 찔린 채로 쓰러져 있었다.  당");
                Print(7, "신은 그 자에게로 다가갔다.  그는 창으로 심");
                Print(7, "장을 관통 당한채 쓰러져 있었다. 나는 그 창");
                Print(7, "이 낯에 익어서 그 창을 뽑아 쥐었다.  그 창");
                Print(7, "은 바로 내가 사냥용으로 쓰던 나의 창이었다.");
                PressAnyKey();
                party.etc[1] = 10;
                Silent_Scroll();
                int tile_i, tile_j;
                for (int k = 11; k >= 7; k--) {
                    if ((k + 1) <= 11) {
                        PutTile(5 + 25 - pas_x, k + 1, tile_i); PutTile(6 + 25 - pas_x, k + 1, tile_j);
                    }
                    tile_i = Real_Map(25, k + 39); tile_j = Real_Map(26, k + 39);
                    PutSprite(5 + 25 - pas_x, k, 13); PutSprite(6 + 25 - pas_x, k, 13);
                    Delay(500);
                }
                Print(7, " 갑자기 나타난 로어성의 병사들은 당신의 창");
                Print(7, "을 빼앗아 들고 말했다.");
                Print(7, "");
                Print(9, " 우리는 당신을 살인 혐의로 체포합니다.  그");
                Print(9, "리고 이 창은 증거물로 압수 하겠습니다.  변");
                Print(9, "명은 법정에서 하십시요.");
                PressAnyKey();
                Delete_Scroll_Screen();
                mapname = "Lore";
                Load();
                pas_x = 51; pas_y = 31;
                bool aux = SoundOn; SoundOn = false; Scroll(false); SoundOn = aux;
                int b = 1; if (player[1].classtype == 1) b += 8;
                for (int k = 11; k >= 6; k--) {
                    if ((k + 1) <= 11) {
                        PutTile(4, k + 1, tile_i); PutTile(5, k + 1, 42); PutTile(6, k + 1, tile_j);
                    }
                    tile_i = Real_Map(50, k + 25); tile_j = Real_Map(52, k + 25);
                    PutSprite(4, k, 13); PutSprite(5, k, b); PutSprite(6, k, 13);
                    Delay(700);
                }
                Print(7, " 로드안이 병사에게 말했다.");
                Print(7, "");
                Print(10, " 이 자는 누구인데 여기로 끌고 왔소?");
                PressAnyKey();
                Print(9, " 이 자는 여기 이 창으로 광산업자를 찔러 죽");
                Print(9, "였습니다.  우리 둘이 공교롭게도 이 자가 그");
                Print(9, "를 찔렀던 이 창을 뽑아내는 것을 목격하고는");
                Print(9, "즉시 체포 했습니다. 우리들의 경해로는 광산");
                Print(9, "업자와의 금전 관계로 인해  저질러진 살인이");
                Print(9, "라고 보아집니다.");
                Print(9, " 그리고 이 창을 증거물로 제시하겠습니다.");
                for (int k = 5; k >= 4; k--) {
                    if ((k + 1) <= 6) PutTile(4, k + 1, tile_i);
                    tile_i = Real_Map(50, k + 25);
                    PutSprite(4, k, 13);
                    Delay(1000);
                }
                Delay(1000);
                for (int k = 5; k <= 6; k++) {
                    if ((k - 1) >= 4) PutTile(4, k - 1, tile_i);
                    tile_i = Real_Map(50, k + 25);
                    PutSprite(4, k, 13);
                    Delay(1000);
                }
                PressAnyKey();
                Print(7, " 로드안이 말했다.");
                Print(7, "");
                Print(10, " 당신은 이곳의 법률을 알고 있겠지요?  범죄");
                Print(10, "없는 평화로운 세상을 만들기 위해  살인자는");
                Print(10, "모두 사형 또는 종신형에 처한다는 것도 알고");
                Print(10, "있을거요. 당신은 전과가 없기 때문에 사형을");
                Print(10, "선고하지는 않겠소.  그렇지만 최대한 자비를");
                Print(10, "베풀어도 종신형은 면치 못할 꺼요.");
                PressAnyKey();
                Delete_Scroll_Screen();
                pas_x = 39; pas_y = 10;
                Silent_Scroll();
                Delay(2000);
                tile_i = Real_Map(pas_x + 2, pas_y);
                tile_j = Real_Map(pas_x + 1, pas_y);
                PutMap(pas_x + 2, pas_y, tile_j);
                PutMap(pas_x + 1, pas_y, tile_i);
                Silent_Scroll();
                Print(9, " 나의 동료가 한 명 생겼군.  나는 1년 반 전");
                cPrint(9, 11, "에 절도 상해 혐의로  종신형을 선고 받은 ", "카", "");
                cPrint(11, 9, "노푸스", "라고 하네.  우리는 같이 평생을 살 동", "");
                Print(9, "료이니 잘 지내도록 하지.");
                PressAnyKey();
                Delete_Scroll_Screen();
                Delay(2000);
                PutMap(pas_x + 2, pas_y, tile_i);
                PutMap(pas_x + 1, pas_y, tile_j);
                party.year = 674; party.day = 18; party.hour = 8; party.min = 0; party.sec = 0;
                Silent_Scroll();
                Print(7, " 때는 674년 1월 19일.  당신은 여기서 약 두");
                Print(7, "달을 지냈다.");
            }
        }
    }

    if (mapname == "Valiant" || mapname == "VALIANT") {
        if (pas_y == 70) {
            if (wantexit()) DefaultExit();
            else { pas_y--; Silent_Scroll(); }
        }
    }

    if (mapname == "Ground2" || mapname == "GROUND2") {
        if (on(30, 47) && !getbit(51) && !getbit(50)) {
            Clear();
            Print(7, " 당신은 길을 가던 중  매우 지친듯이 보이는");
            Print(7, "한 여행자를 만났다. 그는 내가 베스퍼성으로");
            Print(7, "간다는 말을 듣더니 이렇게 말했다.");
            PressAnyKey();
            Print(7, " 저는 지금 아프로디테 테라에서 오는 길입니");
            Print(7, "다. 거기서 우연히 트롤족의 계보를 보았는데");
            Print(7, "그 종족은 왕권 세습제이며 왕권 중심으로 사");
            Print(7, "회가 구성되어 있습니다. 트롤족의 왕은 그리");
            Print(7, "강하지 않지만 그의 부하들 중에는 꽤 강한자");
            Print(7, "가 있습니다.");
            setbit(51);
        }
        if (on(90, 80)) {
            Clear();
            if (getbit(8) && !getbit(7)) {
                Print(7, " 거기에는  오크의 글로 적혀진  비석이 있었");
                Print(7, "다.  그리고 당신은  그 글을 읽어 내려갔다.");
                Print(7, "거기에 적힌 주문을 외자 갑자기 당신의 머리");
                Print(7, "가 혼란스러워졌고 곧 정신을 잃었다.");
                setbit(7);
                Delay(2000);
                Delete_Scroll_Screen();
                party.day += 1; party.hour = 23; party.min = 0; party.sec = 0;
                pas_x = 82; pas_y = 52;
                Silent_Scroll();
            } else if (getbit(7)) {
                Print(7, " 거기에는 오크의 글로 쓰여진 비석이 여전히");
                Print(7, "있었다.");
            } else {
                Print(7, " 거기에는 비석이 하나  서 있었지만  도저히");
                Print(7, "무슨 글인지 알아 볼 수가 없었다.");
            }
        }
        if (on(44, 7)) {
            Clear();
            if (getbit(3)) {
                if (!getbit(203) && party.etc[20] < 4) {
                    Print(7, " 당신이 아프로디테 게이트 안으로 막들어 가");
                    Print(7, "려고할 때, 허공에서 낯익은 목소리가 당신을");
                    Print(7, "불렀다. 그 목소리의 주인공은 바로 로드안이");
                    Print(7, "었다");
                    PressAnyKey();
                    Print(10, " 드디어 이곳까지 왔군요. 내가 한 가지 부탁");
                    Print(10, "을 더 해도 되겠소?  이미 베스퍼성은 쑥밭이");
                    Print(10, "되어서  사실상 원정은  더 이상 필요치 않게");
                    Print(10, "되었소. 그래서 부탁하건데 우리의 원흉인 트");
                    Print(10, "롤족을  완전히 멸해 주시오.  그들의 마을은");
                    Print(10, "베스퍼성 북동쪽 산악지역에 있소. 당신의 승");
                    Print(10, "리를 기원하리라.");
                    Print(11, "[ 경험치 + 10000 ]");
                    for (int i = 1; i <= 6; i++) {
                        if (exist(i)) player[i].experience += 10000;
                    }
                    setbit(203);
                    PressAnyKey();
                }
                Print(7, " 당신이 이 곳에 서자  새로운 풍경이 나타나");
                Print(7, "기 시작했다.");
                mapname = "Ground3";
                pas_x = 13; pas_y = 12;
                Load();
                OverLap(300, true);
            }
        }
    }

    if (mapname == "Gaea" || mapname == "GAEA") {
        if (on(18, 27)) Find_Gold(142, 5000);
        if (on(16, 23)) Find_Gold(143, 10000);
        if (on(15, 25)) Find_Gold(144, (rand() % 9 + 1) * 1000);
        if (on(12, 26)) Find_Gold(145, (rand() % 9 + 1) * 1000);
        if (on(10, 24) && !getbit(146)) {
            if (party.etc[21] < 3) Find_Gold(146, (rand() % 9 + 1) * 1000);
            else {
                Clear();
                cPrint(7, 11, "당신은 ", "화염의 크리스탈", "을 발견했다.");
                party.crystal[1]++;
            }
            setbit(146);
        }
        if (on(23, 25) && party.etc[21] == 3) { pas_x = 20; Scroll(true); }
        if (on(21, 25) && party.etc[21] == 3) { pas_x = 24; Scroll(true); }
        if (pas_y == 10 && !(getbit(3) && getbit(4))) {
            pas_y++; Clear(); Print(13, "원인을 알 수 없는 힘이 당신을 거부했다."); Silent_Scroll();
        }
        if (pas_y == 6) {
            if (!(getbit(3) && getbit(4))) {
                pas_y++; Clear(); Print(7, "게이트는 전혀 작동하지 않았다."); Silent_Scroll();
            } else {
                Screen_Fade_Out(300);
                absolute_set_xy = true;
                mapname = "Ground4";
                pas_x = 48; pas_y = 36;
                Load();
                absolute_set_xy = false;
                Silent_Scroll();
            }
        }
        if (pas_y == 45) {
            if (wantexit()) DefaultExit();
            else { pas_y--; Silent_Scroll(); }
        }
    }

    if (mapname == "OrcTown" || mapname == "ORCTOWN") {
        if (pas_y == 19 && !getbit(55)) {
            Clear();
            enemynumber = 8;
            for (int i = 1; i <= 3; i++) joinenemy(i, 19);
            for (int i = 4; i <= 7; i++) joinenemy(i, 20);
            joinenemy(8, 22);
            DisplayEnemies(true);
            Print(7, " 당신 앞에는  어느새인가 매직유저들이 길을");
            Print(7, "막고 서 있었다.");
            if (getbit(7)) {
                Print(7, " 그 중에서 마법사 차림을 제대로 한 오크 하");
                Print(7, "나가 당신에게 말을했다.");
                Print(7, "");
                Print(13, " 나는 오크킹님을 위해 죽고 사는 아키메이지");
                Print(13, "라고 한다. 나를 쓰러뜨리기 전에는 오크킹님");
                Print(13, "을 만날 수가 없다.");
            }
            PressAnyKey();
            BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            PutMap(23, 18, 43);
            setbit(55);
            Silent_Scroll();
        }
        if (pas_x >= 24 && pas_x <= 27 && pas_y >= 16 && pas_y <= 18 && !getbit(56)) {
            Clear();
            enemynumber = 6;
            for (int i = 1; i <= 2; i++) joinenemy(i, 21);
            joinenemy(3, 23);
            for (int i = 4; i <= 5; i++) joinenemy(i, 20);
            joinenemy(6, 24);
            DisplayEnemies(true);
            Print(7, " 당신이 오크킹에게 접근하자  모든 전투사들");
            Print(7, "이 오크킹 주위를 막아섰다.");
            if (getbit(7)) {
                Print(7, " 그러자 오크킹이 당신에게 말했다.");
                Print(7, "");
                Print(13, " 당신이  왜 나를  죽이려 하는지 모르겠지만");
                Print(13, "결과는 뻔한 일이다.");
                Print(13, " 자! 한바탕 붙어보자!!");
            }
            PressAnyKey();
            BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); pas_x = 25; pas_y = 21; return; }
            PutMap(25, 17, 43); PutMap(26, 17, 43);
            setbit(56);
            setbit(3);
            for (int i = 53; i <= 64; i++) setbit(i);
            party.etc[21] = 2;
            for (int i = 1; i <= 6; i++) {
                if (exist(i) && player[i].dead == 0)
                    player[i].experience += 15000;
            }
            Silent_Scroll();
            Clear();
            Print(14, " 당신은 오크족의 보스인 오크킹을 물리쳤다.");
            Print(11, "[ 경험치 + 15000 ]");
        }
        if (pas_y == 45) {
            if (wantexit()) DefaultExit();
            else { pas_y--; Silent_Scroll(); }
        }
    }

    if (mapname == "Ground3" || mapname == "GROUND3") {
        if (on(13, 11)) {
            Clear();
            Print(7, " 당신이 이 곳에 서자  새로운 풍경이 나타나");
            Print(7, "기 시작했다.");
            mapname = "Ground2";
            pas_x = 44; pas_y = 8;
            Load();
            OverLap(300, true);
        }
    }

    if (mapname == "Vesper" || mapname == "VESPER") {
        if (pas_x == 5 || pas_x == 71 || pas_y == 6 || pas_y == 70) {
            if (wantexit()) {
                absolute_set_xy = true;
                mapname = "Ground3";
                pas_x = Map_Header.exitX; pas_y = Map_Header.exitY;
                pas_x += pas_x1; pas_y += pas_y1;
                Load();
                Scroll(true);
                absolute_set_xy = false;
            } else {
                pas_x -= pas_x1; pas_y -= pas_y1;
                Silent_Scroll();
            }
        }
    }

    if (mapname == "TrolTown" || mapname == "TROLTOWN") {
        if (on(6, 36) || on(45, 29)) {
            Clear();
            Print(7, " 당신이 아래에 있는 벽돌을 밟자  벽돌은 약");
            Print(7, "간 아래로 꺼졌다.  그리고 몇 초후에 둔탁한");
            Print(7, "기계음이 들렸다.");
            for (int i = 25; i <= 26; i++) {
                if (on(6, 36)) {
                    for (int j = 24; j <= 25; j++) PutMap(i, j, 44);
                } else {
                    for (int j = 22; j <= 23; j++) PutMap(i, j, 44);
                }
            }
        }
        if (pas_x >= 25 && pas_x <= 26 && pas_y >= 26 && pas_y <= 30) {
            if (party.etc[4] == 0) {
                Clear();
                Print(7, "일행은 계곡으로 떨어져 버렸다.");
                PressAnyKey();
                party.etc[6] = 255;
                GameOver();
                return;
            }
        }
        if (on(13, 19)) {
            Clear();
            Print(7, " 당신이 이곳에 서자 갑자기 부비트랩이 작동");
            Print(7, "하며 거대한 폭발이 일어났고, 곧이어 의식이");
            Print(7, "흐려졌다");
            PressAnyKey();
            party.etc[6] = 255;
            GameOver();
            return;
        }
        if (on(12, 17) && !getbit(130)) {
            Clear();
            cPrint(7, 11, "당신은 ", "에너지 크리스탈", "을 발견했다.");
            party.crystal[7]++; setbit(130);
        }
        if (on(14, 17) && !getbit(131)) {
            Clear();
            cPrint(7, 11, "당신은 ", "한파의 크리스탈", "을 발견했다.");
            party.crystal[2]++; setbit(131);
        }
        if (on(12, 21) && !getbit(132)) { Clear(); Find_Gold(132, 15000); setbit(132); }
        if (on(14, 21) && !getbit(133)) {
            Clear();
            Print(7, "당신은 식량 200개를 발견했다.");
            if (party.food + 200 < 256) party.food += 200; else party.food = 255;
            setbit(133);
        }
        if (pas_y == 70) {
            if (wantexit()) DefaultExit(); else { pas_y--; Silent_Scroll(); }
        }
    }

    if (mapname == "Ground4" || mapname == "GROUND4") {
        if (on(48, 35)) {
            Screen_Fade_Out(300); DefaultExit();
        }
    }

    if (mapname == "Ancient" || mapname == "ANCIENT") {
        if (on(10, 13) || on(11, 13)) {
            Clear();
            Print(7, " 당신은  무덤 앞으로 발을  내 디디려 했지만");
            Print(7, "어떤 힘에 의해 더 이상 들어 갈 수가 없었다.");
            pas_y++; Silent_Scroll();
        }
        if (on(10, 12) || on(11, 12)) {
            Clear();
            Print(7, ""); Print(7, "");
            Print(15, "         Rest In Peace ----------");
            Print(7, "");
            Print(10, "              에인션트 이블");
            Print(7, "");
            Print(15, "        348 년 가을, 여기에 잠들다");
            if (!getbit(206)) {
                PressAnyKey();
                Print(11, " [ 경험치 + 500000 ]");
                for (int i = 1; i <= 6; i++) {
                    if (exist(i)) player[i].experience += 500000;
                }
                setbit(206);
            }
        }
        if (pas_x == 5 || pas_x == 16 || pas_y == 6 || pas_y == 25) {
            if (wantexit()) {
                absolute_set_xy = true;
                mapname = "Ground4";
                pas_x = Map_Header.exitX; pas_y = Map_Header.exitY;
                pas_x += pas_x1; pas_y += pas_y1;
                Load(); Scroll(true);
                absolute_set_xy = false;
            } else {
                pas_x -= pas_x1; pas_y -= pas_y1; Silent_Scroll();
            }
        }
    }

    if (mapname == "Hut" || mapname == "HUT") {
        if (on(9, 13) && !getbit(204)) {
            Clear();
            Print(7, " 당신은 앞에 있는 창을 가졌다");
            Print(11, " [ 랜서 + 1 ]");
            if (party.BackPack.data[3][6] + 1 < 255) party.BackPack.data[3][6]++;
            setbit(204);
            PutMap(8, 13, 35);
            Silent_Scroll();
        }
        if (on(9, 14) && !getbit(205)) {
            Clear();
            Print(7, " 당신은 앞에 있는 갑옷을 가졌다");
            Print(11, " [ 라멜라 + 1 ]");
            if (party.BackPack.data[6][7] + 1 < 255) party.BackPack.data[6][7]++;
            setbit(205);
            PutMap(8, 14, 35);
            Silent_Scroll();
        }
        if (pas_x == 5 || pas_x == 26 || pas_y == 6 || pas_y == 25) {
            if (wantexit()) {
                absolute_set_xy = true;
                mapname = "Ground4";
                pas_x = Map_Header.exitX; pas_y = Map_Header.exitY;
                pas_x += pas_x1; pas_y += pas_y1;
                Load(); Scroll(true);
                absolute_set_xy = false;
            } else {
                pas_x -= pas_x1; pas_y -= pas_y1; Silent_Scroll();
            }
        }
    }

    if (mapname == "Ground5" || mapname == "GROUND5") {
        Clear();
        Print(7, " 당신은 잠을 자고있는 어떤 은둔자를 보았다.");
        Print(7, "그는 당신의 기척에 잠을 깨었는지  눈을 뜨고");
        Print(7, "당신을 바라 보았다. 그리고는 말을 시작했다.");
        Print(7, "");
        if (getbit(14)) {
            if (getbit(13)) {
                Print(9, " 이제 당신은 드라코니안의 말을 들을 수 있을");
                Print(9, "것입니다.");
            } else {
                Print(9, " 나에게는 당신이  뭘 원하는지 훤히 보이는군");
                Print(9, "요. 당신은 지금 드라코니안의 말을 배우려 하");
                Print(9, "고있죠?");
                if (party.etc[5] > 0) {
                    Print(9, " 그럼 가르쳐 드리죠.  이건 제가 발명한 자동");
                    Print(9, "통역기인데 귀속에다 넣을면  드라코니안의 말");
                    Print(9, "을 들을 수 있을 것입니다.");
                    setbit(13);
                } else {
                    Print(9, " 하지만 귀찮아서 가르쳐 주고 싶지 않군요.");
                }
            }
        } else {
            Print(9, " 혹시 당신 지금 드라코니안의 글을 배우려 하");
            Print(9, "지 않습니까? 그렇다면 이 대륙 전체를 뒤져서");
            Print(9, "금빛의 인어를 찾아 보십시오.");
        }
    }

    if (mapname == "DracTown" || mapname == "DRACTOWN") {
        if (pas_x == 5) {
            if (wantexit()) {
                DefaultExit();
                if (getbit(6)) PutMap(15, 31, 53);
            } else { pas_x++; Silent_Scroll(); }
        }
        if (on(65, 101) && !getbit(29)) {
            Clear();
            Print(7, " 갑자기 호수 밑에서 누군가가 당신의 발을 잡");
            Print(7, "고 호수 밑으로 끌어 내리려 했다.  하지만 당");
            Print(7, "신의 물 위를 걷는 마법때문에 당신은 전혀 끌");
            Print(7, "려 들어가지 않았다. 도리어 당신이 정체 불명");
            Print(7, "의 손을 끌어 올려 물 밖으로 들어 올렸다. 놀");
            Print(7, "랍게도 당신이 물 밖으로 끌어 올린 것은 금빛");
            Print(7, "의 인어였다. 그녀는 겁에 질려 당신에게 애원");
            Print(7, "했다.");
            PressAnyKey();
            Print(9, " 제발 살려주세요. 그냥 장난친것 뿐이예요.");
            Print(9, " 만약 살려 주시면  당신에게  드라코니안족의");
            Print(9, "글을 가르쳐 드릴께요.");
            setbit(29);
            m[0] = "";
            m[1] = "인어를 놓아준다";
            m[2] = "죽여 버린다";
            if (Select(160, 2, 2, false, true) != 2) {
                Print(9, " 그 인어는 당신에게 금빛의 가루를 뿌렸다.");
                Print(9, " 그러자 당신은 약간의 어지러움을 느꼈다.");
                setbit(14);
            } else {
                Print(7, " 당신은 그 인어를 죽여 버렸다.");
            }
        }
        if (on(76, 73) && !getbit(104)) {
            setbit(104); Clear(); Print(11, " [ 화살 + 400 ]");
            if (party.arrow + 400 < 60000) party.arrow += 400;
            PutMap(77, 73, 44); Silent_Scroll();
        }
        if (on(68, 66) && !getbit(105)) {
            Clear();
            Print(7, " 당신이 보물상자를 열자  갑자기 흡혈귀가 튀");
            Print(7, "어나왔다.");
            PressAnyKey();
            enemynumber = 8;
            for (int i = 1; i <= 8; i++) joinenemy(i, 4);
            DisplayEnemies(true);
            BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            setbit(105); PutMap(67, 66, 44); Silent_Scroll();
        }
        if (on(68, 73) && !getbit(106)) {
            setbit(106); Clear(); Print(11, " [ 식량 + 255 ]");
            party.food = 255; PutMap(67, 73, 44); Silent_Scroll();
        }
        if (on(63, 71) && !getbit(107)) {
            setbit(107); Clear(); Print(11, " [ 황금 + 200000 ]");
            party.gold += 200000; PutMap(62, 71, 44); Silent_Scroll();
        }
        if (on(68, 88) && !getbit(108)) {
            setbit(108); Clear(); Print(11, " [ 소환의 크리스탈 + 1 ]");
            party.crystal[6]++; PutMap(67, 88, 44); Silent_Scroll();
        }
        if (on(63, 79) && !getbit(109)) {
            Clear();
            Print(7, " 당신이  보물상자를 열자  갑자기 드라큐라가");
            Print(7, "튀어나왔다.");
            PressAnyKey();
            enemynumber = 5;
            for (int i = 1; i <= 5; i++) joinenemy(i, 14);
            DisplayEnemies(true);
            BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            setbit(109); PutMap(62, 79, 44); Silent_Scroll();
        }
        if (on(64, 64) && !getbit(110)) {
            setbit(110); Clear(); Print(11, " [ 한파의 크리스탈 + 1 ]");
            party.crystal[2]++; PutMap(65, 64, 44); Silent_Scroll();
        }
        if (on(97, 100)) {
            Clear();
            cPrint(7, 15, " 묘비에는 ", "제 21대 드라콘 제왕의 묘", "라고 쓰여");
            Print(7, "있었다.");
        }
        if (on(89, 103)) {
            Clear();
            cPrint(7, 15, " 묘비에는 ", "제 17대 드라콘 제왕의 지하 묘", "라고");
            Print(7, "쓰여 있었고  바로 밑에는 기묘하게 생긴 바닥");
            Print(7, "이 있었다.");
            if (getbit(30)) return;
            PressAnyKey();
            Print(7, " 당신이 바닥을 밀어내자  바닥이 스르르 밀려");
            Print(7, "나며 좁은 지하계단이 나타났다.  계단의 입구");
            Print(7, "에는 스산한 기운이 맴돌고 있었고  자세히 다");
            Print(7, "시 보니 뚜렸하게 이런 글자가 새겨져 있었다.");
            Print(12, "'이곳으로 발을 디딘 자는 드라콘 제왕의 저주");
            Print(12, "를 받게 될 것이다.'");
            m[0] = "";
            m[1] = "지하계단으로 내려간다";
            m[2] = "다시 바닥을 원위치 시킨다";
            if (Select(200, 2, 2, false, true) != 1) {
                Print(7, " 당신이 바닥을 끌어 당기자 다시 스르르 밀려");
                Print(7, "와서 원위치 되었다.");
                return;
            }
            int k = 0;
            for (int i = 5; i >= 2; i--) { if (exist(i)) k = i; }
            if (k > 0) {
                std::string s = ReturnBJosa(player[k].name);
                if (s == "가") s = "";
                s = std::string(player[k].name) + s;
                Print(7, " 당신이 일행들을 데리고 지하계단으로 내려가");
                Print(7, "려 하자 당신을 저지시키는 사람이 있었다.");
                Print(7, " 그는 바로 " + s + "였다.");
                Print(7, " 그는 말을 했다.");
                PressAnyKey();
                Print(9, " 우리들의 목적은 따로 있는데 일부러 이런 모");
                Print(9, "험을 할 필요가 없지 않은가?  우리들은  모두");
                Print(9, "여기에 있겠네.  게다가 계단에 쓰여 있는  이");
                Print(9, "글도 마음에 걸린다네. 정말 가고 싶다면 자네");
                Print(9, "혼자서 가게나. 우리는 여기서 기다릴테니.");
                m[0] = "";
                m[1] = "혼자라도 지하무덤으로 간다";
                m[2] = "대원들의 의견대로 그만 둔다";
                if (Select(200, 2, 2, false, true) == 2) {
                    Print(7, " 당신은 다시 물러서서 바닥을 원위치 시켰다.");
                    return;
                }
            }
            setbit(30);
            for (int i = 2; i <= 5; i++) {
                // UTF-8 안전 백업: name_hidden에 현재 이름 복사 후 이름 숨김
                player[i].name_hidden_len = player[i].name_len;
                std::memcpy(player[i].name_hidden, player[i].name, player[i].name_len);
                if (player[i].name_hidden_len < 17) player[i].name_hidden[player[i].name_hidden_len] = '\0';
                player[i].name_len = 0;
                player[i].name[0] = '\0';
            }
            Display_Condition();
            DefaultEnter();
        }
    }

    if (mapname == "Tomb" || mapname == "TOMB") {
        if (on(83, 10)) PutMap(39, 31, 44);
        if (on(19, 35) && !getbit(124)) {
            setbit(124); Clear(); Print(11, " [ 플래티움 갑옷 + 1 ]");
            if (party.BackPack.data[6][10] < 255) party.BackPack.data[6][10]++;
        }
        if (on(84, 15) && !getbit(125)) { setbit(125); Clear(); Print(11, " [ 황금 + 100000 ]"); party.gold += 100000; }
        if (on(87, 15) && !getbit(126)) { setbit(126); Clear(); Print(11, " [ 황금 + 50000 ]"); party.gold += 50000; }
        if (on(84, 26) && !getbit(127)) {
            Clear();
            Print(7, " 당신이  보물상자를 열자  갑자기 저승사자가");
            Print(7, "튀어나왔다.");
            PressAnyKey();
            enemynumber = 1; joinenemy(1, 1);
            enemy[1].level = 30; enemy[1].hp = enemy[1].endurance * enemy[1].level * 10;
            DisplayEnemies(true); BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            setbit(127);
        }
        if (on(87, 26) && !getbit(128)) {
            Clear();
            Print(7, " 당신이  보물상자를 열자  갑자기 켈베로스가");
            Print(7, "튀어나왔다.");
            PressAnyKey();
            enemynumber = 5;
            for (int i = 1; i <= 5; i++) {
                joinenemy(i, 12);
                enemy[i].level = 15;
                enemy[i].hp = enemy[i].endurance * enemy[i].level * 10;
            }
            DisplayEnemies(true); BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            setbit(128);
        }
        if (on(85, 21) && !getbit(129)) {
            Clear();
            Print(7, " 당신이 괸 앞에 섰을때 무언가 묘한 분위기가");
            Print(7, "당신의 전신을 스치고 지나갔다.  잠시후 관의");
            Print(7, "뚜껑이 스르르 열리기 시작했다.  당신은 무의");
            Print(7, "식적으로 한 걸음 뒤로 물러났다. 곧이어 관속");
            Print(7, "에서는  에테르화된 드라코니안의 형체가 일어");
            Print(7, "났다. 그리고 말을 하기 시작했다.");
            PressAnyKey();
            enemynumber = 1; joinenemy(1, 58);
            enemy[1].name = "17대 드라콘제왕";
            enemy[1].specialcastlevel |= 0x80;
            DisplayEnemies(true);
            Print(13, " 100년만에 도굴꾼이 또 이곳에 나타났군.  너");
            Print(13, "도 역시 내가 가지고 있는  영혼의 크리스탈을");
            Print(13, "훔칠 목적으로 여기에 온 것임에 틀림없다. 하");
            Print(13, "지만 너도 별 수없이  이 앞의 해골과 같은 운");
            Print(13, "명이 될것이다.");
            PressAnyKey();
            do {
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) Clear();
            } while (party.etc[6] != 0);
            Clear();
            Print(11, " [ 영혼의 크리스탈 + 1 ]");
            setbit(129); party.crystal[5]++;
        }
    }

    if (mapname == "Imperium" || mapname == "IMPERIUM") {
        if (pas_y == 45) {
            if (wantexit()) { DefaultExit(); PutMap(pas_x, pas_y - 1, 53); }
            else { pas_y--; Silent_Scroll(); }
        }
        if (on(28, 17)) {
            Clear(); SetColor(14);
            int idx_x = g_HWinX1, idx_y = g_HWinY1;
            cHPrint(idx_x, idx_y, " You  ask  Young-Kie  how to play this game"); idx_y += 16;
            cHPrint(idx_x, idx_y, "easilly. Young-Kie's smile makes you uneasy"); idx_y += 16;
            cHPrint(idx_x, idx_y, ", as uneasy as  staring down the sources of"); idx_y += 16;
            cHPrint(idx_x, idx_y, "game programs. \" It's really very simple, \""); idx_y += 16;
            cHPrint(idx_x, idx_y, "he  laughs. \" This game's  secret  code  is"); idx_y += 16;
            cHPrint(idx_x, idx_y, "ascii #197. \"");
        }
        if (on(25, 24)) {
            if (!getbit(31)) {
                Clear();
                Print(7, " 드디어 " + FromPas(player[1].name, player[1].name_len) + "님이 여기에 당도 하셨군요.");
                Print(7, " 저의 소개를 하자면 바로 그 에인션트 이블입");
                Print(7, "니다. 저의 육체는 벌써 오래전에 사라지고 없");
                Print(7, "습니다. 저는 로드안과는 달리 그저 평범한 인");
                Print(7, "간이었으니까요.  지금은  나의 의지만이 남아");
                Print(7, "반신반인의 경지에서  이렇게 이곳에 존재하고");
                Print(7, "있습니다. 그럼 본론으로 들어가겠습니다.");
                Print(7, " 저는 예전에 로드안에게, 선을 위해서라면 나");
                Print(7, "의 이름을 이용하여  악을 비난해도 좋다고 약");
                Print(7, "속했었습니다.  그래서 로어 세계에서  어떠한");
                Print(7, "안좋은 일이 생기더라도  저의 소행이었노라고");
                Print(7, "치부해버리며  로드안은 자신의 지위를 유지할");
                Print(7, "수 있었습니다.  하지만 로드안은 해가 갈수록");
                Print(7, "자신의 위치를 지키기 위해서  저의 이름을 이");
                Print(7, "용한다는 느낌이 와닿기 시작했습니다. 원래의");
                Print(7, "그는 전혀 그럴 사람이 아니었는데  너무나 큰");
                Print(7, "명성과 지위에 올라있다보니  로어성의 성주라");
                Print(7, "는 직책이 그를 자신만을 생각하는 현실주의자");
                Print(7, "로 바꾸어 버린 것입니다.  선의 대표자로서의");
                Print(7, "그는 항상 주위를 환기 시킬만한  선행을 찾기");
                Print(7, "시작했고  결국은 남에게 보이기 위한 선을 행");
                Print(7, "하기 시작했습니다.  그리고  급기야 이번같은");
                Print(7, "큰 실수를  저지르고야 말았습니다.  로드안은");
                Print(7, "인간을 너무 생각하는 마음에서 인간의 영역을");
                Print(7, "늘려주기 원했고  그 결과는 타종족에 대한 침");
                Print(7, "범으로 이어졌습니다.  이건 분명 로드안의 잘");
                Print(7, "못된 행동이며 거의 돌이킬 수 없는 결과를 낳");
                Print(7, "고야 말았습니다.  지금의 그는 권력과 명예의");
                Print(7, "노예가 되어  광적으로 이 일을 강행하고 있습");
                Print(7, "니다. 그리고 이 일이 다 끝나더라도 더 큰 명");
                Print(7, "예를 얻기위해 선행을 빙자한 다른 일을 또 꾸");
                Print(7, "밀 것입니다.");
                Print(7, " 만약 저의 말이 맞다는게 확인되면 부디 저의");
                Print(7, "편에 서서 저를 도와주십시요. 저는 당신과 같");
                Print(7, "은 분의 도움이 크게 필요합니다.");
                Print(7, "");
                PressAnyKey();
                setbit(31);
            }
            Clear();
            Print(7, " 지금 당신을 로어성 앞으로 공간이동 시켜 드");
            Print(7, "리겠습니다.  거기서 저와 로드안의 진위를 확");
            Print(7, "인해 주시기 바랍니다.");
            m[0] = "";
            m[1] = "공간이동을 한다";
            m[2] = "공간이동을 하지않는다";
            if (Select(180, 2, 2, false, true) == 1) {
                party.etc[20] = 7;
                DefaultEnter();
            }
        }
    }
}


// ---------------------------------------------------------------------------
// 맵 전용 이벤트 분기 2 (퍼즐 및 던전 심층부)
// ---------------------------------------------------------------------------
static void SpecialEvent_Sub2() {
    int x0, y0, x2, y2;
    static bool ebony = false;
    bool AllRight = true;

    Scroll(true);

    if (mapname == "Kobold" || mapname == "KOBOLD") {
        if (pas_x == 5) {
            if (wantexit()) {
                if (getbit(86)) {
                    Clear();
                    Print(7, " 당신이 문을 나서려 하자  도망쳤던 코볼트킹");
                    Print(7, "이 당신 앞을 가로 막았다.");
                    Print(7, "");
                    Print(13, " 나는 너를 내 손으로 처치하지 않고는 편안히");
                    Print(13, "죽을 수가 없었다. 그래서 나는 영원한 생명을");
                    Print(13, "가지기를 원했고  그 댓가로 악마에게 혼을 팔");
                    Print(13, "았다. 나는 결코 죽지않고 너를 끝까지 괴롭힐");
                    Print(13, "것이다.");
                    PressAnyKey();
                    do {
                        Clear();
                        enemynumber = 1; joinenemy(1, 45);
                        DisplayEnemies(true);
                        BattleMode(false, true);
                        if (party.etc[6] == 254) return;
                    } while (party.etc[6] == 2);
                }
                DefaultExit();
                if (getbit(86)) {
                    Clear();
                    Print(7, " 당신이 성 밖으로 탈출하자  허공에서 로드안");
                    Print(7, "의 음성이 들려왔다.");
                    Print(7, "");
                    Print(10, " 당신은 빨리 이곳을 나가시오.  나의 선의 봉");
                    Print(10, "인으로  악마의 힘을 빌린 코볼트킹을 막겠소.");
                    Print(10, "아마 그는 다시는 이 성을 벗어나지 못할거요.");
                    Print(7, "");
                    Print(7, " 말이채 끝나기도 전에 코볼트성 주위에는  구");
                    Print(7, "형의 결계가 형성 되었고  곧이어 기의 파문이");
                    Print(7, "일렁이기 시작했다.");
                    setbit(5);
                }
            } else {
                pas_x++; Silent_Scroll();
            }
        }
        if (on(8, 30) && Real_Map(9, 30) != 53) {
            PutMap(9, 30, 53); Silent_Scroll(); Clear();
            Print(7, " 갑자기 땅에서 푯말이 솟아 나왔다.");
        }
        if (pas_x >= 30 && pas_x <= 55 && pas_y >= 21 && pas_y <= 36) {
            if (on(49, 31) && !getbit(17)) {
                Clear(); enemynumber = 7;
                for (int i = 1; i <= 5; i++) joinenemy(i, 36);
                for (int i = 6; i <= 7; i++) joinenemy(i, 39);
                DisplayEnemies(true); BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { Silent_Scroll(); return; }
                Clear();
                Print(7, " 당신은 쓰러진  코볼트 근위기사의 몸을 뒤져");
                Print(7, "에메랄드키를 찾아냈다.");
                setbit(17);
            } else {
                for (int j = pas_y - 1; j <= pas_y + 1; j++)
                    for (int i = pas_x - 1; i <= pas_x + 1; i++) PutMap(i, j, 50);
                Silent_Scroll();
            }
        }
        if (on(38, 13)) {
            PutMap(99, 21, 43); Clear();
            Print(7, " 당신이 바닥의 기묘한 벽돌을 밟자 돌이 약간"); Print(7, "밑으로 들어갔다.");
        }
        if (on(121, 36)) {
            PutMap(110, 26, 43); Clear();
            Print(7, " 당신이 바닥의 기묘한 벽돌을 밟자 돌이 약간"); Print(7, "밑으로 들어갔다.");
        }
        if (on(112, 29)) {
            PutMap(96, 21, 43); Clear();
            Print(7, " 당신이 바닥의 기묘한 벽돌을 밟자 돌이 약간"); Print(7, "밑으로 들어갔다.");
        }
        if (on(96, 36)) {
            PutMap(107, 30, 43); Clear();
            Print(7, " 당신이 바닥의 기묘한 벽돌을 밟자 돌이 약간"); Print(7, "밑으로 들어갔다.");
        }
        if (on(105, 27)) {
            PutMap(118, 35, 43); Clear();
            Print(7, " 당신이 바닥의 기묘한 벽돌을 밟자 돌이 약간"); Print(7, "밑으로 들어갔다.");
        }
        if (on(100, 36) && !getbit(18)) {
            Clear(); enemynumber = 8;
            for (int i = 1; i <= 7; i++) joinenemy(i, 36); joinenemy(8, 39);
            DisplayEnemies(true); BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            Clear(); Print(7, " 당신은 벽에 걸려 있는 황금 키를 가졌다."); setbit(18);
        }
        if (on(32, 90) && !getbit(19)) {
            Clear(); enemynumber = 8;
            for (int i = 1; i <= 7; i++) joinenemy(i, 36); joinenemy(8, 37);
            DisplayEnemies(true); BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            Clear();
            Print(7, " 당신은 코볼트 매직유저의 몸을 뒤져서  백금");
            Print(7, "키를 발견했다.");
            setbit(19);
        }
        if (pas_x >= 96 && pas_x <= 119 && pas_y >= 85 && pas_y <= 98) {
            if (rand() % 2 == 0) { x2 = pas_x1; y2 = pas_y1; }
            else {
                do { x2 = (rand() % 3) - 1; y2 = (rand() % 3) - 1; } while (x2 == 0 && y2 == 0);
            }
            x0 = pas_x + x2; y0 = pas_y + y2;
            int rm = Real_Map(x0, y0);
            if (rm == 43) { PutMap(pas_x, pas_y, 43); PutMap(x0, y0, 0); }
            else { x0 = pas_x - 2 * x2; y0 = pas_y - 2 * y2; PutMap(pas_x, pas_y, 43); PutMap(x0, y0, 0); }
            int rrm = Real_Map(pas_x + pas_x1, pas_y + pas_y1);
            if (rrm == 37 || rrm == 38) { pas_x += pas_x1 * 2; pas_y += pas_y1 * 2; Silent_Scroll(); }
        }
        if (on(95, 91) && !getbit(20)) {
            Clear(); enemynumber = 8;
            for (int i = 1; i <= 6; i++) joinenemy(i, 36);
            for (int i = 7; i <= 8; i++) joinenemy(i, 37);
            DisplayEnemies(true); BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            Clear(); Print(7, " 당신은 벽에 걸린 사파이어 키를 가졌다."); setbit(20);
        }

        if (pas_x == 27 && getbit(18)) {
            PutMap(28, 60, 43); PutMap(28, 61, 43);
            PutMap(29, 60, 0); PutMap(29, 61, 0);
            Silent_Scroll();
        }
        if (pas_x == 124 && getbit(19) && getbit(89)) {
            PutMap(123, 60, 43); PutMap(123, 61, 43);
            PutMap(122, 60, 0); PutMap(122, 61, 0);
            Silent_Scroll();
        }
        if (pas_x == 62 && getbit(20)) {
            for (int j = 60; j <= 61; j++)
                for (int i = 63; i <= 64; i++) PutMap(i, j, 43);
            PutMap(65, 60, 0); PutMap(65, 61, 0);
            Silent_Scroll();
        }
        if (pas_x == 27 && getbit(17)) {
            for (int j = 60; j <= 61; j++)
                for (int i = 87; i <= 88; i++) PutMap(i, j, 43);
            PutMap(86, 60, 0); PutMap(86, 61, 0);
            Silent_Scroll();
        }

        if (pas_x == 122 && !getbit(92)) {
            Clear(); enemynumber = 8;
            for (int i = 1; i <= 6; i++) joinenemy(i, 39);
            for (int i = 7; i <= 8; i++) joinenemy(i, 40);
            DisplayEnemies(true);
            Print(13, " 정말 끈질긴 인간들이군. 번번히 패하면서 또");
            Print(13, "시비를 걸어 오다니...");
            PressAnyKey(); BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { pas_x++; Silent_Scroll(); return; }
            setbit(92);
        }
        if (pas_x == 29 && !getbit(91)) {
            Clear(); enemynumber = 8;
            for (int i = 1; i <= 4; i++) joinenemy(i, 39);
            for (int i = 5; i <= 7; i++) joinenemy(i, 37);
            joinenemy(8, 41);
            DisplayEnemies(true);
            Print(13, " 또 인간들이 우리 성을 침범하러 왔군.  너희");
            Print(13, "들은 이 주위의 해골들이 보이지 않는가? 이것");
            Print(13, "들은 우리에게 무모한 싸움을 걸었다가 죽어간");
            Print(13, "사람들의 해골이다.");
            PressAnyKey(); BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { pas_x--; Silent_Scroll(); return; }
            setbit(91);
        }
        if (pas_x == 86 && !getbit(90)) {
            Clear(); enemynumber = 8;
            for (int i = 1; i <= 7; i++) joinenemy(i, 37); joinenemy(8, 38);
            DisplayEnemies(true);
            Print(13, " 나는 이 성의 경비대장이다.  나의 임무는 바");
            Print(13, "로 너희들을  이 안으로  들여 놓지 않는 것이");
            Print(13, "다. 나의 목숨을 바쳐서라도...");
            PressAnyKey(); BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { pas_x++; Silent_Scroll(); return; }
            setbit(90);
        }
        if (pas_x == 86 && getbit(90)) {
            for (int j = 60; j <= 61; j++)
                for (int i = 63; i <= 65; i++) PutMap(i, j, 43);
            for (int j = 60; j <= 61; j++)
                for (int i = 28; i <= 29; i++) PutMap(i, j, 43);
        }
        if (pas_x == 65 && !getbit(89)) {
            Clear(); enemynumber = 4;
            for (int i = 1; i <= 3; i++) joinenemy(i, 39); joinenemy(4, 42);
            DisplayEnemies(true);
            Print(13, " 용케도 우리 성의 내부까지 들어왔군. 당신들");
            Print(13, "은 소환 마법이란걸 들어봤나? 지금 바로 내가");
            Print(13, "소환 마법의 진가를 보여주지.");
            PressAnyKey(); BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { pas_x--; Silent_Scroll(); return; }
            setbit(89);
        }

        if (on(48, 73) && Real_Map(46, 47) == 50) {
            for (int j = 42; j <= 50; j++)
                for (int i = 37; i <= 54; i++)
                    if (Real_Map(i, j) == 50) PutMap(i, j, 49);
        }
        if (on(45, 46) && Real_Map(56, 63) == 50) {
            for (int j = 51; j <= 74; j++)
                for (int i = 56; i <= 60; i++)
                    if (Real_Map(i, j) == 50) PutMap(i, j, 48);
        }
        if (pas_x >= 42 && pas_x <= 53 && pas_y >= 55 && pas_y <= 66) {
            if (party.etc[36] <= 3) {
                party.etc[36]++;
                PutMap(pas_x, pas_y, 43);
                if (party.etc[36] == 4) {
                    Clear();
                    Print(7, " 당신은 땅에 반쯤 묻혀있는  오팔키를 발견했"); Print(7, "다.");
                    setbit(21);
                } else PutMap((rand() % 12) + 42, (rand() % 12) + 55, 0);
            }
        }

        if (pas_x >= 91 && pas_x <= 121 && pas_y >= 41 && pas_y <= 80) {
            if (on(91, 41) && !getbit(22)) {
                Clear();
                Print(7, " 당신은 어둠속에서도  반짝이는 물체를  하나");
                Print(7, "보았다. 자세히보니 그것은 흑요석키였고 즉시");
                Print(7, "그것을 가졌다.");
                setbit(22);
            } else if ((on(91, 59) || on(121, 62)) && ebony) {
                ebony = false; Silent_Scroll();
            } else if (on(93, 60)) { pas_x = 91; pas_y = 60; ebony = false; Silent_Scroll(); }
            else if (on(119, 61)) { pas_x = 121; pas_y = 61; Silent_Scroll(); }
            else if (on(103, 75)) { pas_x = 107; pas_y = 60; ebony = false; Silent_Scroll(); }
            else if (on(108, 75)) { pas_x = 102; pas_y = 49; Silent_Scroll(); }
            else if (on(105, 58) && Real_Map(105, 59) != 43) { PutMap(105, 59, 43); }
            else if (!ebony) { ebony = true; Silent_Scroll(); }
        }

        if (on(58, 101) && Real_Map(58, 100) != 49) {
            for (int j = 100; j >= 86; j--) PutMap(58, j, 49);
            PutMap(58, 85, 0); Silent_Scroll(); Clear();
            Print(7, " 당신이 무심코 벽에 팔을 기대자 벽은 안으로");
            Print(7, "쑥 들어가 버렸다. 그리고는 새로운 길이 나타");
            Print(7, "났다.");
        }
        if (on(58, 85)) {
            PutMap(58, 85, 43); PutMap(76, 74, 52); Clear();
            Print(7, " 당신이 발 밑의 벽돌을 밟자  벽돌은  유압에");
            Print(7, "의해 서서히 가라앉기 시작했다.  당신은 즉시");
            Print(7, "벽돌에서 발을 떼고 한 걸음 물러섰다. 하지만");
            Print(7, "벽돌은 빨려 들어가듯이 밑으로 가라 앉았다.");
            int k = 0; for (int i = 5; i >= 2; i--) if (exist(i)) k = i;
            if (k > 0) {
                std::string s = std::string(player[k].name) + ReturnBJosa(player[k].name);
                Print(7, " 그때 옆에 있던 " + s + " 말했다."); Print(7, "");
                Print(9, " 이런 장치는 전에 한번 본 적이 있다네. 벽돌");
                Print(9, "이 가라 앉는 무게와 압력으로 다른 곳의 물체");
                Print(9, "를 떠올리는 장치라네. 그런데 어디가 다시 떠");
                Print(9, "올랐는지 모르겠군.");
            }
        }
        if (on(76, 74)) {
            PutMap(65, 25, 35); PutMap(64, 25, 0); PutMap(76, 74, 48); Clear();
            Print(7, " 당신이 물 위로 떠오른 땅을 밟자  다시 가라"); Print(7, "앉아 버렸다.");
            Silent_Scroll();
        }
        if (on(64, 25) && Real_Map(108, 53) != 0) {
            for (int j = 53; j <= 54; j++)
                for (int i = 108; i <= 109; i++) PutMap(i, j, 0);
            PutMap(64, 25, 43);
            Print(7, " 성의 남동쪽에서  돌연히 커다란 진동음과 함"); Print(7, "께 기계 소리가 들렸다.");
        }
        if (on(106, 54) && !getbit(200)) {
            Print(7, " 당신은 벽에서 무언가 반짝이는 것이 있어 벽");
            Print(7, "으로 다가갔다.  가까이에서 보니 그것은 수정");
            Print(7, "이었다.  하지만 그 수정은 특이하게도 스스로");
            Print(7, "희미한 빛을 발하고 있었다.  당신이 수정속을");
            Print(7, "들여다 보았을때  소스라치게 놀랄 수 밖에 없");
            Print(7, "었다.  그속에는 형체를 분간하기 힘들 정도의");
            Print(7, "많은 영혼들이 배회하고 있었다.  그들이 아마");
            Print(7, "이 희미한 빛을 발하는 것 같았다.  당신이 약");
            Print(7, "간의 힘을 주어 빼어 보니 쉽게 벽에서 떨어져");
            Print(7, "나왔다.");
            int k = 0; for (int i = 1; i <= 5; i++) if (exist(i)) k = i;
            if (k > 0) {
                std::string s = std::string(player[k].name) + ReturnBJosa(player[k].name);
                Print(7, " 그걸 보더니 " + s + " 말했다."); Print(7, "");
                Print(9, " 아니, 이건....  이건 바로 그 전설 속에서나");
                cPrint(9, 11, "논해지던 ", "영혼의 크리스탈", "임에 틀림없네. 이것");
                Print(9, "은 전세계에 두개 밖에 존재하지 않는 귀한 것");
                Print(9, "인데 우리가 발견했군."); Print(7, ""); PressAnyKey();
            }
            setbit(200); party.crystal[5]++;
        }

        if (on(75, 60) && getbit(21) && Real_Map(75, 61) != 43) { PutMap(75, 61, 43); Silent_Scroll(); }
        if (on(76, 61) && getbit(22) && Real_Map(77, 61) != 43) { PutMap(77, 61, 43); Silent_Scroll(); }
        if (on(77, 59) && !getbit(88)) {
            enemynumber = 8;
            for (int i = 1; i <= 3; i++) joinenemy(i, 39);
            for (int i = 4; i <= 5; i++) joinenemy(i, 40);
            for (int i = 6; i <= 7; i++) joinenemy(i, 41);
            joinenemy(8, 43);
            DisplayEnemies(true); Clear();
            Print(13, " 신성한 신의 제단에 더러운 발을 디딘 놈들은");
            Print(13, "누구냐?  신을 모독한 너희네 인간들에게 몸소");
            Print(13, "가르침을 주겠다. 밧아랏!!");
            PressAnyKey(); BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { pas_y++; Silent_Scroll(); return; }
            Clear();
            Print(7, " 당신은 대제사장으로부터 다이아몬드키 두 개"); Print(7, "를 빼았았다.");
            setbit(88);
        }

        if (pas_y == 39 && getbit(88) && ((party.etc[30] >> 4) == (pas_x - 63) / 4)) {
            if (Real_Map(pas_x, pas_y - 1) != 43) {
                for (int j = pas_y - 2; j <= pas_y - 1; j++)
                    for (int i = pas_x - 1; i <= pas_x; i++) PutMap(i, j, 43);
                Silent_Scroll();
            }
        }
        if (pas_y == 82 && getbit(88) && ((party.etc[30] & 0x0F) == (pas_x - 64) / 4)) {
            if (Real_Map(pas_x, pas_y + 1) != 43) {
                for (int j = pas_y + 1; j <= pas_y + 2; j++)
                    for (int i = pas_x; i <= pas_x + 1; i++) PutMap(i, j, 43);
                Silent_Scroll();
            }
        }

        const int BIT1 = 1, BIT2 = 2, BIT3 = 4, BIT4 = 8, BIT5 = 16;
        
        // Chest 1
        if (on(68, 25)) {
            if (getbit(147)) goto skip_chest1;
            if ((party.etc[31] & BIT1) > 0) {
                if (getbit(24)) {
                    if (getbit(134)) goto skip_chest1;
                battle1:
                    enemynumber = 1; joinenemy(1, 46); Clear();
                    Print(7, " 갑자기 보물상자의 양 옆에서 팔이 나오고 밑");
                    Print(7, "에서 다리가 나오더니  괴물로 변해 당신을 공");
                    Print(7, "격하기 시작했다.  당신은  그것이 보물상자의");
                    Print(7, "모습을 하고 있는  미미크라는 것을 금방 알아");
                    Print(7, "차렸다.");
                    PressAnyKey(); DisplayEnemies(true); BattleMode(false, true);
                    if (party.etc[6] == 254) return;
                    if (party.etc[6] == 2) { pas_y++; Silent_Scroll(); return; }
                    PutMap(69, 25, 43); Silent_Scroll(); setbit(134);
                } else if (getbit(11)) {
                    Clear(); AllRight = true;
                    Print(7, " 당신이 보물상자 앞으로 다가섰을때  상자 위");
                    Print(7, "의 두개골이 당신에게 질문을 던졌다.");
                    PressAnyKey();
                    Print(15, " 1편 <또 다른 지식의 성전>과 2편 <다크 메이");
                    Print(15, "지 실리안 카미너스>에  모두 출현했던 인물이");
                    Print(15, "아닌 것은 누구인가?");
                    m[0] = ""; m[1] = "로드안"; m[2] = "네크로만서"; m[3] = "레드 안타레스"; m[4] = "알비레오";
                    AllRight = AllRight && Random_Order(4);
                    Print(15, " 이 게임의 1,2,3 편에 모두 출현한 인물은 누");
                    Print(15, "구인가?");
                    m[0] = ""; m[1] = "실리안 카미너스"; m[2] = "안타레스 Jr."; m[3] = "드라코니안"; m[4] = "에인션트 이블";
                    AllRight = AllRight && Random_Order(4);
                    Print(15, " 이 게임에서 1,3 편만 출현한 인물이 아닌 것");
                    Print(15, "은 누구인가?");
                    m[0] = ""; m[1] = "카노푸스"; m[2] = "안타레스 Jr."; m[3] = "드라코니안"; m[4] = "제작자(안영기)";
                    AllRight = AllRight && Random_Order(4);
                    if (AllRight) {
                        Print(7, " 보물상자는 스스로 스르륵 열렸다."); Print(7, "");
                        Print(11, " [ 인내력 + 0 에서 + 4 ]");
                        for (int i = 1; i <= 6; i++) {
                            if (exist(i)) player[i].endurance += (player[i].luck / 5);
                        }
                        setbit(147);
                    } else { setbit(24); goto battle1; }
                } else {
                    Clear();
                    Print(7, " 당신이 보물상자 앞으로 다가섰을때  상자 위");
                    Print(7, "의 두개골이 무어라 말을 했지만 전혀 알아 들");
                    Print(7, "을 수가 없었다.");
                }
            } else {
                Clear();
                Print(7, " 당신이 보물상자 앞으로 다가섰지만  상자 위");
                Print(7, "의 두개골은 아무말도 하지 않았다.");
            }
        skip_chest1:;
        }
        
        // Chest 2
        if (on(72, 25)) {
            if (getbit(148)) goto skip_chest2;
            Clear();
            if ((party.etc[31] & BIT2) > 0) {
                if (getbit(25)) {
                    if (getbit(135)) goto skip_chest2;
                battle2:
                    enemynumber = 1; joinenemy(1, 46); Clear();
                    Print(7, " 갑자기 보물상자의 양 옆에서 팔이 나오고 밑");
                    Print(7, "에서 다리가 나오더니  괴물로 변해 당신을 공");
                    Print(7, "격하기 시작했다.  당신은  그것이 보물상자의");
                    Print(7, "모습을 하고 있는  미미크라는 것을 금방 알아");
                    Print(7, "차렸다.");
                    PressAnyKey(); DisplayEnemies(true); BattleMode(false, true);
                    if (party.etc[6] == 254) return;
                    if (party.etc[6] == 2) { pas_y++; Silent_Scroll(); return; }
                    PutMap(73, 25, 43); Silent_Scroll(); setbit(135);
                } else if (getbit(11)) {
                    AllRight = true;
                    Print(7, " 당신이 보물상자 앞으로 다가섰을때  상자 위");
                    Print(7, "의 두개골이 당신에게 질문을 던졌다.");
                    PressAnyKey();
                    int j_rand = (std::rand() % 100) + 1;
                    char s_buf[16]; sprintf(s_buf, "%2d", j_rand);
                    std::string s_str = s_buf;
                    Print(15, " 당신은  핵융합 마법을  종종 사용하곤 한다.");
                    Print(15, "당신이 공기 중에 분포 해 있는  수소원자 4개");
                    Print(15, "를 헬륨원자 1개로 핵융합 시켰을때  질량결손");
                    Print(15, "이 " + s_str + " mg 이라면 적에게 방출된 에너지는 얼마");
                    Print(15, "인가?");
                    char m_buf[32];
                    m[0] = "";
                    sprintf(m_buf, "%4.2f", (j_rand * 3) / 100.0f); m[1] = m_buf;
                    sprintf(m_buf, "%4.2f", (j_rand * 1) / 100.0f); m[2] = m_buf;
                    sprintf(m_buf, "%4.2f", (j_rand * 10) / 100.0f); m[3] = m_buf;
                    sprintf(m_buf, "%4.2f", (j_rand * 9) / 100.0f); m[4] = m_buf;
                    for (int i = 1; i <= 4; i++) m[i] += " x 10^15";
                    AllRight = AllRight && Random_Order(4);
                    Print(15, " 당신은 종종 공간이동을 하여  위치를 이동하");
                    Print(15, "기도 한다.  그리고 그때 당신은 3차원의 공간");
                    Print(15, "에  강한 중력을 걸어  공간을 압축 시킨후 그");
                    Print(15, "공간면을 뚫고  이동하고 나서  공간을 원위치");
                    Print(15, "시킴으로써(워프 방식) 그것을 가능하게 한다.");
                    Print(15, "이것은 무슨 법칙으로 증명될 수 있겠는가?");
                    m[0] = ""; m[1] = "질량보존의 법칙"; m[2] = "작용 반작용의 법칙"; m[3] = "특수 상대성 이론"; m[4] = "일반 상대성 이론";
                    AllRight = AllRight && Random_Order(4);
                    Print(15, " 당신은 '마법 화구'라는  불공을 만들어 적에");
                    Print(15, "게 던져 제압해본 경험이 있다. 당신은 당신의");
                    Print(15, "손 위에 불덩어리를 만들기 위해서  이 물질을");
                    Print(15, "결합시킨다. 이 물질의 이름은 무엇인가?");
                    m[0] = ""; m[1] = "Hydrogen"; m[2] = "Carbon"; m[3] = "Nitrogen"; m[4] = "Oxygen";
                    AllRight = AllRight && Random_Order(4);
                    if (AllRight) {
                        Print(7, " 보물상자는 스스로 스르륵 열렸다."); Print(7, "");
                        Print(11, " [ 소환의 크리스탈 + 1 ]");
                        party.crystal[6]++; setbit(148);
                    } else { setbit(25); goto battle2; }
                } else {
                    Print(7, " 당신이 보물상자 앞으로 다가섰을때  상자 위");
                    Print(7, "의 두개골이 무어라 말을 했지만 전혀 알아 들");
                    Print(7, "을 수가 없었다.");
                }
            } else {
                Print(7, " 당신이 보물상자 앞으로 다가섰지만  상자 위");
                Print(7, "의 두개골은 아무말도 하지 않았다.");
            }
        skip_chest2:;
        }

        // Chest 3
        if (on(76, 25)) {
            if (getbit(149)) goto skip_chest3;
            Clear();
            if ((party.etc[31] & BIT3) > 0) {
                if (getbit(26)) {
                    if (getbit(136)) goto skip_chest3;
                battle3:
                    enemynumber = 1; joinenemy(1, 46); Clear();
                    Print(7, " 갑자기 보물상자의 양 옆에서 팔이 나오고 밑");
                    Print(7, "에서 다리가 나오더니  괴물로 변해 당신을 공");
                    Print(7, "격하기 시작했다.  당신은  그것이 보물상자의");
                    Print(7, "모습을 하고 있는  미미크라는 것을 금방 알아");
                    Print(7, "차렸다.");
                    PressAnyKey(); DisplayEnemies(true); BattleMode(false, true);
                    if (party.etc[6] == 254) return;
                    if (party.etc[6] == 2) { pas_y++; Silent_Scroll(); return; }
                    PutMap(77, 25, 43); Silent_Scroll(); setbit(136);
                } else if (getbit(11)) {
                    AllRight = true;
                    Print(7, " 당신이 보물상자 앞으로 다가섰을때  상자 위");
                    Print(7, "의 두개골이 당신에게 질문을 던졌다.");
                    PressAnyKey();
                    m[0] = ""; m[1] = "              맞다"; m[2] = "             아니다";
                    Print(15, " 달과 지구의 나이는 같거나 지구가 더 많다.");
                    if (Select(180, 2, 2, false, true) != 2) AllRight = false;
                    Print(15, " 예전에는 화성과 목성 사이에 혹성이 하나 있"); Print(15, "었다.");
                    if (Select(180, 2, 2, false, true) != 1) AllRight = false;
                    Print(15, " 태양의 나이는 50 억살이다.  80 억년 전에도"); Print(15, "지금의 태양 자리에 항성이 있었을 것이다.");
                    if (Select(180, 2, 2, false, true) != 1) AllRight = false;
                    Print(15, " 우주의 크기는 변함이 없다.");
                    if (Select(180, 2, 2, false, true) != 2) AllRight = false;
                    Print(15, " 태양계 혹성의 순서는  수성, 금성, 지구,..."); Print(15, "천왕성, 해왕성, 명왕성의 순서로 일정하다.");
                    if (Select(180, 2, 2, false, true) != 2) AllRight = false;
                    Print(15, " 토성을 물에 띄우면 뜬다.");
                    if (Select(180, 2, 2, false, true) != 1) AllRight = false;
                    if (AllRight) {
                        Print(7, " 보물상자는 스스로 스르륵 열렸다."); Print(7, "");
                        Print(11, " [ 화염의 크리스탈 + 1 ]");
                        party.crystal[1]++; setbit(149);
                    } else { setbit(26); goto battle3; }
                } else {
                    Print(7, " 당신이 보물상자 앞으로 다가섰을때  상자 위");
                    Print(7, "의 두개골이 무어라 말을 했지만 전혀 알아 들");
                    Print(7, "을 수가 없었다.");
                }
            } else {
                Print(7, " 당신이 보물상자 앞으로 다가섰지만  상자 위");
                Print(7, "의 두개골은 아무말도 하지 않았다.");
            }
        skip_chest3:;
        }

        // Chest 4
        if (on(80, 25)) {
            if (getbit(150)) goto skip_chest4;
            Clear();
            if ((party.etc[31] & BIT4) > 0) {
                if (getbit(27)) {
                    if (getbit(137)) goto skip_chest4;
                battle4:
                    enemynumber = 1; joinenemy(1, 46); Clear();
                    Print(7, " 갑자기 보물상자의 양 옆에서 팔이 나오고 밑");
                    Print(7, "에서 다리가 나오더니  괴물로 변해 당신을 공");
                    Print(7, "격하기 시작했다.  당신은  그것이 보물상자의");
                    Print(7, "모습을 하고 있는  미미크라는 것을 금방 알아");
                    Print(7, "차렸다.");
                    PressAnyKey(); DisplayEnemies(true); BattleMode(false, true);
                    if (party.etc[6] == 254) return;
                    if (party.etc[6] == 2) { pas_y++; Silent_Scroll(); return; }
                    PutMap(81, 25, 43); Silent_Scroll(); setbit(137);
                } else if (getbit(11)) {
                    AllRight = true;
                    Print(7, " 당신이 보물상자 앞으로 다가섰을때  상자 위");
                    Print(7, "의 두개골이 당신에게 질문을 던졌다.");
                    PressAnyKey();
                    m[0] = ""; m[1] = "              맞다"; m[2] = "             아니다";
                    Print(15, " BASIC 언어의  ';'(세미콜론)은  문장의 끝을"); Print(15, "의미한다");
                    if (Select(180, 2, 2, false, true) != 2) AllRight = false;
                    Print(15, " Pascal에서 대입 기호는 '='이다.");
                    if (Select(180, 2, 2, false, true) != 2) AllRight = false;
                    Print(15, " C 언어는  영문 소문자를  기본으로 설계되었"); Print(15, "다.");
                    if (Select(180, 2, 2, false, true) != 1) AllRight = false;
                    Print(15, " FORTRAN은 subroutine 기능이 없다.");
                    if (Select(180, 2, 2, false, true) != 2) AllRight = false;
                    Print(15, " Small Talk, C++, Pascal은 현재 모두 객체지"); Print(15, "향 프로그래밍을 지원한다.");
                    if (Select(180, 2, 2, false, true) != 1) AllRight = false;
                    if (AllRight) {
                        Print(7, " 보물상자는 스스로 스르륵 열렸다."); Print(7, "");
                        Print(11, " [ 체력 또는 정신력 + 0 에서 + 2 ]");
                        for (int i = 1; i <= 6; i++) {
                            if (exist(i)) {
                                int j_stat = ((player[i].luck / 5) + 1) / 2;
                                if (player[i].classtype == 1) player[i].mentality += j_stat;
                                else player[i].strength += j_stat;
                            }
                        }
                        setbit(150);
                    } else { setbit(27); goto battle4; }
                } else {
                    Print(7, " 당신이 보물상자 앞으로 다가섰을때  상자 위");
                    Print(7, "의 두개골이 무어라 말을 했지만 전혀 알아 들");
                    Print(7, "을 수가 없었다.");
                }
            } else {
                Print(7, " 당신이 보물상자 앞으로 다가섰지만  상자 위");
                Print(7, "의 두개골은 아무말도 하지 않았다.");
            }
        skip_chest4:;
        }

        // Chest 5
        if (on(84, 25)) {
            if (!getbit(23)) {
                Clear();
                Print(7, " 당신이 앞에 있는 보물상자를 보았을때  섬찟");
                Print(7, "함을 느끼며 한 걸음 물러섰다.  그 상자 위에");
                Print(7, "는 인간의 해골이 앉아 있었고 그 해골에는 두");
                Print(7, "눈동자와 머리카락이 듬성듬성 붙어 있었다.");
                Print(7, " 스르르 해골이 고개를 들며 당신을 노려 보았");
                Print(7, "다. 당신은 다시 한 걸음을 물러섰다.  그러자");
                Print(7, "그 해골은 뼈뿐인 두 손을 들어 당신을 가리켰");
                Print(7, "다. 그리고 인간의 말을 하기 시작했다.");
                Print(7, "");
                Print(9, " 당신은 당신들이 지금하고 있는 일이 어떤 것");
                Print(9, "인지 모르고 있다.  지금 이 시대가  정의라고");
                Print(9, "생각하는 것을 역사가 반드시 정의라고 평가짓");
                Print(9, "지는 않을 것이다. 당신이 만약 에인션트 이블");
                Print(9, "을 만나지 않았다면 당장 그를 만나보아라. 그");
                Print(9, "를 통해 당신은 진정한 정의를 체험하게 될 것");
                Print(9, "이다. 게다가 너는 ...");
                Print(7, "");
                Print(7, " 그가  이런 말을 하고 있을때  갑자기 천정을");
                Print(7, "뚫고 직격뇌진이 그를 강타했다.  당신은 순간");
                Print(7, "적으로 벌어진 이 일을 보고  넋나간듯 얼마동");
                Print(7, "안 있다가 산산히 흩어진 뼈를 보며 방금의 말");
                Print(7, "을 되새겼다.");
                Print(7, "");
                PressAnyKey(); setbit(23);
            }
            if ((party.etc[31] & BIT5) > 0) {
                if (!getbit(151)) {
                    Clear();
                    Print(7, " 당신은 해골이 걸터 앉아 있던 보물상자를 열"); Print(7, "었다."); Print(7, "");
                    Print(11, " [ 크로매틱 방패 + 1 ]"); Print(11, " [ 브리간디 + 1 ]"); Print(11, " [ 철퇴 + 1 ]");
                    if (party.BackPack.data[2][6] < 255) party.BackPack.data[2][6]++;
                    if (party.BackPack.data[5][4] < 255) party.BackPack.data[5][4]++;
                    if (party.BackPack.data[6][5] < 255) party.BackPack.data[6][5]++;
                    setbit(151);
                }
            }
        }

        if ((on(76, 90) || on(77, 90)) && !getbit(87)) {
            Clear(); enemynumber = 5;
            for (int i = 1; i <= 2; i++) joinenemy(i, 41);
            joinenemy(3, 40); joinenemy(4, 42); joinenemy(5, 44);
            DisplayEnemies(true);
            Print(13, " 나는 코볼트킹님의 근위대의 대장이다.  내가");
            Print(13, "살아 있는한  너희들을 결코 이 안으로 한발자");
            Print(13, "국도 들여놓지 못할 것이다.");
            PressAnyKey(); BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { pas_y++; Silent_Scroll(); return; }
            setbit(87);
        }

        if ((on(76, 86) || on(77, 86)) && !getbit(86)) {
            Clear(); enemynumber = 1; joinenemy(1, 45);
            DisplayEnemies(true);
            Print(13, " 지상 최상 최대의 마도사인 나, 코볼트킹에게");
            Print(13, "도전해 오다니 전말 배짱 한번 좋구나. 네놈들");
            Print(13, "은 나 혼자서 상대하겠다. 가소로운 것들...");
            PressAnyKey();
            do {
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) {
                    Clear();
                    Print(7, " 당신은 코볼트킹에게서 도망치려 했지만 그가");
                    Print(7, "만든 결계를 벗어 날 수가 없었다.");
                    PressAnyKey(); DisplayEnemies(true);
                }
            } while (party.etc[6] == 2);
            Clear();
            Print(7, " 당신이  코볼트킹을 물리쳤구나라고 생각했을");
            Print(7, "때 코볼트킹은 공간이동의 마법을 써서 어디론");
            Print(7, "가 사라져 버렸다. 당신은 당황해서 이곳 전지");
            Print(7, "역을 투시해 보았지만 그는 없었다.");
            setbit(86);
        }
    }

    if (mapname == "HdsGate" || mapname == "HDSGATE") {
        int i_tile, j_tile;
        if (on(25, 30)) {
            Clear();
            if (Real_Map(20, 27) == 40) { i_tile = 40; j_tile = 41; Print(7, " 당신은 레버를 왼쪽으로 당겼다."); }
            else { i_tile = 41; j_tile = 40; Print(7, " 당신은 레버를 오른쪽으로 당겼다."); }
            PutMap(20, 27, j_tile); PutMap(22, 28, j_tile); PutMap(21, 29, i_tile);
            PutMap(23, 31, i_tile); PutMap(15, 32, i_tile); PutMap(16, 33, j_tile);
            PutMap(119, 34, j_tile); PutMap(17, 35, i_tile); PutMap(24, 33, j_tile);
            PutMap(28, 33, i_tile); PutMap(25, 35, j_tile); PutMap(27, 35, i_tile);
            PutMap(22, 36, i_tile); PutMap(23, 39, i_tile);
            Silent_Scroll(); return;
        }
        if (on(23, 15)) {
            Clear();
            if (Real_Map(23, 28) == 40) { i_tile = 40; j_tile = 41; Print(7, " 당신은 레버를 왼쪽으로 당겼다."); }
            else { i_tile = 41; j_tile = 40; Print(7, " 당신은 레버를 오른쪽으로 당겼다."); }
            PutMap(21, 27, i_tile); PutMap(23, 28, j_tile); PutMap(20, 30, i_tile);
            PutMap(22, 30, j_tile); PutMap(19, 32, i_tile); PutMap(17, 33, i_tile);
            PutMap(18, 34, j_tile); PutMap(16, 35, j_tile); PutMap(26, 32, j_tile);
            PutMap(28, 32, i_tile); PutMap(25, 34, j_tile); PutMap(27, 34, j_tile);
            PutMap(23, 38, j_tile); PutMap(21, 39, j_tile);
            return;
        }
        if (on(23, 19)) {
            Clear();
            if (Real_Map(20, 29) == 40) { i_tile = 40; j_tile = 41; Print(7, " 당신은 레버를 왼쪽으로 당겼다."); }
            else { i_tile = 41; j_tile = 40; Print(7, " 당신은 레버를 오른쪽으로 당겼다."); }
            PutMap(23, 27, i_tile); PutMap(20, 29, j_tile); PutMap(22, 29, j_tile);
            PutMap(21, 31, i_tile); PutMap(16, 32, i_tile); PutMap(18, 33, i_tile);
            PutMap(16, 34, j_tile); PutMap(19, 35, i_tile); PutMap(25, 32, j_tile);
            PutMap(27, 33, j_tile); PutMap(24, 34, i_tile); PutMap(26, 34, i_tile);
            PutMap(22, 37, i_tile); PutMap(20, 38, i_tile);
            return;
        }
        if (on(27, 15)) {
            Clear();
            if (Real_Map(21, 28) == 40) { i_tile = 40; j_tile = 41; Print(7, " 당신은 레버를 왼쪽으로 당겼다."); }
            else { i_tile = 41; j_tile = 40; Print(7, " 당신은 레버를 오른쪽으로 당겼다."); }
            PutMap(22, 27, i_tile); PutMap(21, 28, j_tile); PutMap(23, 29, i_tile);
            PutMap(20, 31, j_tile); PutMap(18, 32, i_tile); PutMap(19, 33, j_tile);
            PutMap(17, 34, j_tile); PutMap(15, 35, i_tile); PutMap(24, 32, j_tile);
            PutMap(27, 32, j_tile); PutMap(26, 33, i_tile); PutMap(24, 35, j_tile);
            PutMap(28, 35, i_tile); PutMap(23, 37, i_tile); PutMap(21, 40, i_tile);
            return;
        }
        if (on(27, 19)) {
            Clear();
            if (Real_Map(20, 28) == 40) { i_tile = 40; j_tile = 41; Print(7, " 당신은 레버를 왼쪽으로 당겼다."); }
            else { i_tile = 41; j_tile = 40; Print(7, " 당신은 레버를 오른쪽으로 당겼다."); }
            PutMap(20, 28, j_tile); PutMap(21, 30, j_tile); PutMap(23, 30, j_tile);
            PutMap(22, 31, i_tile); PutMap(17, 32, i_tile); PutMap(15, 33, i_tile);
            PutMap(15, 34, j_tile); PutMap(18, 35, j_tile); PutMap(25, 31, i_tile);
            PutMap(28, 34, i_tile); PutMap(26, 35, j_tile); PutMap(21, 38, j_tile);
            PutMap(22, 39, j_tile);
            return;
        }
        // 물가로 들어갔는지 검사
        if (GetMap(pas_x, pas_y) == Real_Map(pas_x, pas_y)) {
            Clear();
            Print(12, " 당신이 물 속으로 발을 디디자  순식간에 온");
            Print(12, "몸이 타버렸다.");
            for (int i = 1; i <= 6; i++) {
                if (exist(i)) { player[i].hp = 0; player[i].dead = 1; }
            }
            Display_Condition(); PressAnyKey(); Clear();
            party.etc[6] = 255; GameOver(); return;
        }
    }

    if (mapname == "Dome" || mapname == "DOME") {
        if (pas_x == 5 || pas_x == 46 || pas_y == 6 || pas_y == 95) {
            if (wantexit()) {
                Print(9, " 당신이 발견한 이 장소는  분명 배리언트 피");
                Print(9, "플즈의 사람들을 로드안의 시각범위에 벗어나");
                Print(9, "게 하기에  충분한 곳일겁니다.  당신은 정말");
                Print(9, "훌륭한 일을 해내셨습니다.  제가  곧 당신을");
                Print(9, "지상으로 이동 시켜 드리겠습니다. 그리고 저");
                Print(9, "는  몇 년 정도 휴식을 취하려 합니다.  저의");
                Print(9, "영적 에너지를  마을을 건설하고 사람들을 이");
                Print(9, "동 시키는데 써버려서 에너지 충전 기간이 필");
                Print(9, "요합니다. 여태껏 당신의 도움들 정말 고마왔");
                Print(9, "습니다. 계속 자신이 결심한 의지대로 꿋꿋하");
                Print(9, "게 나아가십시요.  저는 이제 뒷일을 모두 당");
                Print(9, "신에게 맡기겠습니다.  당신이라면 분명히 해");
                Print(9, "낼거라 믿습니다. 그럼, 안녕히...");
                PressAnyKey();
                mapname = "Ground1"; Load();
                watch_year = party.year; watch_day = party.day;
                watch_hour = party.hour; watch_min = party.min;
                watch_sec = party.sec;
                Time_Watch = true; time_event_no = 3;
                watch_min += 90;
                if (watch_min > 59) {
                    watch_min -= 60; watch_hour += 1;
                    if (watch_hour > 23) {
                        watch_hour -= 24; watch_day += 1;
                        if (watch_day > 359) {
                            watch_day -= 360; watch_year += 1;
                        }
                    }
                }
                pas_x = 80; pas_y = 80; Scroll(true);
            } else {
                pas_x -= pas_x1; pas_y -= pas_y1; Silent_Scroll();
            }
        }
    }

    if (mapname == "Light" || mapname == "LIGHT") {
        if (pas_y == 70) {
            if (wantexit()) DefaultExit(); else { pas_y--; Silent_Scroll(); }
        }
        if (pas_y == 43 && !getbit(33)) {
            int i_tile = 38 - pas_x + 5;
            for (int j = 1; j <= 5; j++) {
                if (j > 1) PutTile(i_tile, j - 1, 42);
                PutTile(i_tile, j, 53);
                if (j < 5) Delay(500);
            }
            Clear();
            Print(7, "나는 이번 반란을 지휘하고 있는 카미너스라는");
            Print(7, "사람이오. 몇일 전에 우리 성에 에인션트 이블");
            Print(7, "의 의지가 다녀갔소. 그리고 이 세상의 진실을");
            Print(7, "어렴풋이 알게 되었소.  우리는 항상 로드안이");
            Print(7, "말하는 것을 그대로 믿어왔소. 그건 여태껏 알");
            Print(7, "려진 그의 명성 때문이었다오.  하지만 이제는");
            Print(7, "예외가 있다는걸 깨닭게 되었소.");
            Print(7, "당신은 그의 명령대로 오크, 트롤, 코볼트, 드");
            Print(7, "라코니안 이 네종족을 제거하여 인간을 이롭게");
            Print(7, "한건 사실이오. 당신 또한 그 일을 자랑스럽게");
            Print(7, "생각할지도 모르오.  하지만  우리의 입장에서");
            Print(7, "볼때 그렇다는 것이지, 절대로 다른 종족의 입");
            Print(7, "장에서는 당신이 한 일이 정의를 위한 것이 아");
            Print(7, "니라오.  실제로 잘못은 우리가 했으면서 도리");
            Print(7, "어 그들에게 잘못을 덮어 씌워서  그들을 해한");
            Print(7, "것이지않소. 당신이 에인션트 이블을 만났다는");
            Print(7, "말을 들었소. 그렇다면 우리가 지금 하려는 일");
            Print(7, "을 이해할거요.  우리들은 당신이 우리의 편에");
            Print(7, "서서 로드안을 응징해 주기를 바라고 있소.");
            Print(7, " 자, 선택은 당신에게 있소.");
            Print(7, ""); Print(7, "");
            Pause(false);
            for (int i = 1; i <= 7; i++) Print(7, "");
            Pause(true);
            m[0] = ""; m[1] = "로드안을 배신한다"; m[2] = "로드안의 말대로 이들을 벌한다";
            if (Select(200, 2, 2, false, true) == 1) {
                setbit(33); party.etc[25] = 2;
                Print(7, " 우리 성의 사람들은 당신의 용기 있는 결단에");
                Print(7, "찬사를 보내고 있소. 정말 장한 일이오.");
                watch_year = party.year; watch_day = party.day;
                watch_hour = party.hour; watch_min = party.min;
                watch_sec = party.sec;
                Time_Watch = true; time_event_no = 0;
                watch_min += 10;
                if (watch_min > 59) {
                    watch_min -= 60; watch_hour += 1;
                    if (watch_hour > 23) {
                        watch_hour -= 24; watch_day += 1;
                        if (watch_day > 359) {
                            watch_day -= 360; watch_year += 1;
                        }
                    }
                }
                for (int i = 2; i <= 5; i++) {
                    // Pascal 원본에도 있던 백업 로직 — UTF-8 안전 방식으로 구현
                    player[i].name_hidden_len = player[i].name_len;
                    std::memcpy(player[i].name_hidden, player[i].name, player[i].name_len);
                    if (player[i].name_hidden_len < 17) player[i].name_hidden[player[i].name_hidden_len] = '\0';
                    player[i].name_len = 0;
                    player[i].name[0] = '\0';
                }
                Display_Condition();
                i_tile = 38 - pas_x + 5;
                for (int j = 5; j >= 1; j--) {
                    PutTile(i_tile, j, 42);
                    if (j > 1) { PutTile(i_tile, j - 1, 53); Delay(500); }
                }
            } else {
                party.etc[25] = 1;
                enemynumber = 8; for (int i = 1; i <= 8; i++) joinenemy(i, 64);
                DisplayEnemies(true);
                do {
                    BattleMode(false, true);
                    if (party.etc[6] == 254) return;
                    if (party.etc[6] == 2) Clear();
                } while (party.etc[6] != 0);
                
                Clear(); enemynumber = 8;
                for (int i = 1; i <= 7; i++) joinenemy(i, 64); joinenemy(8, 65);
                DisplayEnemies(true);
                do {
                    BattleMode(false, true);
                    if (party.etc[6] == 254) return;
                    if (party.etc[6] == 2) Clear();
                } while (party.etc[6] != 0);
                
                Clear(); enemynumber = 8;
                for (int i = 1; i <= 4; i++) joinenemy(i, 64);
                for (int i = 5; i <= 7; i++) joinenemy(i, 65);
                joinenemy(8, 66); DisplayEnemies(true);
                do {
                    BattleMode(false, true);
                    if (party.etc[6] == 254) return;
                    if (party.etc[6] == 2) Clear();
                } while (party.etc[6] != 0);
                
                setbit(33); party.etc[20] = 9; Clear();
                Print(7, " 하지만  카미너스는  배리언트 피플즈의 남은");
                Print(7, "주민들을 데리고  성을 버린채  게이트를 통해");
                Print(7, "도망가 버렸다.");
            }
        }
    }

    if (mapname == "Dark" || mapname == "DARK") {
        if (pas_y == 95) {
            if (wantexit()) DefaultExit(); else { pas_y--; Silent_Scroll(); }
        }
    }
}

// ---------------------------------------------------------------------------
// 메인 이벤트 핸들러
// ---------------------------------------------------------------------------
void SpecialEvent() {
    SpecialEvent_Sub2();
    SpecialEvent_Sub1();
}