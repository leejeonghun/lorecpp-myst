#include "mysttal2.h"
#include "mystsub.h"
#include "uhanx.h"
#include "mystbatt.h"
#include "mystend.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>

static int sgn(int eq) {
    if (eq > 0) return 1;
    if (eq < 0) return -1;
    return 0;
}

void TalkMode_Sub0() {
    Clear();

    // Utility: Extract Player 1 name for dialogues
    std::string p1name = FromPas(player[1].name, player[1].name_len);

    if (mapname == "Lore" || mapname == "LORE") {

        if (at(91, 65) || at(94, 68) || at(87, 73)) { Grocery(); return; }
        if (at(8, 71) || at(14, 69) || at(14, 73)) { Weapon_Shop(); return; }
        if (at(86, 12) || at(87, 14)) { Hospital(); return; }
        if (at(16, 64)) { Sell_Item(); return; }
        if (at(21, 12) || at(25, 13)) { About_Class(); return; }
        if (at(12, 13)) { Sell_EXP(); return; }

        // Inn
        if (at(12, 49)) {
            Print(14, " 이 세계의 창시자는 안영기님이시며 그는 위대한 프로그래머입니다.");
        }
        if (at(15, 55)) {
            if (!getbit(35)) {
                Print(7, " 당신이 혹시  내 아들 카노푸스와 같은 감방");
                Print(7, "을 사용했던 " + p1name + ReturnBJosa(p1name) + " 아닌가요?");
                Print(7, " 역시 그렇군요. 당신이 베스퍼성으로 원정을");
                Print(7, "간다는 말을 들었습니다. 만약 여유가 있으시");
                Print(7, "다면  내 아들을 당신의 일행에 참가 시켜 주");
                Print(7, "실 수 있겠습니까? 지금은 비록 절도 죄로 잡");
                Print(7, "혀 있는 몸이지만  원래 내 아들의 꿈은 로어");
                Print(7, "성 최의 전사가 되는 것이었습니다.  아마 짐");
                Print(7, "은 되지 않을 것입니다. 제발 부탁합니다.");
            } else {
                Print(7, " " + p1name + "님. 제 아들을 잘 부탁합니다.");
            }
        }
        if (at(22, 50)) {
            cPrint(7, 11, " 나의 이름은 ", "알코르", "라고 하오.  나는 이곳의");
            Print(7, "검사로서 나의 길을 개척하려하오. 특히 당신");
            Print(7, "과 함께 베스퍼성의 원정을 하고 싶은데 당신");
            Print(7, "생각은 어떻소?");
            m[0] = "";
            m[1] = "그거 좋은 생각이군요";
            m[2] = "미안하지만 그런 좀 어렵군요";
            if (Select(150, 2, 2, false, true) == 1) {
                int k = 0;
                do { k++; } while (player[k].name_len != 0 && k != 6);
                
                if (k < 6) {
                    auto& p = player[k];
                    p.name_len = std::strlen("알코르");
                    std::strcpy((char*)p.name, "알코르");
                    p.sex = male;
                    p._class = 3;
                    p.classtype = sword;
                    p.level = 2;
                    p.strength = 15;
                    p.mentality = 5;
                    p.concentration = 6;
                    p.endurance = 12;
                    p.resistance = 15;
                    p.agility = 14;
                    p.accuracy = 16;
                    p.luck = 12;
                    p.poison = 0;
                    p.unconscious = 0;
                    p.dead = 0;
                    p.hp = p.endurance * p.level * 10;
                    p.sp = 0;
                    p.experience = 0;
                    p.potential_experience = ReturnEXP(p.level);
                    p.weapon = 3;
                    p.shield = 0;
                    p.armor = 1;
                    p.potential_ac = 2;
                    p.sword_data.sword_skill = 40;
                    p.sword_data.axe_skill = 0;
                    p.sword_data.spear_skill = 0;
                    p.sword_data.bow_skill = 0;
                    p.sword_data.shield_skill = 0;
                    p.sword_data.fist_skill = 10;
                    Modify_Item(k);
                    PutMap(22, 50, 44);
                    setbit(36);
                    Silent_Scroll();
                    Display_Condition();
                    Clear();
                } else {
                    Print(7, " 벌써 사람이 모두 채워져 있군요.  다음 기회");
                    Print(7, "를 기다리지요.");
                    return;
                }
            } else {
                asyouwish();
            }
        }
        if (at(22, 54)) {
            cPrint(7, 11, " 나는 이곳의 기사로 있는 ", "미자르", "라고하오.");
            Print(7, "당신의 임무에 동참한다면  큰 영광으로 여기");
            Print(7, "겠소.");
            m[0] = "";
            m[1] = "그럼 그렇게 하시오";
            m[2] = "하지만 그건 좀 곤란하오";
            if (Select(140, 2, 2, false, true) == 1) {
                int k = 0;
                do { k++; } while (player[k].name_len != 0 && k != 6);
                
                if (k < 6) {
                    auto& p = player[k];
                    p.name_len = std::strlen("미자르");
                    std::strcpy((char*)p.name, "미자르");
                    p.sex = male;
                    p._class = 2;
                    p.classtype = sword;
                    p.level = 2;
                    p.strength = 13;
                    p.mentality = 6;
                    p.concentration = 8;
                    p.endurance = 15;
                    p.resistance = 16;
                    p.agility = 16;
                    p.accuracy = 15;
                    p.luck = 14;
                    p.poison = 0;
                    p.unconscious = 0;
                    p.dead = 0;
                    p.hp = p.endurance * p.level * 10;
                    p.sp = 0;
                    p.experience = 0;
                    p.potential_experience = ReturnEXP(p.level);
                    p.weapon = 2;
                    p.shield = 1;
                    p.armor = 2;
                    p.potential_ac = 2;
                    p.sword_data.sword_skill = 20;
                    p.sword_data.axe_skill = 10;
                    p.sword_data.spear_skill = 5;
                    p.sword_data.bow_skill = 0;
                    p.sword_data.shield_skill = 10;
                    p.sword_data.fist_skill = 10;
                    Modify_Item(k);
                    PutMap(22, 54, 44);
                    setbit(37);
                    Silent_Scroll();
                    Display_Condition();
                    Clear();
                } else {
                    Print(7, " 벌써 사람이 모두 채워져 있군요.  다음 기회");
                    Print(7, "를 기다리지요.");
                    return;
                }
            } else {
                asyouwish();
            }
        }

        // Pub
        if (at(13, 27)) {
            std::string s = (player[1].sex == female) ? "여성" : "남성";
            Print(7, " 거기 " + s + "분, 어서 앉으시지요.");
        }
        if (at(18, 27)) {
            Print(7, " 우리 주점의 명물인 코리아 위스키 맛 좀 보시겠습니까?");
        }
        if (at(21, 33)) {
            Print(7, " 과음은 삼가하십시요.");
        }
        if (at(18, 38)) {
            Print(7, " 자네를 못 볼줄 알았는데 다시 보게 되었군.");
            Print(7, "자! 그 기념으로 한 잔 받게나. 완~~~샷 !!");
            if (party.etc[5] > 0) {
                Print(7, "");
                Print(7, " 당신의 모습을 보니  내가 가이아 테라의 수");
                Print(7, "석 위저드로 있을 때가  생각나는군.  자네를");
                Print(7, "도와주고픈 마음이 있는데 자네는 어떤가?");
                m[0] = "";
                m[1] = "역시 안타레스답군요";
                m[2] = "안됐지만 사양하겠습니다";
                if (Select(170, 2, 2, false, true) == 1) {
                    int k = 0;
                    do { k++; } while (player[k].name_len != 0 && k != 6);
                    
                    if (k < 6) {
                        auto& p = player[k];
                        p.name_len = std::strlen("안타레스 Jr.");
                        std::strcpy((char*)p.name, "안타레스 Jr.");
                        p.sex = male;
                        p._class = 4;
                        p.classtype = magic;
                        p.level = 15;
                        p.strength = 10;
                        p.mentality = 20;
                        p.concentration = 20;
                        p.endurance = 15;
                        p.resistance = 17;
                        p.agility = 19;
                        p.accuracy = 17;
                        p.luck = 18;
                        p.poison = 0;
                        p.unconscious = 0;
                        p.dead = 0;
                        p.hp = p.endurance * p.level * 10;
                        p.sp = 0;
                        p.experience = 0;
                        p.potential_experience = ReturnEXP(p.level);
                        p.weapon = 0;
                        p.shield = 0;
                        p.armor = 2;
                        p.potential_ac = 0;
                        p.sword_data.sword_skill = 60;
                        p.sword_data.axe_skill = 30;
                        p.sword_data.spear_skill = 30;
                        p.sword_data.bow_skill = 0;
                        p.sword_data.shield_skill = 0;
                        p.sword_data.fist_skill = 0;
                        Modify_Item(k);
                        PutMap(18, 38, 47);
                        setbit(38);
                        Silent_Scroll();
                        Display_Condition();
                        Clear();
                    } else {
                        Print(7, " 벌써 사람이 모두 채워져 있군.  다음 기회를");
                        Print(7, "기다리도록 하지.");
                        return;
                    }
                } else {
                    asyouwish();
                }
            }
        }
        if (at(13, 32)) Print(7, " 역시 술은 코리아 위스키(소주)가 최고야.");
        if (at(15, 35)) Print(7, " 음냐, 음냐, 쿨쿨쿨...");
        if (at(18, 33)) {
            Print(7, " 이 세계는 다섯개의 대륙이 있다고 하더군요.");
            Print(7, " 로어 대륙, 가이아 테라,  아프로디테 테라,");
            Print(7, "이쉬도 테라, 우라누스 테라가 있다고 들었습");
            Print(7, "니다.");
        }
        if (at(10, 30)) {
            Print(7, " 만약 당신이 베스퍼성으로 가려 한다면,  가");
            Print(7, "이아 테라를 거쳐  그 성이 있는  아프로디테");
            Print(7, "테라로 가야 합니다.");
        }

        // Prison
        if (at(50, 11)) {
            Print(7, " 당신은 정말 행운입니다.  로드안님은 한 번");
            Print(7, "선고내린 피고인에 대해서는  절대로 사면 시");
            Print(7, "켜 주는 일이 없었는데 말입니다.");
        }
        if (at(53, 11)) {
            Print(7, " 당신은 로드안님에 의해서 선택 되어진 인간");
            Print(7, "입니다.");
        }
        if (at(41, 10)) {
            if (party.etc[20] == 0)
                Print(7, " 특별한 일이 없는한 우리 둘은 평생 여기서 지낼거요.");
            else
                Print(7, " 나도 자네처럼 모험을 하고 싶소.");
        }
        if (at(63, 9)) {
            Print(7, " 나는 다른 종족과의 화합에 대한 책을  발간");
            Print(7, "하려다가 사형 선고를 받았습니다. 나는 정말");
            Print(7, "억울합니다.  로드안님의 말인즉 악을 행하는");
            Print(7, "다른 종족과의 화합은  악과의 화합을 말하는");
            Print(7, "것이라는 겁니다. 내가 아는 몇몇의 종족들은");
            Print(7, "해들 끼치기도 하지만 그것은 그저 그들의 속");
            Print(7, "성일 뿐이지 결코 본성이 악한 것은 아닙니다.");
            Print(7, "로드안님을 위시한 이곳의 교육체계는 너무나");
            Print(7, "선을 강조하며 악을 배척하려 합니다. 하지만");
            Print(7, "어느 정도의 필요악도 있어야 하며 이런 교육");
            Print(7, "방식에 의한 평화는 잠시뿐이지  마침내는 자");
            Print(7, "체적으로 붕괴해 버릴겁니다.");
        }
        if (at(40, 15)) {
            Print(7, " 넌 옆 방에 있었던 " + p1name + " 아냐.");
            Print(7, " 이제 제법 로드안의 충실한 개가 되었군. 그");
            Print(7, "가 주는 달콤한 사탕에 꼬리를 흔드는 격이라");
            Print(7, "...");
        }
        if (at(61, 15)) {
            Print(7, " 나는 전과가 많아서 사형선고를 받았습니다.");
            Print(7, "로드안님도 너무하십니다.  범죄없는  세상을");
            Print(7, "만드는 것도 좋지만 나 같은 좀도둑에게도 사");
            Print(7, "형을 선고하시다니...");
        }

        // Grave
        if (at(72, 78)) {
            Print(7, " 여기는 로어성의 묘지입니다. 함부로 들어가");
            Print(7, "지 마십시요.");
        }
        if (at(64, 76)) {
            Print(7, " 비석에 어떤 글이 적혀 있었다.");
            Print(7, "");
            Print(7, "");
            Print(15, "   여기는 위대한 예언자 데네브의 묘");
            Print(15, "     620년 8월 30일 여기에 잠들다");
            Print(7, "");
            Print(7, "");
            Print(7, " 당신이 자세히 보니 묘비의 아래쪽에 희미하");
            cPrint(7, 11, "게 ", "27,6", " 이라고 새겨져 있음을 알았다.");
        }

        // Citizen
        if (at(56, 64)) {
            Print(7, " 이 세상에 존재하는 모든 선의 대표자는  로");
            Print(7, "드안님이며 악의 대표자는 에인션트 이블입니");
            Print(7, "다.");
        }
        if (at(59, 82)) {
            Print(7, " 세상의 악은 모두 응징되어야 합니다.  아마");
            Print(7, "그건 로드안님만이 할 수 있을 겁니다.");
        }
        if (at(10, 73)) {
            Print(7, " 로어 대륙에는 이곳 로어성과 대륙의 남동쪽");
            Print(7, "에 있는 라스트 디치성이 있습니다. 로어성은");
            Print(7, "지식의 성전이란 뜻이며  라스트 디치성은 최");
            Print(7, "후까지 버티는 자들의 성이란 뜻입니다.");
        }
        if (at(80, 23)) {
            Print(7, " 나는 대륙의 남동쪽에 있는 메너스 금광에서");
            Print(7, "일하고 있습니다.");
        }
        if (at(24, 37)) {
            Print(7, " 오크족과 트롤족은 미개 종족이라  우리와는");
            Print(7, "차원이 틀리지요.");
        }
        if (at(90, 42)) {
            Print(7, " 트롤족은 주로 동굴속에 살며 호전적인 성품");
            Print(7, "을 지녔어요.");
        }
        if (at(31, 64)) {
            Print(7, " 에인션트 이블은  이 세상에 악을 뿌리고 다");
            Print(7, "니는 아주 나쁜 자입니다.");
        }
        if (at(90, 82)) {
            Print(7, " 천신의 대륙과  대지신의 대륙은  큰 산맥을");
            Print(7, "경계로 나누어져 있다던데요.");
        }
        if (at(62, 38)) {
            Print(7, " 트롤족에 의한 베스퍼성 침공은 에인션트 이");
            Print(7, "블이 배후 조종한 것이라고  로드안님이 그러");
            Print(7, "시더군요.");
        }
        if (at(42, 31)) {
            Print(7, " 크리스탈로 소환 시킨 몇몇의 생물들은 절대");
            Print(7, "주인을 배반하지 않는다고 하더군요.");
        }

        // Soldier in Palace
        if (at(50, 51) || at(52, 51)) Print(7, " 당신의 성공을 진심으로 기원합니다.");
        if ((pas_x >= 48 && pas_x <= 54) && (pas_y >= 31 && pas_y <= 37)) {
            if (party.etc[20] < 3)
                Print(7, " 어서 우리 성주님을 만나 보시지요.");
            else
                Print(7, " 만세, 만세, 만만세, 로드안 만세");
        }

        // Lord Ahn Main Events
        if (at(51, 28)) {
            switch (party.etc[20]) {
                case 2:
                    Print(7, " 나는 베스퍼성 원정에 따른  지원자를  찾고");
                    Print(7, "있었소. 여러 방면으로 수소문한 끝에 당신을");
                    Print(7, "선택하게 된거요. 만약 당신이 베스퍼성 원정");
                    Print(7, "을 성공리에 끝낸다면  당신의 전과는 말끔이");
                    Print(7, "없에 주겠소.  그리고 당신이 원하는 삶을 제");
                    Print(7, "공해 줄 의양도 있소. 당신이 주축이 되어 여");
                    Print(7, "러 용사들을 합세 시킬 수도 있소. 당신이 성");
                    Print(7, "공적으로 임무를 완수하기 위한  자금도 주겠");
                    Print(7, "소.  베스퍼성의  위치를  잘 모르겠다면  마");
                    Print(7, "을 사람들에게 물어 보면 될거요.  다른 성에");
                    Print(7, "서 온 원정대와  별개로 행동해도  상관없소.");
                    Print(7, "그리고  내가 다른 성의 성주들에게도 당신에");
                    Print(7, "게 호의를 베풀어 주도록 부탁했으니  아프로");
                    Print(7, "디테 테라까지 가는데 별 어려움은 없을거요.");
                    Print(7, "당신을 믿겠소.");
                    Print(7, "");
                    Print(11, " [ 황금 + 5000 ]");
                    Print(7, "");
                    PressAnyKey();
                    party.gold += 5000;
                    party.etc[20]++;
                    break;

                case 3:
                    Print(7, " 당신은 아프로디테 테라의 베스퍼성으로  지");
                    Print(7, "금 즉시 떠나야 하오.");
                    break;

                case 4:
                    Print(7, " " + p1name + ", 정말 장한 일을 해주었소.");
                    Print(7, " 이제 당신은 완전한 자유의 몸이오. 이건 나");
                    Print(7, "의 성의라오.");
                    Print(11, " [ 황금 + 40000 ] [ 경험치 + 200000 ]");
                    party.gold += 40000;
                    for (int i = 1; i <= 6; i++) {
                        if (player[i].name_len != 0) {
                            player[i].experience += 200000;
                        }
                    }
                    party.etc[20]++;
                    break;

                case 5:
                    Print(7, " 당신에게  한 가지 부탁을  더 해도 되겠소?");
                    Print(7, "이 일은 당신과 같은 영웅들만이  할 수 있기");
                    Print(7, "때문에  당신에게 부탁하는 것이오.  아직 이");
                    Print(7, "세계에는 인간 이외에 코볼트족과 드라코니안");
                    Print(7, "족이 있소.  그 종족들은 오크나 트롤같은 하");
                    Print(7, "급 종족에 비해서는  월등한 전투력을 가지고");
                    Print(7, "있는데다가 드라코니안족 같은 경우에 있어서");
                    Print(7, "는 우리보다 더 뛰어난 지능과 문화를 가지고");
                    Print(7, "있소.  그들은 2개의 대륙을 차지하고 있으면");
                    Print(7, "서 항상 인간들을 배척하고 있기 때문에 인간");
                    Print(7, "들의 진출을 방해하는 요소라고  할 수 있소.");
                    Print(7, "만약 그들이 우리들이 있는 대륙을  침범한다");
                    Print(7, "면 어떻게 될런지는 충분히 예상될거요. 특히");
                    Print(7, "드라코니안족이 덤벼든다면 로어 대륙을 제외");
                    Print(7, "하고는 모두 전멸 될것이 뻔한 일인데다가 이");
                    Print(7, "곳 역시도 그리 오래는 대항하기 힘들것이오.");
                    Print(7, "그들이 숭배하고 있는 악의 화신 에인션트 이");
                    Print(7, "블이 그들을 선동하여 이곳으로 쳐들어왔을때");
                    Print(7, "를 상상해 봤소?  아마  수천년을 유지해오던");
                    Print(7, "이곳의 평화는 산산히 깨어져 버릴 것이오.");
                    Print(7, "");
                    party.etc[20]++;
                    PressAnyKey();
                    break;

                case 6:
                    Print(7, " 당신에게 부탁하겠소.  코볼트족과 드라코니");
                    Print(7, "안족을 선제 공격하여  이곳의 평화를 보장하");
                    Print(7, "고  신대륙 개척을 앞당기게 해주시오.  만약");
                    Print(7, "당신이 그 두 종족을 멸해준다면 나는 우라누");
                    Print(7, "스테라를 당신에게 넘겨줄 의양도 있소. 제발");
                    Print(7, "부탁하오.");
                    break;

                case 7:
                    Print(7, " 돌아왔군요. " + p1name + ".");
                    Print(7, " 역시 내가 보는 눈은 정확했소.  당신이라면");
                    Print(7, "분명히  우리에게  승리를 안겨줄거란걸 알았");
                    Print(7, "소. 당신에게 주기로 약속한 우라누스 테라도");
                    Print(7, "곧 당신의 손에 들어갈 거요. 그리고 그때 당");
                    Print(7, "신은 그곳의 성주가 되는 것이오.");
                    Print(11, " [ 황금 + 100000 ] [ 경험치 + 2500000]");
                    party.gold += 100000;
                    for (int i = 1; i <= 6; i++) {
                        if (player[i].name_len != 0) {
                            player[i].experience += 2500000;
                        }
                    }
                    PressAnyKey();
                    Print(7, " 하지만  그전에 먼저 할 일이 생겼소.  지금");
                    Print(7, "배리언트 피플즈에서 대대적인 반란이 발생했");
                    Print(7, "는데 빨리 진압하지 않으면  다른 성으로까지");
                    Print(7, "번질지도 모르오.  이번의 반란은  나에 대한");
                    Print(7, "반역임과 동시에  선에 대한 도전이라고 받아");
                    Print(7, "들이고 있소.  그들의 이런 행위는 여지껏 지");
                    Print(7, "켜온 평화를 깨어 버리려는  에인션트 이블의");
                    Print(7, "마지막 발악인것 같소.");
                    setbit(50);
                    party.etc[20]++;
                    break;

                case 8:
                    Print(7, " 즉시 배리언트 피플즈로 가서 폭동을 진압해");
                    Print(7, "주시오.  당신이 그 일을 끝내고 돌아오면 나");
                    Print(7, "는 당신에게 공작의 작위와 함께 우라누스 테");
                    Print(7, "라에  당신의 공국을 건설하도록 허락해 주겠");
                    Print(7, "소.");
                    break;

                case 9:
                    Print(7, " 당신 덕택에  세상은 완전한 평화를 얻게 되");
                    Print(7, "었소.  그리고 약속한대로 당신에게 우라누스");
                    Print(7, "테라의 통치권과 공작의 칭호를 내려주겠소.");
                    Print(7, " 축하하오, " + p1name + " 공작...");
                    Print(7, " 그런데  미안하지만 한가지 부탁만  더 들어");
                    Print(7, "주겠소? 당신이 멸망시킨 네종족이 사실은 완");
                    Print(7, "전히 사라진게 아니오. 그들은 지금 현재, 죽");
                    Print(7, "어간 그 종족의 의지가  하나로 뭉쳐  자신이");
                    Print(7, "있던 대륙에  원혼이 되어 남아있소.  그것들");
                    Print(7, "중에서는  스스로의 의지로 이곳을 향해 오는");
                    Print(7, "원혼도 있소.  그 원혼들이 이 대륙에 들어와");
                    Print(7, "서 우리에게 복수하기 전에 우리가 먼저 그들");
                    Print(7, "을 발견하여 강한 힘속에 봉인 시켜 버려야하");
                    Print(7, "오. 그래서 부탁하건데, 이 크리스탈 볼을 받");
                    Print(7, "으시오. 이 크리스탈 볼 속을 들여다 보면 오");
                    Print(7, "크, 트롤, 코볼트, 드라코니안 종족의 원혼이");
                    Print(7, "있는 장소가 드러날꺼요. 그리고 그 장소에서");
                    Print(7, "그들의 원혼을 발견하여 크리스탈 볼 속에 봉");
                    Print(7, "인 시켜 버리시오. 네 종족을 모두 봉인 시켰");
                    Print(7, "을때  그 크리스탈 볼을 나에게 가져오면  그");
                    Print(7, "이후는 내가 알아서 하겠소.  그리고  당신의");
                    Print(7, "영토인 우라누스 테라로 이동 시켜 주겠소.");
                    Print(11, " [ 크리스탈 볼 + 1 ]");
                    Print(7, "");
                    PressAnyKey();
                    party.crystal[9]++;
                    party.etc[20]++;
                    break;

                case 10:
                    Print(7, " 내가 준 크리스탈 볼을 이용하여  네 종족의");
                    Print(7, "원혼을 모두 그 안에 봉인시켜서 나에게 가져");
                    Print(7, "오시오. 꼭 부탁하오.");
                    break;

                case 11: {
                    pas_x = 51; pas_y = 31;
                    int face = 1;
                    if (player[1].classtype == magic) face += 8;
                    Delete_Scroll_Screen();
                    Delay(1000);
                    Silent_Scroll();
                    Print(10, " 오, " + p1name + " 공작!");
                    Print(10, " 당신은 나의 마지막 부탁까지 훌륭하게 들어");
                    Print(10, "주었소.  이제는 당신에게 더 이상 이런 일을");
                    Print(10, "시키지 않겠다고 약속하오.  먼저, 네 종족의");
                    Print(10, "원혼을 봉인한 크리스탈 볼을 이리 주시오.");
                    PressAnyKey();
                    
                    for (int i = 5; i >= 4; i--) {
                        PutTile(5, i + 1, 42);
                        PutSprite(5, i, face);
                        Delay(500);
                    }
                    Delay(1000);
                    for (int i = 4; i <= 5; i++) {
                        PutTile(5, i, 42);
                        PutSprite(5, i + 1, face);
                        Delay(500);
                    }
                    
                    Print(10, " 이제는 이 수정구슬을 완전히 봉인해서 다시");
                    Print(10, "는 그들의 원혼이 우리를 위협하지 못하게 해");
                    Print(10, "야겠소.");
                    Delay(1000);
                    Print(7, "");
                    Print(7, " 로드안의  손에 들린 크리스탈 볼이  가볍게");
                    Print(7, "떠오르기 시작했다. 로드안은 차츰 양손의 기");
                    Print(7, "력을 증강 시켰고 크리스탈 볼은 붉게 달아오");
                    Print(7, "르기 시작했다. 곧이어 크리스탈 볼은 이글거");
                    Print(7, "리는 에너지 공으로 변하였다.");
                    Print(7, "");
                    PutSprite(5, 4, 20);
                    PressAnyKey();
                    
                    Print(10, " 자, 이제 이것을 아무도 구제 할 수 없는 곳");
                    Print(10, "으로 보내 버려야겠소.  그곳은 바로 로어 대");
                    Print(10, "륙의 지하에 있는 하데스 테라라고들 하는 곳");
                    Print(10, "이오.");
                    Delay(1000);
                    Print(7, "");
                    Print(7, " 로드안이 양손을 내리자 에너지 공은 서서히");
                    Print(7, "밑으로 떨어지기 시작했다.  그리고는 바닥에");
                    Print(7, "스며드는 것처럼 땅속으로 사라져 버렸다.");
                    Silent_Scroll();
                    PressAnyKey();
                    
                    Print(10, " 그럼, 이제 당신을 우라누스 테라로 보내 주");
                    Print(10, "겠소.");
                    PressAnyKey();
                    PutTile(5, 3, 42);
                    PutTile(5, 4, 54);
                    
                    Print(10, " 그전에 당신은 새로운 모습으로 태어날꺼요.");
                    Print(10, "그러기 위해서는  당신의 일행들과 나의 신하");
                    Print(10, "들이 자리를 피해줘야만 하오.  그 이유는 차");
                    Print(10, "차 알려 드리리다.");
                    PressAnyKey();
                    
                    for (int i = 2; i <= 6; i++) {
                        player[i].name_len = 0;
                    }
                    Display_Condition();
                    
                    for (int j = 3; j <= 5; j++) {
                        PutTile(2, j * 2, 44);
                        PutTile(8, j * 2, 44);
                    }
                    
                    Print(7, " 잠시간의 침묵이 흘렀다.");
                    Delay(3000);
                    Print(7, "");
                    Print(7, " 지금  주위에는 아무도 없다는 것을  확인한");
                    Print(7, "로드안은  갑자기 당신을 향해 독심을 사용하");
                    Print(7, "기 시작했다.  당신은 긴장을 완전히 풀고 있");
                    Print(7, "었던데다가 로드안의 갑작스런 최면에 말려들");
                    Print(7, "어  방어를 할 기회도 없이  그에게 독심술을");
                    Print(7, "당하였다. 당신은 곧 당신의 자유의사를 잃어");
                    Print(7, "버렸고 정신이 아득해지기 시작하였다.");
                    PressAnyKey();
                    
                    Print(10, " 정말 미안하게 됐네, " + p1name + ".");
                    Print(10, " 당신은  여지껏 나를 위해 일해 주었는데 나");
                    Print(10, "는 당신에게 이렇게 할 수 밖에는 없다네. 나");
                    Print(10, "는  처음부터 이미 당신을  지목하고 있었지.");
                    Print(10, "나와의 관계를 만들기 위해서  당신을 살인죄");
                    Print(10, "로 몰았던 것도 나였고,  항상 천리안과 투시");
                    Print(10, "로  당신을 감시하던 사람도  바로 나였다네.");
                    Print(10, "지금의 당신은  나 로드안을 제외하고는 명실");
                    Print(10, "공히 이 세계 최강의 용사이지. 내가 지금 하");
                    Print(10, "려는 이야기를 잘 들어보게.");
                    PressAnyKey();
                    
                    Print(10, " 이곳은 현재 너무 평화가 지속되어서 사람들");
                    Print(10, "이 현재의 평화를 전혀 고마워 할줄을 모른다");
                    Print(10, "네.  그래서 내가 새로운 일을 지금 꾸미려하");
                    Print(10, "지. 그 일이란 어떤 악의 추종자가 이 세계에");
                    Print(10, "나타나  사람들을 위협하고 악을 퍼뜨려서 이");
                    Print(10, "세계의 평화를 말살하려고 할때 나와 내가 선");
                    Print(10, "택한 인물의 활약으로 세계는 다시 평화를 찾");
                    Print(10, "게되고  주민들은 더욱 나에게 감사하며 선을");
                    Print(10, "지키려 노력한다는 내용일세. 이런 일을 내가");
                    Print(10, "꾸몄다는 것을  아무도 알지못한다면 나는 분");
                    Print(10, "명히  평화의 구세주로서의 자리를  확보하게");
                    Print(10, "된다네.  여기서 필요한 사람은  바로 위에서");
                    Print(10, "말한 '악의 추종자'가 될  사람이네.  하지만");
                    Print(10, "아무도 지원하지 않을거란건  뻔한 사실이지.");
                    Print(10, "그래서 세계를 위협 할 만한 능력을 가진  당");
                    Print(10, "신을 이런식으로 끌어 들인거라네. 당신은 항");
                    Print(10, "상 악을 퍼뜨리려  이 세계에  발을 내리지만");
                    Print(10, "번번히 나와 내가 선택한 용사에 의해 패배하");
                    Print(10, "게 되지. 그것이 바로 당신이 이제 지게될 운");
                    Print(10, "명이라네.  당신은 이제 스스로의 자유의사에");
                    Print(10, "관계없이 운명에 묶여서 행동하게 될걸세. 하");
                    Print(10, "지만 패배를 너무 의식할 필요는 없네.  최종");
                    Print(10, "적인 승리는 당신이 하게 만들어 줄테니까...");
                    Print(10, "");
                    PressAnyKey();
                    
                    Print(7, " 로드안은 말을 마치자  당신을 향해  백색의");
                    Print(7, "가루를 뿌리며 주문을 외우기 시작했다.");
                    Delay(2000);
                    PutTile(5, 6, 42);
                    PutSprite(5, 6, 26);
                    PutSprite(5, 5, 25);
                    PressAnyKey();
                    
                    cPrint(10, 11, " 당신은 이제 악의 추종자로서의 형체를 갖추", "", "");
                    cPrint(10, 11, "었다네. 4 m 가량의 키와 강력한 마력을 겸비", "", "");
                    cPrint(10, 11, "한 악의 최대 전투사로서...  당신은 이제 ", "네", "");
                    cPrint(10, 11, "", "크로만서", "란 이름으로  이 세상의 평화가 오래");
                    Print(10, "지속 될 때  이 세상을 위협하려 내려올 운명");
                    Print(10, "을 가지게 되었다네.  당신은 그때  우라누스");
                    Print(10, "테라를 기점으로 세력을 뻗치게 될걸세. 그때");
                    Print(10, "는 당신이 우라누스 테라의 주인이 될거라네.");
                    Print(10, "아마 그때서야  내가 당신에게 우라누스 테라");
                    Print(10, "를 주겠다는 약속이 지켜질 걸세.");
                    Print(10, "");
                    Print(10, " 그럼... 안녕, 네크로만서.");
                    Print(10, "");
                    PressAnyKey();
                    
                    PutTile(5, 6, 42);
                    PutTile(5, 5, 42);
                    Print(7, " 네크로만서로 변한 당신은 공간의 틈을 통해");
                    Print(7, "다른 공간으로 빠져 버렸다.  그리고 로어 세");
                    Print(7, "계의 평화가 너무 오래 유지될 때  당신은 운");
                    Print(7, "명에 의해 이 세계로 내려 올 것이다.");
                    PressAnyKey();
                    
                    Clear();
                    Screen_Fade_Out(500);
                    SoundOn = false;
                    mapname = "UnderGrd";
                    Load();
                    pas_x = 31; pas_y = 21;
                    PutMap(46, 48, 54);
                    OverLap(400, false);
                    PutSprite(5, 6, 20);
                    
                    while (pas_x != 46 || pas_y != 48) {
                        pas_x = pas_x + sgn(46 - pas_x);
                        pas_y = pas_y + sgn(48 - pas_y);
                        Scroll(false);
                        PutSprite(5, 6, 20);
                        Delay(300);
                    }
                    
                    mapname = "Dome";
                    Load();
                    pas_x = 26; pas_y = 6;
                    PutMap(28, 21, 53);
                    OverLap(400, false);
                    PutSprite(5, 6, 20);
                    
                    while (pas_x != 26 || pas_y != 14) {
                        pas_x = pas_x + sgn(26 - pas_x);
                        pas_y = pas_y + sgn(48 - pas_y);
                        Scroll(false);
                        PutSprite(5, 6, 20);
                        Delay(500);
                    }
                    
                    Scroll(false);
                    Delay(5000);
                    for (pas_y = 15; pas_y <= 18; pas_y++) {
                        Scroll(false);
                        Delay(1000);
                    }
                    Delay(3000);
                    
                    for (int j = 11; j >= 9; j--) {
                        if (j < 11) PutTile(8, j + 1, 44);
                        PutTile(8, j, 52);
                        Delay(500);
                    }
                    
                    Print(13, " 아스모데우스여,  방금 에너지 공이 이 방으");
                    Print(13, "로 들어가는 것을 보았는데 어떻게 된것인가?");
                    PressAnyKey();
                    
                    Print(12, " 예, 메피스토펠레스님.  그것은 원혼이 봉인");
                    Print(12, "된 에너지 공인데  로드안이 지하세계로 버린");
                    Print(12, "것입니다. 그런데 그것이 카미너스씨의 몸 속");
                    Print(12, "으로 들어갔습니다.");
                    PressAnyKey();
                    
                    Print(13, " 그래? 그렇다면 분명 카미너스씨의 자손중에");
                    Print(13, "서 원혼을 봉인한채 태어 나는 아이가 틀림없");
                    Print(13, "이 있을 것이다. 그 아이가 태어났을때, 우리");
                    Print(13, "의 힘으로 봉인된 원혼을 풀어 버린다면 우리");
                    Print(13, "는 그 힘을 이용해 세계를 지배할 수 있을 것");
                    Print(13, "이다. 후후후, 로드안은 정말 멍청한 짓을 했");
                    Print(13, "군.");
                    Print(13, " 우리에게 도리어 이런 기회를 줘 버리다니..");
                    PressAnyKey();
                    
                    End_Demo(1);
                    break;
                }
            }
        }
    }

    if (mapname == "Dark" || mapname == "DARK") {

        if (at(87, 73) || at(94, 68) || at(91, 65)) { Grocery(); return; }
        if (at(8, 71) || at(14, 69) || at(14, 73)) { Weapon_Shop(); return; }
        if (at(86, 12) || at(87, 14)) { Hospital(); return; }
        if (at(9, 64)) { Sell_Item(); return; }
        if (at(82, 16)) { Medicine(); return; }
        if (at(21, 12) || at(25, 13)) { About_Class(); return; }
        if (at(28, 12)) { Sell_EXP(); return; }

        if (at(19, 53)) {
            Print(14, " 이 세계의 창시자는 안영기님이시며 그는 위대한 프로그래머입니다.");
        }
        if (at(13, 55)) {
            Print(7, " 이십 몇년 전에  이곳은  네크로만서란 자에");
            Print(7, "의해 큰 혼란이 있었습니다.");
        }
        if (at(24, 50)) Print(7, " 안녕하시오.");
        if (at(13, 27) || at(18, 27)) Print(7, " 여기는 코리아 위스키가 제일 잘 팔립니다.");
        if (at(21, 33)) Print(7, " 요즈음은 너무 할 일이 없어서 따분합니다.");
        if (at(18, 33)) {
            Print(7, " 당신  정말 이상한 차림이군요.  거의 100년");
            Print(7, "전에나 입던 옷인데...");
        }
        if (at(13, 32)) Print(7, " 킥킥.. 정말 우스운 복장을 하고 있군요.");
        if (at(18, 38)) Print(7, " 요새는 너무 따분한 세상의 연속이에요.");
        
        // Prison
        if (at(50, 11) || at(53, 11)) {
            Print(7, " 여태껏 근 20년간  아무도 이곳에 갇히지 않");
            Print(7, "았죠. 그래서 저는 너무 할 일이 없습니다.");
        }
        
        // Grave
        if (at(72, 78)) {
            Print(7, " 여기는 로어성의 묘지입니다. 함부로 들어가");
            Print(7, "지 마십시요.");
        }
        
        if (at(77, 46)) Print(7, " 메너스는 폐광이 된지 50년이 넘었죠.");
        if (at(83, 59)) {
            Print(7, " 라스트디치는 주민 등록을 마친 자만이 들어");
            Print(7, "갈 수있죠.");
        }
        if (at(83, 25)) {
            Print(7, " 20년 전에 알비레오라는 사람이 이상한 예언");
            Print(7, "서를 남겼죠. 거기서 다크 메이지라는 사람이");
            Print(7, "나타난다고 했었는데 실제로는 나타나지 않았");
            Print(7, "어요.");
        }
        if (at(68, 61)) {
            Print(7, " 당신들의 옷은 정말 특이하군요. 100년전 사");
            Print(7, "람 같아요.");
        }

        // Soldiers
        if (at(50, 51) || at(52, 51)) Print(7, " 안녕하시오.");
        if ((pas_x >= 48 && pas_x <= 54) && (pas_y >= 31 && pas_y <= 37)) Print(7, " ...");

        // Final Boss Event
        if (at(51, 28)) {
            Print(6, " 로드안!! 당신은 나를 기억하겠소?  100년전");
            Print(6, "쯤에  나를 이용하여  무고한 네 종족을 멸망");
            Print(6, "시키게 만들었던것도 말이오.");
            Print(7, "");
            Print(7, " 당신의 말이 채 끝나기도 전에  로드안은 다");
            Print(7, "급하게 외쳤다.");
            Print(7, "");
            Print(9, " 경호원, 경호원 !!");
            Print(7, "");
            Print(7, " 순간 몇명의 용사가 주위에서 나와 로드안을");
            Print(7, "감쌌다.");
            PressAnyKey();
            
            // First wave of enemies
            enemynumber = 7;
            for (int i = 1; i <= 3; i++) joinenemy(i, 69);
            for (int i = 4; i <= 6; i++) joinenemy(i, 68); // Fixed logic from original Pascal '5 to 3' typo
            joinenemy(7, 70);
            
            do {
                DisplayEnemies(true);
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) Clear();
            } while (party.etc[6] != 0);
            Clear();
            
            // Second wave (Lord Ahn)
            enemynumber = 5;
            for (int i = 1; i <= 3; i++) joinenemy(i, 71);
            joinenemy(4, 72);
            joinenemy(5, 74);
            
            do {
                DisplayEnemies(true);
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) Clear();
            } while (party.etc[6] != 0);
            Clear();
            
            Print(15, " 당신은 드디어 숙적 로드안을 물리쳤다.");
            PressAnyKey();
            End_Demo(3);
        }
    }
}