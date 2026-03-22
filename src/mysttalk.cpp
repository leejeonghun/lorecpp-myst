#include <cstring>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "mysttalk.h"
#include "mystsub.h"
#include "uhanx.h"
#include "mystbatt.h"
#include "mystend.h"
#include "mysttal1.h"
#include "mysttal2.h"

// Dispatch to Sub0 (Lore/Dark)
extern void TalkMode_Sub0(); 
// Sub2 (Dungeons/Caves)
extern void TalkMode_Sub2();

void TalkMode() {
    if (mapname == "Lore" || mapname == "LORE" || mapname == "Dark" || mapname == "DARK") {
        TalkMode_Sub0();
    } else if (mapname == "Imperium" || mapname == "IMPERIUM" || mapname == "DracTown" || mapname == "DRACTOWN") {
        TalkMode_Sub3();
    } else if (Map_Header.Tile_Type == town) { // 파스칼 원본에 맞춰 수정
        TalkMode_Sub1();
    } else {
        TalkMode_Sub2();
    }
}

void TalkMode_Sub1() {
    Clear();

    // ==========================================
    // LastDtch (라스트 디치 성)
    // ==========================================
    if (mapname == "LastDtch" || mapname == "LASTDTCH") {
        if (at(57, 17) || at(54, 20) || at(58, 22)) { Grocery(); return; }
        if (at(59, 56) || at(59, 58) || at(59, 60)) { Weapon_Shop(); return; }
        if (at(17, 60) || at(17, 56)) { Hospital(); return; }
        if (at(17, 58)) { Medicine(); return; }
        if (at(16, 24) || at(24, 19)) { About_Class(); return; }
        if (at(18, 19)) { Sell_EXP(); return; }

        if (at(21, 61)) {
            if (getbit(4)) {
                Print(7, " 베스퍼성에 대한 복수를 해주셔서 대단히 감사합니다.");
            } else if (party.etc[5] > 0 && !getbit(10)) {
                Print(7, " 원하신다면 제가 트롤의 글을 가르쳐 드리죠.");
                Print(7, " 식량 50 개만 주시면 일주일안에 모두 가르쳐 드리겠습니다.");
                m[1] = "그에게 트롤의 글을 배운다"; 
                m[2] = "그의 제안을 거절한다";
                if (Select(150, 2, 2, false, true) == 1) {
                    if (party.food >= 50) {
                        Delete_Scroll_Screen();
                        Print(7, " 당신은 그에게 일주일간 트롤의 글을 배웠다.");
                        PressAnyKey();
                        party.day += 7; 
                        party.food -= 50; 
                        setbit(10);
                        Scroll(true);
                        Print(7, " 이제 당신은 트롤의 글을 읽고 쓸 수 있게 되었다.");
                    } else {
                        Print(7, " 하지만 당신에게는 충분한 식량이 없군요.");
                    }
                }
            } else {
                Print(7, " 저는 베스퍼성에서 이곳으로 가까스로 탈출해 왔습니다. 아마 베스퍼성의 생존자는 아무");
                Print(7, " 도 없을 것입니다. 나는 내 눈으로 나의 아내와 어린 자식이 트롤족의 곤봉에 죽어가는 것");
                Print(7, " 을 봤습니다. 나는 차마 그곳을 떠나고 싶지는 않았지만 이 소식을 다른 대륙에 알리기");
                Print(7, " 위해 겨우 탈출에 성공하여 여기서 치료를 받고 있습니다.");
            }
        }
        
        if (at(61, 58)) {
            cPrint(7, 11, " 나의 이름은 ", "알타이르", "라고 하오. 독수리같은");
            Print(7, " 용맹을 지녔다는 뜻이지요. 나는 이 무기상점");
            Print(7, " 에서 일하면서 파이터로서의 기술을 연마했소.");
            Print(7, " 만약 당신이 나를 필요로 한다면 기꺼이 도와 주겠소.");
            m[1] = "나에게는 당신같은 사람이 필요하오"; 
            m[2] = "그럴 필요까지는 없소";
            if (Select(165, 2, 2, false, true) == 1) {
                int k = 0; 
                for(int i = 1; i <= 6; i++) { if(player[i].name_len == 0) { k = i; break; } }
                if (k > 0 && k < 6) {
                    Lore& p = player[k];
                    p.name_len = std::strlen("알타이르"); std::strcpy(p.name, "알타이르");
                    p.sex = male; p._class = 1; p.classtype = sword; p.level = 3;
                    p.strength = 19; p.mentality = 4; p.concentration = 3;
                    p.endurance = 18; p.resistance = 10; p.agility = 16;
                    p.accuracy = 15; p.luck = 14; p.poison = 0; p.unconscious = 0; p.dead = 0;
                    p.hp = p.endurance * p.level * 10; p.sp = 0; p.experience = 0;
                    p.potential_experience = ReturnEXP(p.level);
                    p.weapon = 9; p.shield = 2; p.armor = 2; p.potential_ac = 2;
                    p.sword_data.sword_skill = 10; p.sword_data.axe_skill = 30; p.sword_data.spear_skill = 10;
                    p.sword_data.bow_skill = 10; p.sword_data.shield_skill = 10; p.sword_data.fist_skill = 10;
                    Modify_Item(k); PutMap(61, 58, 44); setbit(39);
                    Silent_Scroll(); Display_Condition(); Clear();
                } else {
                    Print(7, " 벌써 사람이 모두 채워져 있군요. 다음 기회를 기다리지요.");
                }
            } else asyouwish();
        }

        if (at(34, 30)) {
            Print(7, " 나의 이름은 내가 가진 이 하프에서 유래된");
            cPrint(7, 11, "", "베가", "라고 하며 현재는 이곳의 음유시인으로");
            Print(7, " 있습니다. 나는 원래 이곳 저곳을 돌아다는 성미라서 계속 이곳에 머무르는 것은 매우 답");
            Print(7, " 답합니다. 그래서 당신과 함께 여행하고 싶습니다.");
            m[1] = "좋소, 허락하지요"; 
            m[2] = "우리가 하는 일은 너무 위험해서...";
            if (Select(160, 2, 2, false, true) == 1) {
                int k = 0; 
                for(int i = 1; i <= 6; i++) { if(player[i].name_len == 0) { k = i; break; } }
                if (k > 0 && k < 6) {
                    Lore& p = player[k];
                    p.name_len = std::strlen("베가"); std::strcpy(p.name, "베가");
                    p.sex = female; p._class = 2; p.classtype = magic; p.level = 4;
                    p.strength = 6; p.mentality = 16; p.concentration = 19;
                    p.endurance = 10; p.resistance = 17; p.agility = 9;
                    p.accuracy = 15; p.luck = 18; p.poison = 0; p.unconscious = 0; p.dead = 0;
                    p.hp = p.endurance * p.level * 10; p.sp = 0; p.experience = 0;
                    p.potential_experience = ReturnEXP(p.level);
                    p.weapon = 0; p.shield = 0; p.armor = 1; p.potential_ac = 0;
                    p.sword_data.sword_skill = 10; p.sword_data.axe_skill = 40; p.sword_data.spear_skill = 10;
                    p.sword_data.bow_skill = 10; p.sword_data.shield_skill = 10; p.sword_data.fist_skill = 10;
                    Modify_Item(k); PutMap(34, 30, 47); setbit(41);
                    Silent_Scroll(); Display_Condition(); Clear();
                } else {
                    Print(7, " 벌써 사람이 모두 채워져 있군요. 다음 기회를 기다리겠습니다.");
                }
            } else asyouwish();
        }

        if (at(38, 17)) { // Lord
            if (!getbit(4)) {
                Print(7, " 나는 로드안에게서 당신이 여기로 온다는 소식을 들었소. 유감스럽게도 우리 성의 원정대");
                Print(7, " 는 벌써 떠났다오. 당신도 빨리 뒤따라 갔으면 하오. 성안의 북쪽에 가이아 테라로 통하");
                Print(7, " 는 게이트가 있소. 그곳으로 들어가면 가이아 테라로 이동 될 것이오. 돌아오는 방법도 마");
                Print(7, " 찬가지라오. 부디 성공하기를 빌겠소.");
            } else {
                Print(7, " 당신은 역시 로드안이 지목한 용사답소. 정말 훌륭한 일을 해내었소.");
            }
        }

        if (at(60, 42)) Print(7, " 내가 전에 어떤 모험가를 통해서 얼핏 들었는데 이 세계에는 모두 여섯개의 테라가 있다고 들었습니다.");
        if (at(61, 46)) Print(7, " 정말 로드안은 선의 상징입니다. 이번에도 사건이 터지자 마자 악을 무찌르기 위해서 노력을 다 했으니까 말입니다.");
        if (at(56, 59)) Print(7, " 전갈의 심장을 상징하는 이름을 가진 안타레스라는 자의 마법 능력은 정말 대단했습니다. 한때, 가이아 테라성의 수석 위저드로서 지상 최대의 공격 마법을 구사하더군요. 약 몇년 전에 이곳에 머물렀던 적이 있습니다.");
        if (at(36, 19)) Print(7, " 이 세상에서 로드안만큼 칭송받는 인물도 드물 겁니다. 그는 항상 선의 선봉에 서서 주민들을 악으로부터 지켜내고 있으니까요.");
        if (at(36, 21)) Print(7, " '테라'라고 하는 말은 라틴어로 대륙이란 뜻이지요.");
        if (at(41, 20)) Print(7, " 이 성안에 있는 게이트는 가이아 테라의 배리언트 피플즈라는 성으로 통해 있습니다.");
        if (at(41, 18)) Print(7, " 라스트 디치성과 배리언트 피플즈성은 생김새가 똑같습니다.");
        if (at(41, 22)) Print(7, " 으윽! 요즘에도 음악 제어하기 힘든 파스칼 언어로 게임을 만드는 무식한 사람이 있다니.");
        if (at(37, 41) || at(40, 41)) Print(7, "... ...");
    }

    // ==========================================
    // Valiant (배리언트 피플즈 성)
    // ==========================================
    if (mapname == "Valiant" || mapname == "VALIANT") {
        if (at(54, 27) || at(58, 26) || at(61, 21)) { Grocery(); return; }
        if (at(54, 57) || at(54, 59)) { Weapon_Shop(); return; }
        if (at(24, 59) || at(20, 60) || at(19, 56)) { Hospital(); return; }
        if (at(21, 55)) { Medicine(); return; }
        if (at(20, 21) || at(22, 23)) { About_Class(); return; }
        if (at(15, 19)) { Sell_EXP(); return; }

        if (at(17, 24)) {
            cPrint(7, 11, " 나의 이름은 메듀사의 머리를 뜻하는 ", "알골", "이라고하오.");
            Print(7, " 나는 이름처럼, 보기만해도 상대를 제압할 수 있는 용맹을 지녔소. 나는 소환계");
            Print(7, " 통의 마법을 이 곳에서 수 년을 연마했다오. 이제는 나의 능력을 실전에 이용할 때가 되");
            Print(7, " 었소. 나는 꼭 당신의 원정을 도와주고 싶소.");
            m[1] = "당신같이 용맹을 지닌자라면 좋소"; m[2] = "않됐지만 저는 지금 바쁘군요";
            if (Select(180, 2, 2, false, true) == 1) {
                int k = 0; for(int i = 1; i <= 6; i++) { if(player[i].name_len == 0) { k = i; break; } }
                if (k > 0 && k < 6) {
                    Lore& p = player[k];
                    p.name_len = std::strlen("알골"); std::strcpy(p.name, "알골");
                    p.sex = male; p._class = 3; p.classtype = magic; p.level = 3;
                    p.strength = 10; p.mentality = 17; p.concentration = 15;
                    p.endurance = 8; p.resistance = 12; p.agility = 11;
                    p.accuracy = 14; p.luck = 20; p.hp = p.endurance * p.level * 10; p.sp = 0;
                    p.potential_experience = ReturnEXP(p.level);
                    p.weapon = 0; p.shield = 0; p.armor = 1; p.potential_ac = 0;
                    p.sword_data.sword_skill = 10; p.sword_data.axe_skill = 20; p.sword_data.spear_skill = 20;
                    p.sword_data.bow_skill = 0; p.sword_data.shield_skill = 0; p.sword_data.fist_skill = 35;
                    Modify_Item(k); PutMap(17, 24, 27); setbit(40);
                    Silent_Scroll(); Display_Condition(); Clear();
                } else Print(7, " 벌써 사람이 모두 채워져 있군요. 다음 기회를 기다리지요.");
            } else asyouwish();
        }

        if (at(56, 40)) {
            Print(7, " 나는 켄타우루스의 정기를 받은 사냥꾼으로,");
            cPrint(7, 11, "", "프록시마", "라고하오. 이름에서 알수 있듯이 나의 수호성은 알파 센타우리라오.");
            Print(7, " 사냥을 나온 중에 당신이 이 곳에 있다는 소리를 듣고 당신의 일행에 참가 하려고 왔던거요. 나를 받아 들이겠소?");
            m[1] = "좋소, 승락하지요"; m[2] = "글세요, 곤란한데요";
            if (Select(175, 2, 2, false, true) == 1) {
                int k = 0; for(int i = 1; i <= 6; i++) { if(player[i].name_len == 0) { k = i; break; } }
                if (k > 0 && k < 6) {
                    Lore& p = player[k];
                    p.name_len = std::strlen("프록시마"); std::strcpy(p.name, "프록시마");
                    p.sex = male; p._class = 4; p.classtype = sword; p.level = 4;
                    p.strength = 15; p.mentality = 5; p.concentration = 6;
                    p.endurance = 16; p.resistance = 11; p.agility = 16;
                    p.accuracy = 19; p.luck = 10; p.hp = p.endurance * p.level * 10; p.sp = 0;
                    p.potential_experience = ReturnEXP(p.level);
                    p.weapon = 26; p.shield = 0; p.armor = 2; p.potential_ac = 1;
                    p.sword_data.sword_skill = 0; p.sword_data.axe_skill = 0; p.sword_data.spear_skill = 10;
                    p.sword_data.bow_skill = 40; p.sword_data.shield_skill = 0; p.sword_data.fist_skill = 10;
                    Modify_Item(k); PutMap(56, 40, 27); setbit(42);
                    Silent_Scroll(); Display_Condition(); Clear();
                } else Print(7, " 벌써 사람이 모두 채워져 있군요. 다음 기회를 기다리지요.");
            } else asyouwish();
        }

        if (at(38, 17)) {
            if (!getbit(5) || getbit(6)) {
                Print(7, " 나의 성 배리언트 피플즈에 오신 것을 대단이 환영하오. 우리 성의 이름에서 알 수 있듯이");
                Print(7, " 이 곳의 사람들은 진정한 용기에서 오는 참다운 용기를 아주 높이 평가하고 있소.");
                Print(7, " 당신같이 용기있는 자들이라면 이 성 어느 곳에서도 환영을 받을 것이오.");
            } else {
                Print(7, " 당신의 위용은 모두 전해 들었소. 인류를 위협하는 세 종족을 없에고 마지막 남은 우라누스 테라의 드라코니안족을 벌하러 간다고 들었소.");
                Print(7, " 나에게는 우라누스 게이트를 여는 마법이 있소. 그럼 당신을 우라누스 테라로 보내 드리리다.");
                PressAnyKey();
                Delete_Scroll_Screen();
                mapname = "Ground5";
                Load();
                OverLap(400, true);
            }
        }

        if (at(45, 45)) Print(7, "가이아 테라는 대지신을 상징하는 대륙입니다.");
        if (at(32, 61)) Print(7, " 오크족은 미개한 집단이지만 개중에는 머리가 약간 뛰어나서 마법 능력이 있는 자도 있다고 하더군요.");
        if (at(50, 56)) Print(7, " 오크족의 마을은 대륙의 남동쪽에 있습니다. 그들의 무기는 시원챦은 것들뿐이라 우리에게 위협이 되지는 않습니다.");
        if (at(44, 31)) {
            if (!getbit(5) || getbit(6)) Print(7, " 이 대륙에는 모두 4 개의 대륙으로 가는 게이트가 있습니다. 그 중, 2 개의 게이트가 이 성 안에 있습니다.");
            else Print(7, " 지금 당신은 우라누스 게이트를 찾고 있는 것 같은데, 그렇다면 저희 성주님을 만나 보십시요.");
        }
        if (at(36, 19)) Print(7, " 고대 최강의 마법사였던 레드 안타레스의 아들이 시공간을 넘어 이 세계 어딘가에서 살고 있다고 들었습니다.");
        if (at(41, 21)) Print(7, " 세상에는 크리스탈에 여러가지 힘을 봉인한 물질이 있습니다. 가장 강력한 영혼의 크리스탈은 모든 적을 한꺼번에 유체이탈 시켜 버린다더군요. 그것뿐만 아니라 화염의 크리스탈과 한파의 크리스탈 같은 공격용 크리스탈, 소환의 크리스탈 같은 소환용 크리스탈도 있고, 치료용과 그 이외의 용도로 쓰이는 것도 있다고 들었습니다.");
        if (at(41, 19)) {
            if (!getbit(4)) {
                Print(7, " 제 생각에는 당신이 지금하려고 하는 일이 옳지 않다고 봅니다. 비록 트롤족이 야만스럽고 호전적이라는 하지만 아무런 이유없이 베스퍼성을 침공하려 했겠습니까?");
                Print(7, " 당신은 지금 원인은 모른채 결과만을 가지고 분개하고 있다는 생각을 안하십니까? 좀 더 깊이 알아보고 햏동하는 것이 진정한 용기라고 봅니다.");
            } else Print(7, " 당신이 트롤족을 멸한 것은 후세에 오점으로 남을 것입니다.");
        }
        if (at(36, 21)) {
            if (!getbit(5)) Print(7, " 이쉬도 테라에는 아직 인류의 문명이 뻗치지 못했습니다. 거기에는 코볼트족이 지배하고 있기 때문에 아무도 접근하지 못했던 것입니다. 가기만하면 번번히 쫒겨 나오곤 하니까요.");
            else Print(7, " 당신 덕분에 이제는 이쉬도 테라에도 인간의 문명을 뻗칠수 있게 되었습니다.");
        }
    }

    // ==========================================
    // Gaea (가이아 테라 성)
    // ==========================================
    if (mapname == "Gaea" || mapname == "GAEA") {
        if (at(37, 39) || at(40, 37) || at(41, 41)) { Grocery(); return; }
        if (at(37, 10) || at(40, 12) || at(41, 15)) { Weapon_Shop(); return; }
        if (at( 9, 39) || at(12, 41)) { Hospital(); return; }
        if (at(12, 15) || at(15, 12)) { About_Class(); return; }
        if (at(12, 11)) { Sell_EXP(); return; }

        if (at(25, 9)) {
            std::string tempStr = std::string(" 당신이 ") + std::string(player[1].name) + "라는 사람이오?";
            Print(7, tempStr);
            Print(7, " 만나서 정말 반갑소. 나는 이곳의 전투승인");
            cPrint(7, 11, "", "데네볼라", "라고하오. 사자를 뜻하는 나의 이름에서 알 수 있듯이 나는 맨손 전투에서만은 어느 누구에게도 지지 않소.");
            Print(7, " 나는 이곳에서 이쉬도 테라의 코볼트족을 정벌하고 신대륙을 개척할 일행을 찾고 있었소. 내가 당신의 일행에 참가하려는데 어떻게 생각하오?");
            m[1] = "쾌히 승락하겠소"; m[2] = "하지만 전투승은 필요없소";
            if (Select(200, 2, 2, false, true) == 1) {
                int k = 0; for(int i = 1; i <= 6; i++) { if(player[i].name_len == 0) { k = i; break; } }
                if (k > 0 && k < 6) {
                    Lore& p = player[k];
                    p.name_len = std::strlen("데네볼라"); std::strcpy(p.name, "데네볼라");
                    p.sex = male; p._class = 5; p.classtype = sword; p.level = 10;
                    p.strength = 20; p.mentality = 5; p.concentration = 6;
                    p.endurance = 18; p.resistance = 12; p.agility = 15;
                    p.accuracy = 17; p.luck = 10; p.hp = p.endurance * p.level * 10; p.sp = 0;
                    p.potential_experience = ReturnEXP(p.level);
                    p.weapon = 0; p.shield = 0; p.armor = 3; p.potential_ac = 3;
                    p.sword_data.sword_skill = 0; p.sword_data.axe_skill = 0; p.sword_data.spear_skill = 0;
                    p.sword_data.bow_skill = 0; p.sword_data.shield_skill = 0; p.sword_data.fist_skill = 60;
                    Modify_Item(k); PutMap(25, 9, 44); setbit(43);
                    Silent_Scroll(); Display_Condition(); Clear();
                } else Print(7, " 벌써 사람이 모두 채워져 있군요. 다음 기회를 기다리지요.");
            } else asyouwish();
        }

        if (at(11, 36)) {
            Print(7, " 저는 이곳에서 초급마법을 배우는 카펠라라고 합니다.");
            Print(7, " 사실 메이지로서의 제 능력은 아직 많이 부족합니다. 그래서 당신들과 함께 실전에서 많은 것들을 배우고 싶습니다. 제가 일행에 끼어도 되겠습니까?");
            m[1] = "그렇다면 한번 동행해 보겠소"; m[2] = "당신은 아직 미숙하니 곤란하오";
            if (Select(150, 2, 2, false, true) == 1) {
                int k = 0; for(int i = 1; i <= 6; i++) { if(player[i].name_len == 0) { k = i; break; } }
                if (k > 0 && k < 6) {
                    Lore& p = player[k];
                    p.name_len = std::strlen("카펠라"); std::strcpy(p.name, "카펠라");
                    p.sex = female; p._class = 1; p.classtype = magic; p.level = 2;
                    p.strength = 5; p.mentality = 15; p.concentration = 20;
                    p.endurance = 10; p.resistance = 19; p.agility = 11;
                    p.accuracy = 17; p.luck = 15; p.hp = p.endurance * p.level * 10; p.sp = 0;
                    p.potential_experience = ReturnEXP(p.level);
                    p.weapon = 0; p.shield = 0; p.armor = 0; p.potential_ac = 0;
                    p.sword_data.sword_skill = 10; p.sword_data.axe_skill = 10; p.sword_data.spear_skill = 10;
                    p.sword_data.bow_skill = 10; p.sword_data.shield_skill = 10; p.sword_data.fist_skill = 0;
                    Modify_Item(k); PutMap(11, 36, 44); setbit(44);
                    Silent_Scroll(); Display_Condition(); Clear();
                } else Print(7, " 벌써 사람이 모두 채워져 있군요. 다음 기회를 기다리겠습니다.");
            } else {
                Print(7, " 그렇다면 할 수 없군요. 제 스승인 안타레스 Jr.님께서 로어성에 가신후 좋은 스승을 못 찾았기에 당신에게 부탁 드렸던건데...");
            }
        }

        if (at(42, 25)) {
            if (getbit(3) && getbit(4)) {
                Print(7, " 당신이 원한다면 우리 성의 북쪽에 있는 이쉬도 테라로 가는 게이트 입구를 열어 주겠소.");
                if (!getbit(5)) Print(7, " 거기에는 코볼트족만이 살고 있으니 조심하시오.");
            } else {
                switch(party.etc[21]) {
                    case 0: {
                        std::string tmp = std::string(" 역시 와 주었군요, ") + std::string(player[1].name) + ".";
                        Print(7, tmp);
                        Print(7, " 당신이 지금 베스퍼성에 원정을 가고 있는 중이란 건 알고 있소. 하지만 나의 부탁을 하나 들어 주지 않겠소?");
                        Print(7, " 지금 우리가 있는 가이아 테라에는 인류와 오크족이 공존하고 있소. 오크족들은 우리 대륙을 횡단하는 여행자들에게 약간의 피해를 주기는 했지만 그리 문제 될 것까지는 없었소.");
                        Print(7, " 하지만 베스퍼성의 참사를 듣고나니 우리성도 오크족에 대해서는 예외가 아니란걸 알았소. 우리 성에서도 몇 번은 그들을 정벌하자는 의견이 나왔지만 그들의 해악성이 별로 크지 않은데다가 그들의 전염병이 우리에게 옮을수 있다는 생각에서 모두들 기피해 왔던게 사실이오.");
                        Print(7, " 하지만 사태가 사태이니만큼 한시 바삐 그들을 정벌해야만 우리가 편안히 지낼수 있겠소.");
                        Print(7, " 오크족을 정벌해 주지 않겠소? 만약 당신이 허락한다면 당신에게 이 성의 보물창고를 개방하리다. 이곳 주민을 대표하여 당신에게 부탁하겠소.");
                        Print(7, "");
                        PressAnyKey();
                        party.etc[21]++;
                        break;
                    }
                    case 1:
                        Print(7, " 오크족의 오크킹만 제거한다면 그들의 사회는 붕괴 될거요. 그리고 오크족의 마을은 이 성의 동쪽 멀리에 있소. 그럼 부탁하오.");
                        break;
                    case 2:
                        Print(7, " 당신이라면 해낼줄 알았소. 그 답례로 보물창고를 당신에게 개방하겠소. 거기서 마음껏 필요한 것을 가져 가도록하시오.");
                        party.etc[21]++;
                        break;
                    case 3:
                        Print(7, " 베스퍼성이 있는 아프로디테 테라로 가는 게이트가 대륙의 북동쪽 산악지역에 있소. 부디 당신이 성공하기를 바라오.");
                        break;
                }
            }
        }

        if (at(37, 27)) Print(7, " 오크족은 강한자가 왕이되는 전통을 가지고 있습니다.");
        if (at(38, 14)) Print(7, " 오크족에서 제대로 마법을 행하는 자는 오크킹의 왼팔격인 이키메이지뿐입니다.");
        if (at(40, 24) || at(40, 27)) Print(7, " 오크들은 정말 추하고 더럽고 비위생적인 종족입니다.");
        if (at(37, 24)) Print(7, " 저는 오크족에 대해 수 년동안 정보수집을 했었습니다. 제가 알게된 바로는, 오크의 보스는 오크킹이며 그 밑에 오크 아키몽크와 오크 아키메이지가 있습니다. 또 아키메이지 밑에는 그의 마법제자인 오크 매직유저가 7 명 있으며 아키몽크 밑에는 4 명의 오크 전사와 수 십 또는 수 백명의 오크 병사가 있습니다.");
        if (at(42, 34)) Print(7, " 드라코니안족은 인간과 드래곤의 트기라더군요. 인간처럼 두 팔과 두 다리가 있으나 머리는 드래곤의 머리이며 날개와 꼬리가 있고 지능 또한 상당히 높다고 하더군요.");
        if (at(27, 15)) {
            if (!getbit(5)) Print(7, " 코볼트족의 배타성 때문에 인류의 신세계 개척이 늦어지고 있습니다. 그들을 굴복 시켜 한시 바삐 새 시대를 열어야 합니다.");
            else Print(7, " 이제는 신세계 개척에 힘써야 할 때입니다.");
        }
        if (at(14, 38)) Print(7, " 저는 코볼트족이 지배하는 이쉬도 테라에 갔다가 겨우 도망쳐 나왔습니다. 그들은 외부 종족이 그들의 영역 내에 들어오는 것을 매우 싫어하더군요. 그들의 무장은 다른 종족보다 강하여 도저히 당해내기 힘들었습니다.");
        if (at(24, 29)) {
            if (party.etc[5] == 0 || getbit(8)) {
                Print(7, " 당신이 오크의 말과 글을 배운다면 그들과 대화가 이루어 질거요.");
            } else {
                Print(7, " 만약 당신이 나에게 금을 2000개 준다면 기꺼이 일주일만에 오크의 글을 가르쳐 주리다.");
                m[1] = "그렇게 합시다"; m[2] = "배울 필요는 없을것 같소";
                int k = Select(100, 2, 2, false, true);
                if (k == 1) {
                    if (party.gold < 2000) {
                        Print(7, " 하지만 당신에게는 충분한 금이 없군요.");
                        return;
                    }
                    party.gold -= 2000;
                    Screen_Fade_Out(200);
                    party.day += 7;
                    party.hour = 11;
                    party.min = 0;
                    PlusTime(0, 0, 1);
                    Print(7, " 일주일이 경과했다.");
                    PressAnyKey();
                    OverLap(150, true);
                    Print(7, " 당신은 이제 오크족의 글을 읽을 수가 있소.");
                    Print(7, " 만약 당신이 오크족의 말도 배우고 싶다면 이 대륙의 남동쪽 섬으로 가시오.");
                    Print(7, " 그리고, 다른 세 종족의 말과 글을 배울 수있는 길이 반드시 있을거요.");
                    setbit(8);
                }
            }
        }
    }

    // ==========================================
    // Ancient (에인션트)
    // ==========================================
    if (mapname == "Ancient" || mapname == "ANCIENT") {
        if (at(10, 17) || at(11, 17)) {
            Clear();
            Print(7, " 신전 입구인듯한 곳에 신자인것 같은 드라코니안 두명이 서 있었다. 당신들을 보고 뭐라고 알 수 없는 말을 서로 지껄이더니 서투른 인간의 말로 당신에게 말했다.");
            Print(7, "");
            Print(13, " 여기는 에인션트 이블님의 신전을 모시고 있는 신전입니다. 그리고 우리 둘은 이곳을 관리하는 신도들입니다. 어떤 일로 여기에 오셨습니까?");
            m[1] = "당신들을 죽이러 왔소"; m[2] = "에인션트 이블을 만나러 왔소";
            int k = Select(220, 2, 2, false, true);
            if (k == 1) {
                enemynumber = 2;
                for(int i = 1; i <= 2; i++) {
                    joinenemy(i, 48);
                    enemy[i].name = "드라코니안 신도";
                }
                DisplayEnemies(true);
                BattleMode(true, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { Silent_Scroll(); return; }
                for (int i = 10; i <= 11; i++) PutMap(i, 17, 44);
                Scroll(true);
                setbit(28);
            } else {
                Print(13, " 하지만 그분은 벌써 몇 백년 전에 돌아가셨습니다. 지금은 그분의 말씀만 남아 우리들을 올바르게 지도하고 계십니다.");
            }
        }
    }

    // ==========================================
    // Hut (움막)
    // ==========================================
    if (mapname == "Hut" || mapname == "HUT") {
        if (at(15, 9)) {
            if (getbit(204) || getbit(205)) {
                Print(7, " 당신들이 가지고 있는 창과 갑옷을 어디선가 본 것 같은데...");
            } else if (getbit(12)) {
                Print(7, " 이제 당신은 웬만한 코볼트 글을 이해 할 수 있을 겁니다.");
            } else {
                Print(7, " 나는 어릴때부터 코볼트족에게 발견되어 그들의 손에 자라났습니다. 지금은 비록 인간이라는 이유로 그들의 집단에서 소외되어 이곳에서 홀로 살고 있지만 한때는 코볼트족의 말과 글을 사용하던 시절이 있습니다.");
                Print(7, " 당신 혹시 나에게 체력 회복약 5개만 주지 않겠습니까? 그런다면 나는 기꺼이 당신에게 코볼트의 글을 가르쳐 주겠습니다. 그것도 일주일만에 말입니다.");
                m[1] = "좋소, 그렇게 합시다"; m[2] = "체력 회복약 5개는 좀...";
                int k = Select(220, 2, 2, false, true);
                if (k == 1) {
                    if (party.item[1] >= 5) { // item[1] = 체력 회복약 (원작 파스칼 배열 기준 가정)
                        party.item[1] -= 5;
                        Screen_Fade_Out(300);
                        Print(7, " 일주일이 지났다.");
                        PressAnyKey();
                        setbit(12);
                        party.day += 7;
                        PlusTime(0, 0, 1);
                        OverLap(300, true);
                        Print(7, " 당신은 이제 코볼트 글을 알게 되었다.");
                    } else {
                        Print(7, " 하지만 당신에게는 체력 회복약 5개가 없다.");
                    }
                }
            }
        }
    }

    // ==========================================
    // Dome (돔 / 빛의 지붕)
    // ==========================================
    if (mapname == "Dome" || mapname == "DOME") {
        if (at(26, 14)) Print(7, " 저는 카미너스라는 그때 그 사람입니다. 우리는 이 마을을 '빛의 지붕'이라고 명명했습니다. '빛'은 정의를 뜻하고 '지붕'은 그것을 지키는 것 또는 지키는 사람이란 뜻입니다. 즉, 정의를 지키는 사람들의 마을이란 뜻입니다.");
        if (at(13, 27)) Print(7, " 당신은 우리들의 생명의 은인입니다. 또한 새로운 로드안이 되실 충분한 가능성을 갖고 계신분입니다.");
        if (at(22, 35)) Print(7, " 당신의 용기있는 결단에 정말 감격했습니다.");
        if (at(39, 29)) Print(7, " 당신과 에인션트 이블이 정말 진정한 선의 수호자입니다.");
        if (at(19, 54)) Print(7, " 당신은 비록 과거에 로드안의 심복으로써 큰 실수를 저질렀지만 이제는 명실공히 이런 난세의 구세주입니다.");
        if (at(30, 63)) Print(7, " 우리들은 투표로 이곳의 지도자를 선출했습니다. 그는 바로 우리들을 일어서게한 용감한 사람, 바로 카미너스씨입니다.");
        if (at(19, 73)) Print(7, " 이곳 사람들은 모두 당신에게 고마와하고 있고 당신이 우리가 못다한 일을 해줄거라 믿고 있습니다.");
        if (at(38, 41)) {
            if (!getbit(202)) {
                Print(7, " 당신의 용기에대한 나의 작은 정성이오. 자");
                cPrint(7, 11, "이걸 받으시오. 우리 조상 대대로 내려오던 ", "에", "");
                cPrint(7, 11, "", "보니 크리스탈", "이란거요.");
                party.crystal[4]++;
                setbit(202);
            } else {
                Print(7, " 상당히 귀한 물건이니 조심해서 간직하시오.");
            }
        }
    }
}

void TalkMode_Sub3() {
    Clear();

    // ==========================================
    // DracTown (드라코니안 타운)
    // ==========================================
    if (mapname == "DracTown" || mapname == "DRACTOWN") {
        int h = 0;
        
        // NPC Interactions & Check
        if (at(24, 98)) { h = 1; if (getbit(13)) { Print(7, " 아니! 인간이 여길 들어오다니..."); PressAnyKey(); } }
        if (at(24, 92)) { h = 2; if (getbit(13)) { Print(7, " 음? 너는... 호모 사피엔스 종족이로군. 이처럼 하등한 포유류가 감히 드라코니안 종족의 영역을 침법하다니 놀랍군."); PressAnyKey(); } }
        if (at(25, 81)) { h = 3; if (getbit(13)) { Print(7, " 그래... 너 잘만났다. 한번 죽어봐라."); PressAnyKey(); } }
        if (at(34, 100)){ h = 4; if (getbit(13)) { Print(7, " 당신이 우리를 정벌하러 왔다는 말을 들었는데 뭔가 잘못 안 것은 아니오? 여태껏 우리 종족과 당신들은 아무런 접촉도 없었는데 악의를 품은채 이곳으로 올 이유가 없지 않소. 어짜피 당신은 우리들의 상대가 안되니 여기서 썩 물러 나시오."); PressAnyKey(); } }
        if (at(36, 93)) { h = 5; if (getbit(13)) { Print(7, " 우리 드라코니안족은 드래곤과 인간의 장점만을 모아 진화한 생명체로써 드라콘이라고 줄여부르기도 하지. 지능은 IQ 200에서 250 정도로 인간의 두배 정도이고 키도 평균 2m 30cm로 인간보다 크고 게다가 날개도 있어서 하늘을 나는 것도 가능하네. 수명도 150 에서 200 세 정도라서 인간보다 발전 가능성도 뛰어나지."); Print(7, " 흐흐흐... 어때? 이래도 우리에게 대항하겠는가?"); PressAnyKey(); } }
        if (at(42, 84)) { h = 6; if (getbit(13)) { Print(7, " 우와~~ 말로만 듣던 인간을 직접 이렇게 보게되다니... 정말 신기하게 생겼네."); PressAnyKey(); } }
        if (at(33, 82)) { h = 7; if (getbit(13)) { Print(7, " 콩알만한 것들이 겁도 없이 들어 오다니. 심심하던 차에 잘됐군. 싸움이나 걸어야지."); PressAnyKey(); } }
        if (at(41, 70)) { h = 8; if (getbit(13)) { Print(7, " 쩝쩝, 배고파."); PressAnyKey(); } }
        if (at(32, 22)) { h = 9; if (getbit(13)) { Print(7, " 나는 이곳 최고의 물리학자일세. 그리고 나의 이름은 아인슈타인이라네. 내가 어릴때 머리가 완전히 돌이었지. 그래서 항상 아이들이 나를"); cPrint(7, 10, "보며 ", "\"Du bist ein Stein\"", " 이라고 놀리곤했지."); Print(7, "거기서 나온 이름이 내 이름인 'Einstein'일세"); PressAnyKey(); } }
        if (at(40, 33)) { h = 10; if (getbit(13)) { Print(7, " 만약, 당신이 자꾸 우리를 위협하겠다면 우리는 성의 서쪽 감옥에 있는 인간들을 모두 처형시켜 버릴테다."); PressAnyKey(); } }
        if (at(16, 19)) { h = 11; if (getbit(13)) { Print(7, " 나를 여기서 탈출 시켜 주시오. 그러면 우리가문의 보물을 드리리다."); PressAnyKey(); } }
        if (at(16, 35)) { h = 12; if (getbit(13)) { Print(7, " 나도 나쁜 놈이지만 당신은 더 나쁜 놈이오. 왜 그렇냐고 물으면 웃지요."); Print(7, " 으하하하.. 낄낄.. 흐흐흐..."); PressAnyKey(); } }
        if (at(15, 48)) { h = 13; if (getbit(13)) { Print(7, " 위대한 프로그래머이시며 이 세계의 창시자이신 분과 타임워커 알비레오는 동일인물이라는 생각이 안드오?"); PressAnyKey(); } }

        if (h > 0) {
            if (h != 3 && h != 7) {
                Print(7, " 당신 앞에 있는 드라코니안족을 당신은 어떻게 할 것인가를 선택하시오.");
                m[1] = "죽여 버린다"; m[2] = "그냥 살려 준다";
                int k = Select(110, 2, 2, false, true);
                if (k != 1) return;
            }
            enemynumber = 1;
            joinenemy(1, 48);
            DisplayEnemies(true);
            BattleMode(true, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            PutMap(pas_x + pas_x1, pas_y + pas_y1, 44); // pas_x, pas_y, pas_x1, pas_y1 은 전역 좌표 관련 변수 가정
            Scroll(true);
            setbit(111 + (h - 1));
            return;
        }

        if (at(15, 57)) {
            Print(7, " 나는 드라콘족의 통치체제에 반발하다가 이곳에 반역죄로 잡혀오게 되었소. 당신 역시 이곳의 체제를 전복 시키려는게 주목적인것 같은데, 우리 힘을 합해 보는게 어떻겠소.");
            m[1] = "그렇게 하지요"; m[2] = "그건 안되겠소";
            if (Select(170, 2, 2, false, true) == 1) {
                join(48, 6);
                PutMap(15, 57, 44);
                Silent_Scroll();
                Display_Condition();
            } else asyouwish();
        }

        if (at(22, 24)) {
            Print(7, " 나는 마징가 Z 를 실제로 보았죠. 정말 무쇠팔, 무쇠다리, 로켓트주먹을 가지고 있더군요.");
            Print(7, " 만약 전쟁이 일어나면 인천 앞바다가 갈라지며 출동한다고 들었어요. 그것뿐만 아니죠. 잠실구장이 열리면서 메칸더 V 도 나오고 비룡폭포에서는 그렌다이져가 뉴크프리트와 합체하여 출격하고 육군본부에 보호망이 쳐지며 그레이트 마징가와 비너스 A 가 같이 나타나고 공군본부에서 출동한 독수리 5 형제가 조국의 창공을 지키게 되죠. 물론 김박사가 만든 태권 V도 최후의 희망으로 버티고 있죠. 뭐,믿기지 않는다면 국방부 장관님께 직접 물어 보시죠.");
            Print(7, "");
            PressAnyKey();
            Print(7, " 제 이름이 뭐냐구요 ? 저는 쇠돌이에요.");
        }

        if (at(24, 50)) {
            if (party.etc[5] == 0) {
                Print(7, " 그는 당신을 슬쩍본 후 다시 자기 할 일만 하고 있었다.");
            } else {
                Print(7, " 당신은 지금 로드안에게 속고 있소. 그는 표면적인 선을 행하기 위해서 당신을 이용하고 있는 것이오. 당신이 드라코니안족을 적대시할 이유가 없소. 그들은 인간에게 아무런 피해도 간섭도 없이 살아왔소. 그들이 에인션트 이블을 숭상한다는 것이 정벌의 이유가 될 수는 없소. 당신은 에인션트 이블을 진정으로 만나보기라도 했소? 당신은 위선자 로드안의 말만 믿고 에인션트 이블과 그의 추종자들을 배척하고 있는거란 말이오. 당신은 어릴때부터 에인션트 이블과 악은 나쁘다고 사상교육을 받아왔던걸 아시오? 진리를 모른채 단지 위에서 가르치는대로 배우며 그것을 진실로 받아들이고 머리속에 새겼던게 바로 화근이었던 것이오.");
                Print(7, "");
                PressAnyKey();
            }
        }

        if (at(67, 21)) {
            if (party.etc[5] == 0) {
                Print(7, " 그는 당신을 보더니 외면해 버렸다.");
            } else {
                switch (party.etc[24]) {
                    case 0:
                    case 1:
                        Print(7, " 그는 당신을 힐끗 보더니 외면해 버렸다.");
                        party.etc[24]++;
                        break;
                    case 2:
                        if (getbit(13)) {
                            Print(7, " 나는 드라코니안족의 대예언자요. 나에게는 당신의 미래가 보인다오. 그러나 그것은 너무나 무서운 것이기에 함부로 얘기할 수 없소.");
                            party.etc[24]++;
                        }
                        break;
                    case 3:
                        if (getbit(13)) {
                            Print(7, " 좋소, 애기해 주겠소. 당신은 곧 에인션트 이블님을 만나게 될거요. 그리고 로드안을 배신하려는 선택을 할지도 모르는 순간이 다가올 것이오. 그때의 선택은 당신의 결말을 크게 뒤흔들것이오.");
                            party.etc[24]++;
                        }
                        break;
                    default:
                        Print(7, " 그는 당신을 힐끗 보더니 외면해 버렸다.");
                        break;
                }
            }
        }

        // Fixed Encounters & Guards
        if (at(14, 105) || at(15, 106)) {
            if (getbit(13)) {
                Print(13, " 잠깐, 너희들은 누구냐? 이 안으로 들어갈 수 있는 것은 드라코니안족 뿐이다. 썩 물러나라.");
                m[1] = "적과 싸운다"; m[2] = "그냥 물러선다";
                if (Select(170, 2, 2, false, true) != 1) return;
            }
            enemynumber = 5;
            for (int i = 1; i <= 4; i++) joinenemy(i, 49);
            joinenemy(5, 51);
            DisplayEnemies(true);
            BattleMode(true, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            setbit(103);
            PutMap(14, 105, 44); PutMap(15, 106, 44);
            Silent_Scroll();
        }

        if (at(47, 61) || at(48, 61)) {
            if (getbit(13)) {
                Print(13, " 이곳은 너희같은 이방인은 절대 들어갈 수 없는 곳이다. 그래도 끝까지 들어가려 한다면 나와의 사생결단이 남았을뿐이다.");
                PressAnyKey();
            }
            enemynumber = 8;
            for (int i = 1; i <= 7; i++) joinenemy(i, 49);
            joinenemy(8, 52);
            DisplayEnemies(true);
            BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            setbit(102);
            for (int j = 59; j <= 61; j++)
                for (int i = 47; i <= 48; i++) PutMap(i, j, 44);
            Silent_Scroll();
        }

        if (at(27, 54)) {
            if (getbit(13)) {
                Print(13, " 오! 에인션트 이블님이시여! 그대의 이름으로 저들에게 패배를 명하소서.");
                PressAnyKey();
            }
            enemynumber = 8;
            for (int i = 1; i <= 4; i++) joinenemy(i, 49);
            for (int i = 5; i <= 6; i++) joinenemy(i, 50);
            joinenemy(7, 47); joinenemy(8, 53);
            DisplayEnemies(true);
            BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            setbit(101);
            for (int i = 24; i <= 27; i++) PutMap(i, 54, 44);
            Silent_Scroll();
        }

        if (at(84, 59)) {
            if (getbit(13)) {
                Print(13, " 내가 광란자라고 불리는 이유를 너희들에게 보여주지. 자, 맛 좀 봐랏!");
                PressAnyKey();
            }
            enemynumber = 8;
            for (int i = 1; i <= 7; i++) joinenemy(i, 53);
            joinenemy(8, 54);
            DisplayEnemies(true);
            BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            setbit(100);
            for (int j = 59; j <= 61; j++) PutMap(84, j, 44);
            Silent_Scroll();
        }

        if (at(79, 88)) {
            if (getbit(13)) {
                Print(13, " 나는 너희들과 같은 보물 약탈자들을 제거하기 위해 보물창고 앞을 지키고 있는 프로스트 드라코니안이다. 나의 냉기와 한파공격에는 아무 저항도 하지 못하고 나가 떨어질 것이다.");
                PressAnyKey();
            }
            enemynumber = 8;
            for (int i = 1; i <= 7; i++) joinenemy(i, 47);
            joinenemy(8, 55);
            DisplayEnemies(true);
            BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            setbit(99);
            for (int i = 79; i >= 77; i--) PutMap(i, 88, 44);
            Silent_Scroll();
        }

        if (at(59, 48)) {
            if (getbit(13)) {
                Print(13, " 멋모르고 날뛰고 있는 인간들아 들어라. 나는 성전사의 명예를 걸고 드라코니안족과 우리의 왕을 위해 싸울 것이다.");
                PressAnyKey();
            }
            enemynumber = 4;
            for (int i = 1; i <= 2; i++) joinenemy(i, 51);
            joinenemy(3, 56); joinenemy(4, 52);
            DisplayEnemies(true);
            BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            setbit(98);
            for (int i = 59; i <= 61; i++) PutMap(i, 48, 44);
            Silent_Scroll();
        }

        if (at(73, 40)) {
            if (getbit(13)) {
                Print(13, " 네가 더 이상 이 안으로 들어갈 수 없는 이유는 바로 내가 있기 때문이다. 내가 만든 나의 환상속에서 너는 스스로의 능력에 대한 비애를 느낄수 밖에 없을 것이다.");
                PressAnyKey();
            }
            enemynumber = 1;
            joinenemy(1, 57);
            DisplayEnemies(true);
            BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            setbit(97);
            for (int i = 73; i <= 75; i++) PutMap(i, 40, 44);
            Silent_Scroll();
        }

        if (at(94, 37)) {
            if (getbit(13)) {
                Print(13, " 나는 아키드라코니안이라고 하는 여기 최강의 용사이다. 또한 드라코니안킹의 방을 지키는 경호대장이다. 너같은 자들은 나의 능력으로 충분히 제거해 버릴수 있다.");
                PressAnyKey();
            }
            enemynumber = 8;
            for (int i = 1; i <= 4; i++) joinenemy(i, 53);
            for (int i = 5; i <= 7; i++) joinenemy(i, 51);
            joinenemy(8, 58);
            DisplayEnemies(true);
            BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }
            setbit(96);
            for (int i = 94; i <= 96; i++) PutMap(i, 37, 44);
            Silent_Scroll();
        }
        
        // Draconian King Boss Fight
        if (at(102, 21) && !getbit(95)) {
            Clear();
            Print(13, " 나는 네가 목표로하는 드라코니안족의 왕이다. 아마 이곳에 살아 남은 우리 종족의 전투사들은 거의 없을 것으로 안다. 보이다시피 지금 너의 앞에는 나와 가드 드라코니안 3명 밖에는 없다. 너는 이 기회를 놓치지 않고 반드시 나를 쓰러뜨려서 드라코니안 종족을 멸망시키려 할 것이란건 알고 있다. 하지만 지금 나의 편이 불리하다고 내가 진다는 것은 아니다. 나 역시 반드시 너희들을 패배 시켜서 우리 종족을 존속 시키려고 필사적이다.");
            Print(13, " 이제 나의 뜻을 알았을 것이라 믿는다. 그럼 이제 결전이다!");
            PressAnyKey();
            PutTile(5, 4, 44); PutTile(5, 3, 53);
            Delay(500);
            PutTile(4, 5, 44); PutTile(4, 6, 53);
            Delay(200);
            PutTile(6, 5, 44); PutTile(6, 6, 53);
            Delay(300);
            PutTile(5, 5, 42); PutSprite(5, 5, 12);
            Delay(1500);
            PutSprite(4, 5, 15); PutSprite(6, 5, 19);
            PutTile(5, 5, 42); PutSprite(5, 5, 17);
            PutSprite(5, 4, 16);
            PutSprite(4, 4, 14); PutSprite(6, 4, 18);
            PressAnyKey();
            
            enemynumber = 4;
            for (int i = 1; i <= 3; i++) joinenemy(i, 50);
            joinenemy(4, 59);
            
            do {
                DisplayEnemies(true);
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) Clear();
            } while (party.etc[6] != 0);
            
            setbit(95);
            PutMap(102, 20, 44); PutMap(102, 21, 42);
            PutMap(101, 21, 44); PutMap(103, 21, 44);
            Silent_Scroll();
            Clear();
            Print(7, " 당신은 마지막 적인 드라코니안족의 왕을 처치했다. 기쁨을 감추지 못한채 돌아 나오려고 할때 갑자기 난데없는 암흑이 성 전체를 뒤덮기 시작했다. 그와 동시에 정체불명의 목소리가 어디에선가 들려왔다.");
            Print(7, "");
            Print(13, " 나는 드라코니안족의 수호령이다. 너희들은 너무나 큰 실수를 나에게 저질렀다. 나는 죽어간 드라코니안족의 복수를 위해 너희들을 이 세상에서 소멸시켜 버리려한다. 너희들은 나의 상대가 전혀되지 않는다. 이쯤에서 작별인사나 해두는게 좋을 것이다.");
            Print(7, "");
            PressAnyKey();
            
            enemynumber = 1;
            joinenemy(1, 59);
            enemy[1].name = "드라콘 수호령";
            enemy[1].specialcastlevel |= 0x80;
            
            do {
                DisplayEnemies(true);
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) Clear();
            } while (party.etc[6] != 0);
            
            setbit(6);
            Clear();
            Print(15, " 당신은 드라코니안족을 멸망 시켰다.");
        }
    }

    // ==========================================
    // Imperium (임페리엄 성)
    // ==========================================
    if (mapname == "Imperium" || mapname == "IMPERIUM") {
        if (at(9, 7)) Print(7, " 예전에는 로어성 주민과도 교역 했었다고 하는데 몇 백년 전부터는 무슨 이유인지는 몰라도 왕래가 완전히 끊겨 버렸어요.");
        if (at(8, 35)) Print(7, " 이 성의 중앙에는 에인션트 이블님의 의지가 잠들어 있습니다.");
        if (at(45, 35)) Print(7, " 이곳의 도서관은 색다른 내용의 보고라오.");
        if (at(24, 39)) Print(7, " 당신은 못 보던 사람이군요. 아마 여행자인것 같은데 우리 성에 온 것을 환영하오.");
        if (at(26, 39)) Print(7, " 여기는 에인션트 이블님의 자비로써 세워진 '임페리엄 마이너성'입니다.");
        if (at(16, 28) || at(18, 27)) { Grocery(); }
        if (at(32, 27) || at(34, 29)) { Hospital(); }
        
        if (at(18, 18)) {
            PutTile(5 + pas_x1, 6 + pas_y1, 47);
            PutSprite(5 + pas_x1, 6 + pas_y1, 24);
            std::string tempStr = std::string(" 안녕하시오, ") + std::string(player[1].name) + ".";
            Print(7, tempStr);
            Print(7, " 나는 나의 게임속의 버그를 찾거나 난이도를 조절하기 위해서 항상 게임속을 떠도는 이 게");
            cPrint(7, 14, "임의 제작자 ", "안 영기", "라는 사람이오. 때로는 로어성의 병사로 때로는 다른 종족의 주민으로 계속 변장하며 이곳 저곳을 떠돌아 다닌다오.");
            Print(7, " 당신은 벌써 이 게임의 반 이상을 했군요. 나는 당신에게 많은 것을 바라지는 않는다오. 다만, 내가 이 게임을 통해 말하고자하는 현실적인 주제를 알아 주었으면 하는 바램이 있을뿐이라오.");
            PressAnyKey();
            Silent_Scroll();
        }
        
        if (at(16, 20)) Print(7, " 안 영기님이 그러시던데 '또 다른 지식의 성전'은 원래 멀티 엔딩이 아니었고 '다크 메이지 실리안 카미너스'는 군입대 시간이 급박해서 멀티 엔딩을 만들지 못했다고 하는데 이번 게임은 확실한 멀티 엔딩이라더군요.");
        if (at(33, 18)) Print(7, " 저는 예언자로서의 능력을 키우는 중입니다. 저의 예언 능력은 아직 미숙하지만 당신의 미래가 조금은 옅보입니다. 당신은 머지않아 중요한 선택의 순간을 맞게 될 것입니다. 그때의 선택에 따라 당신은 큰 운명의 산맥을 넘어야 할 것입니다.");
        if (at(33, 20)) Print(7, " 이곳에는 안 영기님이 만든 게임들이 보관되어 있습니다. 첫번째로 만든 '3D 건담'이라는 시뮬레이션 게임의 APPLE SOFT BASIC 과 65C02 기계어 소스 프로그램부터 13번째 게임인 '또 다른 지식의 성전'과 16번째 게임인 '다크 메이지 실리안 카미너스', 또 17번째 게임인 '비전 속으로'와 같은 게임이 소스와 함께 보관되어 있습니다.");
        if (at(22, 17) || at(28, 18)) Print(7, " 죄송합니다. 프로그래밍 시간 관계상 제작자가 저희 가게를 프로그래밍 하지 못했군요. 그래서 저희 가게는 운영되지 않습니다.");
        if (at(24, 29)) Print(7, " 이 안은 '비전의 지식'이라는 신전이오. 에인션트 이블님의 의지가 잠든 곳이기도 하지요.");
        if (at(26, 29)) Print(7, " 이 안의 두 사람은 에인션트 이블님의 의지를 깨닳은 사람들이지요. 당신에게 결코 거짓말 따위는 하지 않을 거예요.");
        
        if (at(22, 24)) {
            Print(7, "에인션트 이블님은 예전에 아주 덕이 높은 현자였었지요. 또한 로드안과도 무척 친한 사이였어요. 수백년전에 로드안이 선이라는 전제를 걸고 이 세계를 통치 했었지요. 로드안의 훌륭한 정치에 모든 대륙은 평화를 누리게 되었고 그게 너무 만연된 나머지 현재의 평화를 아주 당연시 해버리고는 그것을 지키려는 생각은 접어둔채 항상 남에 의해 평화를 보장 받으려는 사상이 팽배해졌죠.");
            Print(7, "후손들은 급기야 자신의 조상이 흘린 피의 보답으로 이렇게 평화로운 세상이 생겨났다는 것을 까맣게 잊어버리기 시작했고 마침내 로드안은 그런 사람들에게 경각심을 부추겨 보자는 의도로 새로운 개념을 도입했죠. 그것이 바로 그대들이 '악'이라고 배운 것들이예요.");
            Print(7, "그때 로드안의 추상적인 '악'의 개념을 구체적인 '악'으로 만들기 위해 그 상징이 되어줄 사람을 수소문했죠. 로드안의 친구 대부분은 그의 뜻에 찬성헀지만 아무도 자신이 '악'을 대표하는 그런 역을 맡아 남들로부터 비난받고 저주받는 역을 하려고는 하지 않았죠.");
            Print(7, "그때 나타난 사람이 바로 에인션트 이블님이예요. 그분은 여태껏 지녔던 훌륭한 명성들을 버려둔채 악의 집대성으로 군림했지요. 본명 또한 버리고 지금의 이름을 사용하며 로드안의 일에 적극 동참했어요. 그의 존재 때문에 로어 세계의 사람들은 악으로부터 자신과 가족을 지키기 위해 스스로 선을 자각하고 현재의 평화에 감사하며 그 평화를 유지하려 노력하는 자세를 가지게 된거죠.");
            Print(7, "로드안은 사람들의 경각심을 더 부추기기 위해 지금 에인션트 이블이 여러 곳에서 이런 저런 구체적인 악행을 일삼고 있다고 거짓으로 소문을 퍼트리기 시작했고 어릴때부터 교육과정에 그런 사상을 심어 넣어 어른이 되어서도 선을 지켜야 한다는 개념이 없어지지 않도록 해왔어요.");
            Print(7, "");
            PressAnyKey();
        }
        
        if (at(27, 26)) {
            Print(7, "로드안은 선에 대한 너무 강한 집착 때문에 중대한 실수를 저질렀죠. 바로 당신을 시켜서 오크, 트롤, 코볼트, 드라코니안 이 네 종족을 멸망시킨게 그 실수예요. 로드안은 인간을 이롭게 하고자 아프로디테 테라의 트롤족을 몰아내고 그곳에 베스퍼성이란 전초기지를 건설했죠. 그곳은 인간에게 대단히 유용한 자원을 조달했고 그로인해 로드안의 평은 상당히 높아졌어요.");
            Print(7, "그리고 최근에는 다시 마지막 남은 트롤성을 공격하여 아프로디테 테라를 완전히 인간의 것으로 만들자는 계획을 세우고 베스퍼성의 주민들에게 명령했죠. 베스퍼성 사람들은 헤로인이란 마약을 트롤성으로 반입해서 트롤주민들에게 팔았어요. 즉, 수입도 올리고 트롤족을 마약 중독자로 만들어 마약 공급원인 베스퍼성 주민들에게 복종하게 하려는 의도였어요.");
            Print(7, "또한 베스퍼성의 강경파 세력은 수시로 트롤족의 동굴에 침입하여 그들을 괴롭히기 시작했고 마침내 대대적인 침략을 시작했지요. 트롤성을 침략한 이들은 정말 잔혹한 수법으로 트롤의 희생자를 처리하여 그들의 사기를 떨어뜨리려 했지만 결국 효과는 반대로 나타나서 더욱 더 트롤족을 분노에 떨게 했죠. 그리고는 도리어 그들에게 야습을 당해서 베스퍼성은 삽시간에 아수라장이 되고 급기야 모든 주민이 희생을 당하게 되었던 거예요.");
            Print(7, "이 일이 있은 후 로드안은 로어성의 주민들에게 편파적으로 소식을 알렸죠. 자신의 잘못은 은폐한채 베스퍼성이 당한 결과만을 바탕으로 트롤족에 대한 적개심을 부추겼죠. 그리고 그것을 다른 종족에 까지 확산시켜 지금에까지 이르렀던 거예요. 당신이 한 일 때문에 인간은 로어 세계에 있는 5개의 대륙 모두를 손에 넣게 되었죠. 악에 대한 정벌이란 거창한 목표 뒤에는 인간의 영토를 넓히겠다는 정복자의 야욕이 숨어있엇다는 것을 알아야 될거예요. 그 야욕 때문에 죄없는 수많은 타종족 주민들이 악인으로 몰려 비참한 최후를 맞이하게된 것을 당신도 인정하지요?");
            Print(7, "인간의 입장에서 생각할 때는 물론 당신은 영웅이죠. 하지만 모든 종족의 관점에서 본 당신은 비정한 정복자의 꼭두각시일뿐이예요. 나의 말을 깊이 새기고, 머지않은 미래에 있을 중요한 선택에 현명한 판단을 하기 바라겠어요.");
            Print(7, "");
            PressAnyKey();
        }
    }
}