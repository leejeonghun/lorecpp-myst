#include "mysttal1.h"
#include "mystsub.h"
#include "uhanx.h"
#include "mystbatt.h"
#include "mystend.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>

void TalkMode_Sub2() {
    Clear();

    // ==========================================
    // 1. Menace (메니스)
    // ==========================================
    if (mapname == "Menace" || mapname == "MENACE") {
        if (at(27, 32)) {
            // 이름 변환 처리가 포함된 문자열 생성
            std::string player_name = player[1].name;
            Print(7, " 오, " + player_name + ".");
            Print(7, " 오늘 일도 다 끝냈군요. 자, 여기 일당 받으시오.");
            Print(7, "");
            Print(11, " [ 황금 + 4 ]");
            party.gold += 4;
            
            int j_val = 43;
            for (int i_idx = 33; i_idx <= 38; i_idx++) {
                PutMap(27, i_idx - 1, j_val);
                j_val = Real_Map(27, i_idx);
                PutMap(27, i_idx, 21);
                Delay(1000);
                Silent_Scroll();
            }
            PutMap(27, 38, j_val);
        }
        if (at(24, 34)) {
            Print(7, " 당신이 여기 나타나다니 정말 뻔뻔스럽군.");
            Print(7, "사람을 죽여 놓고도 태연한척 하다니...");
        }
        if (at(35, 41)) {
            Print(7, " 몇 개월 전의 사건은 잊어버리게. 이곳에 있");
            Print(7, "는 대부분의 광부들은 자네가 살인을 했다고");
            Print(7, "는 믿지 않네. 단지 자네가 약간 운이 나빴던");
            Print(7, "거지");
        }
        if (at(15, 36)) {
            Print(7, " 내가 전해들은 전설에 의하면 로어 대륙 지");
            Print(7, "하에 하데스 테라라는 지하 대륙이 있다고 들");
            Print(7, "었다네. 하지만 전설인데 뭐.");
        }
        if (at(27, 26)) {
            Print(7, " 자네 이것 좀 봐. 이건 내가 오래 전에 광산");
            Print(7, "일을 하면서 발견한 건데 이상한 점이 있어.");
            Print(7, "이건 어떤 형식의 표음 문자가 적힌 석판인데");
            Print(7, "공교롭게도 30만년 전 지층에서 나왔다네. 그");
            Print(7, "리고 이건 더 오래된 지층에서 나온 공룡의");
            Print(7, "머리뼈인데 마법으로 관통 당한 구멍이 선명");
            Print(7, "하게 나타나 있다네. 공룡과 인류는 서로 다");
            Print(7, "른 시대를 살았는데 이건 정말 이상하지 않은");
            Print(7, "가 ?");
        }
    }

    // ==========================================
    // 2. OrcTown (오크 타운)
    // ==========================================
    if (mapname == "OrcTown" || mapname == "ORCTOWN") {
        int h_val = 0;
        if (at(10, 39)) {
            h_val = 1;
            if (getbit(7)) {
                Print(7, " 제발 살려 주십시요. 저는 여태껏 남에게 해");
                Print(7, "한 번 안입히고 살아왔습니다.");
            } else {
                Print(7, " 당신 앞의 오크 주민이 무어라 당신에게 애원");
                Print(7, "하는듯 했지만 알아들을 수는 없었다.");
            }
            PressAnyKey();
        }
        if (at(9, 19)) {
            h_val = 2;
            if (getbit(7)) {
                Print(7, " 우리는 당신네 인간들에게 아무런 나쁜짓도");
                Print(7, "안했는데 왜 당신들은 우리를 못 살게 구는 겁");
                Print(7, "니까 ?");
                PressAnyKey();
            }
        }
        if (at(13, 12)) {
            h_val = 3;
            if (getbit(7)) {
                Print(7, " 나는 이곳에서 50년 가까이 농사만 지으며 살");
                Print(7, "아왔소. 이곳 밖으로 나가본 적도 거의 없고,");
                Print(7, "더우기 인간에게 밑보일 짓도 하지 않았소. 그");
                Print(7, "런데 왜 당신들은 우리 마을을 짓밟고 우리 동");
                Print(7, "족을 죽이려하오?");
            } else {
                Print(7, " 당신 앞에 있는 늙은 오크가 당신에게 무어라");
                Print(7, "질책하는듯 하였으나 도무지 말을 알아들을 수");
                Print(7, "가 없었다.");
            }
            PressAnyKey();
        }
        if (at(21, 9)) {
            h_val = 4;
            if (getbit(7)) {
                Print(7, " 분명 오크족 중에서도 나쁜자가 있어서 지나");
                Print(7, "가는 여행자들을 위협하기도 했던게 사실이오.");
                Print(7, "하지만 대부분은 선량한 오크들이란걸 알아두");
                Print(7, "시오.");
                PressAnyKey();
            }
        }
        if (at(43, 12)) {
            h_val = 5;
            if (getbit(7)) {
                Print(7, " 당신네들이 이렇게 우리들을 못살게 군다면");
                Print(7, "오크킹님이 가만 있지 않을 것입니다.");
                PressAnyKey();
            }
        }
        if (at(44, 30)) {
            h_val = 6;
            if (getbit(7)) {
                Print(7, " 우리들은 에인션트 이블님을 정신적인 지도자");
                Print(7, "로 생각하고 있습니다.");
                PressAnyKey();
            }
        }
        if (at(35, 29)) {
            h_val = 7;
            if (getbit(7)) {
                Print(7, " 로드안은 이 세계의 적입니다. 그리고 그를");
                Print(7, "따르는 당신도 적입니다.");
            } else {
                Print(7, " 당신앞에 있는 오크 청년은 당장이라도 달려");
                Print(7, "들듯이 당신에게 무어라 소리쳐댔다.");
            }
            PressAnyKey();
        }
        if (at(41, 39)) {
            h_val = 8;
            if (getbit(7)) {
                Print(7, " 우리 부족의 아키메이지는 코볼트족의 왕에게");
                Print(7, "서 마법을 배웠습니다. 그라면 충분히 당신을");
                Print(7, "내 쫒을 겁니다.");
                PressAnyKey();
            }
        }

        if (h_val > 0) {
            Print(7, " 당신 앞에 있는 오크족을 당신은 어떻게 할");
            Print(7, "것인가를 선택하시오.");
            m[0] = "";
            m[1] = "죽여 버린다";
            m[2] = "그냥 살려 준다";
            int k_val = Select(110, 2, 2, false, true);
            if (k_val == 1) {
                enemynumber = 1;
                joinenemy(1, 18);
                DisplayEnemies(true);
                BattleMode(true, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { Silent_Scroll(); return; }
                
                PutMap(pas_x + pas_x1, pas_y + pas_y1, 34);
                Scroll(true);
                setbit(57 + (h_val - 1));
            }
            return;
        }

        if (at(25, 42) || at(25, 43)) {
            Print(7, " 마을의 문 앞을 지키고 있는 오크가 당신에게");
            Print(7, "인간의 말로 이렇게 말했다.");
            Print(7, "");
            Print(7, "");
            Print(13, " 인간은 오크 마을에 들어 올 수 없소.");
            Print(13, " 썩 물러나시오 !!");
            m[0] = "";
            m[1] = "알았소, 나가면 될거 아니오";
            m[2] = "당신들을 쓰러뜨리고 지나가야겠소";
            int k_val = Select(180, 2, 2, false, true);
            if (k_val == 2) {
                enemynumber = 7;
                for (int i_idx = 1; i_idx <= 7; i_idx++) joinenemy(i_idx, 19);
                DisplayEnemies(true);
                BattleMode(true, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { Silent_Scroll(); return; }

                PutMap(25, 42, 43); PutMap(25, 43, 43);
                setbit(53);
            }
        }

        if (at(24, 20) || at(26, 20)) {
            enemynumber = 6;
            for (int i_idx = 1; i_idx <= 3; i_idx++) joinenemy(i_idx, 19);
            joinenemy(4, 20);
            for (int i_idx = 5; i_idx <= 6; i_idx++) joinenemy(i_idx, 21);
            DisplayEnemies(true);
            if (getbit(7)) {
                Print(13, " 인간은 절대 이곳으로 들여 보내지 않겠다.");
                Print(13, "자! 덤벼라.");
            } else {
                Print(7, " 무장을 제법 갖춘 2명의 오크 전사와 다른 전");
                Print(7, "투사들이 당신을 공격하기 시작했다.");
            }
            PressAnyKey();
            BattleMode(false, true);
            if (party.etc[6] == 254) return;
            if (party.etc[6] == 2) { Silent_Scroll(); return; }

            PutMap(24, 20, 43); PutMap(26, 20, 43);
            setbit(54);
        }
        Scroll(true);
        return;
    }

    // ==========================================
    // 3. Vesper (베스퍼)
    // ==========================================
    if (mapname == "Vesper" || mapname == "VESPER") {
        if (Real_Map(pas_x + pas_x1, pas_y + pas_y1) == 53) {
            if (getbit(9)) {
                Print(13, " 아직도 남은 인간이 있었다니... 너도 나에게");
                Print(13, "죽어줘야겠다.");
            } else {
                Print(7, " 당신 앞의 트롤이 무어라 말했지만 알아 들을");
                Print(7, "수가 없었다.");
            }
            PressAnyKey();

            if (at(30, 19)) {
                enemynumber = 7;
                joinenemy(1, 28);
                for (int i_idx = 2; i_idx <= 7; i_idx++) joinenemy(i_idx, 27);
                DisplayEnemies(true);
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) {
                    pas_x = Map_Header.startX; pas_y = Map_Header.startY;
                    Silent_Scroll(); return;
                }
                Clear();
                Print(7, " 당신이 적들을 물리치자 동굴속에서 날렵하게");
                Print(7, "생긴 트롤이 걸어 나왔다.");
                Print(7, "");
                if (getbit(9)) {
                    Print(13, " 음... 나의 부하들을 물리치다니 제법 실력이");
                    Print(13, "있는 것같군. 하지만 네가 이 동굴에 들어온");
                    Print(13, "것은 정말 큰 불운이란걸 명심해라. 바로 베스");
                    Print(13, "퍼 점령대의 대장인 트롤 암살자님이 바로 나");
                    Print(13, "이니까.");
                } else {
                    Print(7, " 그리고는 도저히 알아들을수 없는 말을 몇 마");
                    Print(7, "디 지껄이더니 당신에게 달려 들었다.");
                }
                PressAnyKey();
                enemynumber = 8;
                for (int i_idx = 1; i_idx <= 2; i_idx++) joinenemy(i_idx, 28);
                for (int i_idx = 3; i_idx <= 6; i_idx++) joinenemy(i_idx, 27);
                joinenemy(7, 30);
                joinenemy(8, 29);
                DisplayEnemies(true);
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) {
                    pas_x = Map_Header.startX; pas_y = Map_Header.startY;
                    Silent_Scroll(); return;
                }
                Clear();
                Print(14, " 당신은 트롤 암살자를 처치했다.");
                Print(11, "[ 다크 크리스탈 + 1]");
                party.crystal[3]++;
                PutMap(30, 19, 43);
                setbit(65);
                setbit(1);
            }
            if (at(50, 19)) {
                enemynumber = 7;
                for (int i_idx = 1; i_idx <= 3; i_idx++) joinenemy(i_idx, 28);
                for (int i_idx = 4; i_idx <= 6; i_idx++) joinenemy(i_idx, 27);
                joinenemy(7, 29);
                DisplayEnemies(true);
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) {
                    pas_x = Map_Header.startX; pas_y = Map_Header.startY;
                    Silent_Scroll(); return;
                }
                Clear();
                Print(11, " [ 민첩성 + 1 ]");
                for (int i_idx = 1; i_idx <= 6; i_idx++) {
                    if (std::string(player[i_idx].name) != "") {
                        if (player[i_idx].agility < 20) player[i_idx].agility++;
                    }
                }
                PutMap(50, 19, 43);
                setbit(66);
            }
            if (at(64, 26)) {
                enemynumber = 7;
                joinenemy(1, 26);
                for (int i_idx = 2; i_idx <= 6; i_idx++) joinenemy(i_idx, 27);
                joinenemy(7, 28);
                DisplayEnemies(true);
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { pas_x = Map_Header.startX; pas_y = Map_Header.startY; Silent_Scroll(); return; }
                Clear();
                Print(11, " [ 금화 + 20000 ]");
                party.gold += 20000;
                PutMap(64, 26, 43);
                setbit(67);
            }
            if (at(61, 38)) {
                enemynumber = 6;
                for (int i_idx = 1; i_idx <= 4; i_idx++) joinenemy(i_idx, 27);
                for (int i_idx = 5; i_idx <= 6; i_idx++) joinenemy(i_idx, 26);
                DisplayEnemies(true);
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { pas_x = Map_Header.startX; pas_y = Map_Header.startY; Silent_Scroll(); return; }
                Clear();
                Print(11, " [ 식량 + 50 ]");
                if (party.food + 50 < 256) party.food += 50;
                else party.food = 255;
                PutMap(61, 38, 43);
                setbit(68);
            }
            if (at(40, 40)) {
                enemynumber = 7;
                for (int i_idx = 1; i_idx <= 5; i_idx++) joinenemy(i_idx, 26);
                for (int i_idx = 6; i_idx <= 7; i_idx++) joinenemy(i_idx, 28);
                DisplayEnemies(true);
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { pas_x = Map_Header.startX; pas_y = Map_Header.startY; Silent_Scroll(); return; }
                Clear();
                Print(11, " [ 곤봉 + 5 ]");
                if (party.BackPack.data[1][0] + 5 < 256) party.BackPack.data[1][0] += 5;
                else party.BackPack.data[1][0] = 255;
                PutMap(40, 40, 43);
                setbit(69);
            }
            if (at(22, 40)) {
                enemynumber = 7;
                for (int i_idx = 1; i_idx <= 6; i_idx++) joinenemy(i_idx, 27);
                joinenemy(7, 28);
                DisplayEnemies(true);
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { pas_x = Map_Header.startX; pas_y = Map_Header.startY; Silent_Scroll(); return; }
                Clear();
                Print(11, " [ 화살 + 30 ]");
                if (party.arrow + 30 < 65536) party.arrow += 30;
                else party.arrow = 65535;
                PutMap(22, 40, 43);
                setbit(70);
            }
            if (at(20, 26)) {
                enemynumber = 5;
                for (int i_idx = 1; i_idx <= 5; i_idx++) joinenemy(i_idx, 27);
                DisplayEnemies(true);
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { pas_x = Map_Header.startX; pas_y = Map_Header.startY; Silent_Scroll(); return; }
                Clear();
                Print(11, " [ 금화 + 1000 ]");
                party.gold += 1000;
                PutMap(20, 26, 43);
                setbit(71);
            }
            Silent_Scroll();
            return;
        } else {
            if (!at(47, 55)) {
                Print(7, " 당신 앞에는 형체도 알아보기 힘들 정도로 처");
                Print(7, "참히 살해된 인간의 시체가 있었다.");
            } else {
                Print(7, " 당신 앞의 시체는 다른 시체들보다 늦게 살해");
                Print(7, "당했는지 부패의 정도가 덜했다. 비교적 형태");
                Print(7, "를 알아 볼 수 있었기에 당신은 그의 몸을 조");
                Print(7, "사하기 시작했다.");
                Print(7, "");
                if (getbit(15)) {
                    Print(7, " 하지만 더 이상 발견되지 않았다.");
                    return;
                }
                Delay(1000);
                Print(7, " 곧이어 당신은 그 시체의 오른 손에 무언가가");
                Print(7, "꽉 쥐어져 있음을 알아차렸고 그것이 메세지를");
                Print(7, "전하기 위한 메모 쪽지라는 것을 알게 되었다.");
                Print(7, "당신은 그의 손을 펴보려 했지만 그의 몸은 빳");
                Print(7, "빳하게 굳어 있었고 좀처럼 펼 수가 없었다.");
                
                if (party.item[5] == 0) return; // 부활의 약초
                party.item[5]--;
                Delay(1500);
                Print(7, "");
                Print(7, " 문득 당신은 지금 가지고 있는 부활의 약초가");
                Print(7, "생각이 났다. 비록 지금의 그 시체는 살릴 수");
                Print(7, "없을 정도로 부패했기 때문에 살려내기는 불가");
                Print(7, "능했지만 최소한 죽은지 얼마안된 상태까지는");
                Print(7, "만들 수 있을거라 생각했다. 당신은 그 시체에");
                Print(7, "게 부활의 약초를 사용했고 예상대로 그의 몸");
                Print(7, "은 약간의 핏기가 돌게 되었고 어렵지않게 그");
                Print(7, "의 손에 쥐여진 종이 쪽지를 빼낼 수가 있었");
                Print(7, "다. 내용은 이러했다.");
                Print(7, "");
                PressAnyKey();
                Print(15, " 혹시나 이 글을 누가 읽기를 바라며 ...");
                Print(7, "");
                Print(15, " 트롤족에 의한 베스퍼성의 괴멸은 예견되었던");
                Print(15, "거나 다름없었다. 우리는 인간이기에 인간의");
                Print(15, "편을 들수밖에 없었고 또한 우리의 견해와 입");
                Print(15, "장으로써 이 사실을 다른 대륙으로 알렸다. 하");
                Print(15, "지만 그것은 잘못되었다는 것을 이내 알았다.");
                Print(15, "아마 내가 죽고 난 상황에서의 이곳은 인류와");
                Print(15, "트롤 모두가 자신들의 정의에 따라 서로를 저");
                Print(15, "주하며 대립하고 있을 것이다. 어떻게 보면 서");
                Print(15, "로의 이익을 찾기위해 명분을 세우고 침략을");
                Print(15, "정당화 시킨다고 할 수 있다. 우리는 인간이기");
                Print(15, "에 우리쪽의 잘못은 감추려고한다. 항상 다른");
                Print(15, "종족의 잘못을 들추어내며 우리를 더욱 값지게");
                Print(15, "해왔던게 사실이다. 이번의 경우도 그렇다. 책");
                Print(15, "임이 있다면 사실상 이번일은 인간의 책임이");
                Print(15, "더 크다. 자초지종을 말하자면 다음과 같다.");
                Print(15, "원래 아프로디테 테라는 트롤족의 땅이었다.");
                Print(15, "하지만 인간의 영역을 더욱 확보하려는 야심에");
                Print(15, "서 명분을 세우고 그들의 땅을 침범하여 성을");
                Print(15, "구축한 곳이 바로 이곳 베스퍼성이었다. 그리");
                Print(15, "고 그 일을 총지휘한 자는 바로 .............");
                Print(7, "");
                PressAnyKey();
                Print(7, " 당신이 여기까지 읽어내려 갔을때 그 종이 쪽");
                Print(7, "지는 산화하여 공중으로 흩어져 버렸다. 당신");
                Print(7, "은 지금 이 쪽지에서 말하려고 했던 자로부터");
                Print(7, "천리안으로 감시를 당하고 있다는 사실을 순간");
                Print(7, "깨닭았다. 당신이 알지 못하는 제 3의 인물이");
                Print(7, "있다는 확신이 강해짐에 따라 더욱 더 긴장 할");
                Print(7, "수 밖에 없었다. 하지만 더 이상의 변화는 일");
                Print(7, "어나지 않았다.");
                PressAnyKey();
                setbit(15);
            }
        }
    }

    // ==========================================
    // 4. TrolTown (트롤 타운)
    // ==========================================
    if (mapname == "TrolTown" || mapname == "TROLTOWN") {
        if (Real_Map(pas_x + pas_x1, pas_y + pas_y1) == 54) {
            if (at(25, 8) || at(26, 8)) {
                if (!getbit(16) && getbit(9)) {
                    Print(13, " 아니! 이곳까지 이방인이 쳐들어 오다니.... ");
                    Print(13, "나는 이곳의 왕인 트롤킹이라고 하는데 우리");
                    Print(13, "협상으로 해결하는게 어떻겠는가?");
                    m[0] = "";
                    m[1] = "그럼 조건부터 들어보겠소";
                    m[2] = "협상은 필요없소";
                    int k_val = Select(140, 2, 2, false, true);
                    if (k_val == 1) {
                        Print(13, " 당신이 무슨 목적으로 나의 방까지 쳐들어와");
                        Print(13, "서 나를 죽이려 하는지 모르겠지만 어쨌든 당");
                        Print(13, "신이 여기서 물러나 준다면 지금 이 상자 안에");
                        Print(13, "있는 금의 절반을 당신에게 주겠다. 이 정도면");
                        Print(13, "충분한가?");
                        m[0] = "";
                        m[1] = "좋소, 이걸 받고 물러 나지요";
                        m[2] = "그 상자 안의 금을 모두 주시오";
                        k_val = Select(150, 2, 2, false, true);
                        if (k_val == 1) {
                            Print(7, " 당신은 트롤킹에게 금을 받고는 동굴 밖으로");
                            Print(7, "나왔다.");
                            setbit(16);
                            DefaultExit();
                            return;
                        }
                    }
                }
                Print(7, " 순간 사납게 생긴 두 명의 트롤 전사와 마법");
                Print(7, "사 복장의 한 명이 나타났다.");
                if (getbit(9)) {
                    Print(7, "");
                    Print(13, " 우리는 트롤킹님의 신변 보호를 맡고 있는 트");
                    Print(13, "롤의 광전사들과 위저드이다. 우리들은 목숨을");
                    Print(13, "걸고 당신들로부터 우리의 왕을 지킬 것이다.");
                }
                enemynumber = 7;
                for (int i_idx = 1; i_idx <= 4; i_idx++) joinenemy(i_idx, 32);
                for (int i_idx = 5; i_idx <= 6; i_idx++) joinenemy(i_idx, 33);
                joinenemy(7, 34);
                DisplayEnemies(true);
                PressAnyKey();
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { Silent_Scroll(); return; }
                
                Clear();
                Print(7, " 당신은 경호대원들을 물리치고는 뒤에서 벌벌");
                Print(7, "떨고 있는 트롤킹에게 검을 겨누었다. 그는 겁");
                Print(7, "에 질린채 주저 않았고 당신은 그를 단숨에 베");
                Print(7, "어버렸다.");
                Print(7, "");
                Print(14, " 이제 당신은 전과자 신분이라기보다는 영웅으");
                Print(14, "로써 로어성에 귀환 할 수 있게 되었다.");
                party.etc[20] = 4;
                setbit(4);
                setbit(72);
                PutMap(25, 8, 44); PutMap(26, 8, 44);
                Scroll(true);
                return;
            }
            if (at(17, 19)) {
                enemynumber = 7;
                for (int i_idx = 1; i_idx <= 3; i_idx++) {
                    joinenemy(i_idx, 20);
                    // strcpy 사용할 수 있으나 string 할당으로 가정
                    enemy[i_idx].name = "트롤매직유저";
                    enemy[i_idx].level = 6;
                }
                for (int i_idx = 4; i_idx <= 6; i_idx++) joinenemy(i_idx, 28);
                joinenemy(7, 34);
                DisplayEnemies(true);
                if (getbit(9)) {
                    Print(13, " 감히 트롤의 보물을 탈취하려고 이곳에 오다");
                    Print(13, "니! 나는 이 보물창고의 경비대장으로써 목숨");
                    Print(13, "을 걸고 보물을 지킬테다.");
                    PressAnyKey();
                }
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { Silent_Scroll(); return; }
                setbit(73); PutMap(17, 19, 44); Scroll(true); return;
            }
            if (at(34, 19)) {
                enemynumber = 7;
                for (int i_idx = 1; i_idx <= 4; i_idx++) joinenemy(i_idx, 31);
                for (int i_idx = 5; i_idx <= 6; i_idx++) joinenemy(i_idx, 32);
                joinenemy(7, 33);
                DisplayEnemies(true);
                if (getbit(9)) {
                    Print(13, " 바로 너희들이 우리 동굴에 잡혀 있는 인간들");
                    Print(13, "을 구하러온 자들인가? 하지만 나의 파괴 마법");
                    Print(13, "앞에서는 너희들의 배짱도 산산히 부셔져 버릴");
                    Print(13, "것이다.");
                    PressAnyKey();
                }
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { Silent_Scroll(); return; }
                setbit(74); PutMap(34, 19, 44); Scroll(true); return;
            }
            if (at(13, 32) || at(13, 33)) {
                enemynumber = 8;
                for (int i_idx = 1; i_idx <= 4; i_idx++) joinenemy(i_idx, 31);
                for (int i_idx = 5; i_idx <= 8; i_idx++) joinenemy(i_idx, 28);
                DisplayEnemies(true);
                BattleMode(true, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { Silent_Scroll(); return; }
                setbit(75); PutMap(13, 32, 44); PutMap(13, 33, 44); Scroll(true); return;
            }
            if (at(38, 32) || at(38, 33)) {
                enemynumber = 8;
                for (int i_idx = 1; i_idx <= 2; i_idx++) joinenemy(i_idx, 26);
                for (int i_idx = 3; i_idx <= 5; i_idx++) joinenemy(i_idx, 31);
                for (int i_idx = 6; i_idx <= 8; i_idx++) joinenemy(i_idx, 28);
                DisplayEnemies(true);
                BattleMode(true, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { Silent_Scroll(); return; }
                setbit(76); PutMap(38, 32, 44); PutMap(38, 33, 44); Scroll(true); return;
            }
            if (at(24, 67) || at(25, 67) || at(26, 67)) {
                enemynumber = 8;
                for (int i_idx = 1; i_idx <= 8; i_idx++) {
                    joinenemy(i_idx, 20);
                    enemy[i_idx].name = "트롤매직유저";
                    enemy[i_idx].level = 5;
                }
                DisplayEnemies(true);
                if (getbit(9)) {
                    Print(13, " 너희들을 절대 이 안으로 들여 보낼수 없다!!");
                    PressAnyKey();
                }
                BattleMode(false, true);
                if (party.etc[6] == 254) return;
                if (party.etc[6] == 2) { Silent_Scroll(); return; }
                setbit(77); PutMap(24, 67, 44); PutMap(25, 67, 44); PutMap(26, 67, 44); Scroll(true); return;
            }

            int h_val = 0;
            if (at(6, 31)) {
                h_val = 1;
                if (getbit(9)) {
                    Print(7, " 날 제발 살려 주시오. 나는 이곳의 물리학자");
                    Print(7, "인데 당신에게 중요한 발견하나를 알려줄테니");
                    Print(7, "제발 목숨만은 살려주시오.");
                    m[0] = "";
                    m[1] = "좋소, 한번 들어봅시다";
                    m[2] = "날 속이려고 드는군, 자 죽어랏!";
                    int k_val = Select(120, 2, 2, false, true);
                    if (k_val == 1) {
                        Print(7, " 오, 정말 감사하오. 나는 드라코니안족에게서");
                        Print(7, "물리학을 배우고 이곳에 왔던 참이라오. 나는");
                        Print(7, "차원에 관한 여러가지 연구를 했었소. 그러던");
                        Print(7, "중에 나는 로어 세계에서도 다른 차원으로 가");
                        Print(7, "는 길이 있다는걸 알아냈소. 그곳의 중력은 무");
                        Print(7, "슨 이유에서인지 상당히 밀도가 높게 압축되어");
                        Print(7, "있었고 태양의 인력이 반대로 작용하는 자정무");
                        Print(7, "렵에는 그 지역의 공간이 일그러져 다른 세계");
                        Print(7, "로의 문이 열리는 것을 목격했소. 그 공간의");
                        Print(7, "창을 통해 내가 본것은 허허 벌판이나 죽어가");
                        Print(7, "는 행성의 최후의 모습같은 것들도 있었지만");
                        Print(7, "때때로 생명체가 사는 별들과 연결 되는 것도");
                        Print(7, "확인했소. 거대한 파충류가 사는 곳도 있고,");
                        Print(7, "규질의 몸뚱이를 가지고 질소를 마시며 생활하");
                        Print(7, "는 생물도 보았소. 그중에서도 내가 가장 관심");
                        Print(7, "있게 본 것은, 우리와 같이 C,H,O,N 으로 구성");
                        Print(7, "된 피부를 가지고 산화작용으로 ATP를 합성하");
                        Print(7, "는 생물이 사는 곳이었소. 그들은 마법도 사용");
                        Print(7, "할 줄 모르고 생활하고 있지만 대단한 살상무");
                        Print(7, "기를 많이 보유하고 있었소. 또한 '뉴욕'이란");
                        Print(7, "도시를 보았을때 정말 그들의 과학력과 건축기");
                        Print(7, "술에 탄복하고 말았소. 당신도 내 말을 믿을수");
                        Print(7, "있겠소?");
                        Print(7, "");
                        PressAnyKey();
                        
                        k_val = 0;
                        for (int i_idx = 5; i_idx >= 2; i_idx--) {
                            if (exist(i_idx)) k_val = i_idx;
                        }
                        if (k_val > 0) {
                            std::string s_val = std::string(player[k_val].name) + ReturnBJosa(player[k_val].name);
                            Print(7, " 그때 당신 옆에 있던 " + s_val + " 검을 들어 그를 내리쳤다.");
                            Print(7, " 순간적으로 일어난 일이라 아무도 손을 쓸 수");
                            Print(7, "가 없었다. 그리고 일행들에게 말했다.");
                            Print(7, "");
                            Print(10, " 쳇, 이런 겁장이가 지어내는 말을 믿고 있다");
                            Print(10, "니... 자네들 어서가게. 이런데서 시간을 낭비");
                            Print(10, "해 버리면 우리만 불리해진다네.");
                            PutMap(6, 31, 44);
                            Silent_Scroll();
                            setbit(78);
                            PressAnyKey();
                            Print(7, " 당신은 그래도 아쉬운 마음에 한 마디를 던졌");
                            Print(7, "다.");
                            Print(7, "");
                            Print(9, " 이건 자네가 좀 심했네. 그의 말이 황당하기");
                            Print(9, "는 했지만 거짓인것 같지는 않았다고 생각하");
                            Print(9, "네. 이왕 들은건데 그 장소까지 알았으면 좋았");
                            Print(9, "을걸...");
                            return;
                        }
                    }
                }
            }
            if (at(9, 30)) {
                h_val = 2;
                if (getbit(9)) {
                    Print(7, " 으악! 인간이다 모두 도망쳐라! 인간이 우리");
                    Print(7, "동굴에 침입했다.");
                    PressAnyKey();
                }
            }
            if (at(7, 33)) {
                h_val = 3;
                if (getbit(9)) {
                    Print(7, " 정말 당신들은 집요하군요. 또 우리에게 해를");
                    Print(7, "입히려 쳐들어오다니...");
                    PressAnyKey();
                }
            }
            if (at(11, 35)) {
                h_val = 4;
                if (getbit(9)) {
                    Print(7, " 당신들이 나를 죽이더라도 나의 신인 에인션");
                    Print(7, "트 이블님이 복수를 해 주실 겁니다.");
                    PressAnyKey();
                }
            }
            if (at(40, 30)) {
                h_val = 5;
                if (getbit(9)) {
                    Print(7, " 침략자 !!");
                    PressAnyKey();
                }
            }
            if (at(41, 35)) {
                h_val = 6;
                if (getbit(9)) {
                    Print(7, " 아이고, 위대하신 용사님들. 제발 저에게 헤");
                    Print(7, "로인 주사를 좀 놔 주십시요. 부탁합니다. 지");
                    Print(7, "금 헤로인을 맞지 않으면 죽을 것 같습니다.");
                    PressAnyKey();
                }
            }
            if (at(43, 32)) {
                h_val = 7;
                if (getbit(9)) {
                    Print(7, " 당신네 인간들은 우리 트롤족에게서 금을 뺏");
                    Print(7, "기위해, 진통과 설사에 좋다는 명목으로 헤로");
                    Print(7, "인을 주사하기 시작했소. 그 때문에 상당수의");
                    Print(7, "트롤 주민들이 헤로인 중독에 고생하며, 또한");
                    Print(7, "헤로인을 구하기 위해 인간들에게 많은 금을");
                    Print(7, "지불하고 있소.");
                    PressAnyKey();
                }
            }
            if (at(45, 34)) {
                h_val = 8;
                if (getbit(9)) {
                    Print(7, " 으윽, 제발 헤로인 좀 놔 주십시오. 금 일 만");
                    Print(7, "개를 드리리다.");
                    PressAnyKey();
                }
            }

            if (h_val > 0) {
                if (!getbit(9)) {
                    Print(7, " 당신 앞의 트롤족이 당신에게 무어라 말을 했");
                    Print(7, "지만 알아 들을 수가 없었다.");
                    PressAnyKey();
                }
                Print(7, " 당신 앞에 있는 트롤족을 당신은 어떻게 할");
                Print(7, "것인가를 선택하시오.");
                m[0] = "";
                m[1] = "죽여 버린다";
                m[2] = "그냥 살려 준다";
                int k_val = Select(110, 2, 2, false, true);
                if (k_val == 1) {
                    enemynumber = 1;
                    joinenemy(1, 25);
                    DisplayEnemies(true);
                    BattleMode(true, true);
                    if (party.etc[6] == 254) return;
                    if (party.etc[6] == 2) { Silent_Scroll(); return; }

                    PutMap(pas_x + pas_x1, pas_y + pas_y1, 44);
                    Scroll(true);
                    setbit(78 + (h_val - 1));
                }
                return;
            }
        } else {
            // 타일이 54가 아닐 때의 로직 (감옥 영역 등)
            if (at(37, 17)) {
                Print(7, " 당신들이 우리를 구하러 온 사람들이오? 정말");
                Print(7, "감사하오. 소문에 의하면 베스퍼성이 완전히");
                Print(7, "전멸 당했다는데 그 말이 사실이오? 그렇다면");
                Print(7, "정말 유감이군요.");
                PressAnyKey();
                Print(7, " 나는 베스퍼성에서 베크룩스라고 불리던 사람");
                Print(7, "이오. 나는 단독으로 트롤킹을 잡기위해 여기");
                Print(7, "로 잠입했다가 적의 숫자에 밀려서 포로로 잡");
                Print(7, "혀버렸소. 일대일의 대결이라면 자신이 있었는");
                Print(7, "데 적의 수가 워낙 많아서 이런 곳까지 잡혀오");
                Print(7, "게 되었던거요. 나는 이 일로 더욱 타종족에");
                Print(7, "대한 미움이 커져 버렸소. 나는 당신들에게 진");
                Print(7, "정으로 부탁하나 하겠소. 나를 당신의 일행에");
                Print(7, "넣어 주시오.");
                m[0] = "";
                m[1] = "당신의 부탁을 들어주겠소";
                m[2] = "미안하지만 그런 어렵겠소";
                if (Select(220, 2, 2, false, true) == 1) {
                    int k_val = 0;
                    do {
                        k_val++;
                    } while (std::string(player[k_val].name) != "" && k_val < 6);

                    if (k_val < 6) {
                        strcpy(player[k_val].name, "베크룩스");
                        player[k_val].name_len = strlen("베크룩스");
                        player[k_val].sex = 1; // 1 = male 가이드라인 가정
                        player[k_val]._class = 1; 
                        player[k_val].classtype = 1; // 1 = magic 가정
                        player[k_val].level = 9;
                        player[k_val].strength = 10;
                        player[k_val].mentality = 17;
                        player[k_val].concentration = 18;
                        player[k_val].endurance = 12;
                        player[k_val].resistance = 15;
                        player[k_val].agility = 14;
                        player[k_val].accuracy = 16;
                        player[k_val].luck = 12;
                        player[k_val].poison = 0;
                        player[k_val].unconscious = 0;
                        player[k_val].dead = 0;
                        player[k_val].hp = player[k_val].endurance * player[k_val].level * 10;
                        player[k_val].sp = 0;
                        player[k_val].experience = 0;
                        player[k_val].potential_experience = ReturnEXP(player[k_val].level);
                        player[k_val].weapon = 0;
                        player[k_val].shield = 0;
                        player[k_val].armor = 1;
                        player[k_val].potential_ac = 1;
                        player[k_val].sword_data.sword_skill = 40;
                        player[k_val].sword_data.axe_skill = 40;
                        player[k_val].sword_data.spear_skill = 40;
                        player[k_val].sword_data.bow_skill = 0;
                        player[k_val].sword_data.shield_skill = 30;
                        player[k_val].sword_data.fist_skill = 20;
                        
                        Modify_Item(k_val);
                        PutMap(37, 17, 44);
                        setbit(46);
                        Silent_Scroll();
                        Display_Condition();
                        Clear();
                    } else {
                        Print(7, " 벌써 사람이 모두 채워져 있군요. 다음 기회");
                        Print(7, "를 기다리지요.");
                        return;
                    }
                } else {
                    asyouwish();
                }
            }
            if (at(42, 16)) {
                Print(7, " 헤로인을 만드는건 쉬워요. 설익은 양귀비 열");
                Print(7, "매 꼬투리에 흠집을내면 나오는 우유같은 수액");
                Print(7, "을 갈색 고무질로 건조 시키면 되는거니까요.");
                Print(7, "그건 일종의 중추 신경 완화제의 역할을 하죠.");
                Print(7, "그걸 트롤에게 팔아서 꽤 돈을 벌었는데 한날");
                Print(7, "트롤족들이 들고 일어나는 바람에 꼼짝없이 잡");
                Print(7, "혀서 여기에 있는거죠.");
            }
            if (at(42, 21)) {
                Print(7, " 베스퍼성의 사람들은 이 대륙을 확실한 인간");
                Print(7, "의 영토로 굳히기 위해 번번히 트롤족의 동굴");
                Print(7, "을 공격했소. 때로는 협박을하고 때로는 회유");
                Print(7, "책을 쓰면서 그들을 대륙의 구석으로 몰아 붙");
                Print(7, "이려 했지만 역부족이었소. 그래서 베스퍼 성");
                Print(7, "주를 중심으로 트롤의 동굴에 대대적인 공격을");
                Print(7, "감행했었소. 그때 죽거나 잡힌 트롤들은 모두");
                Print(7, "사지가 절단된채 쇠 꼬챙이에 꿰어 매달려 있");
                Print(7, "었소. 그들에게 겁을 주어 다시는 인간에게 대");
                Print(7, "항하지 못하게 하자는 속셈이었던 거요. 하지");
                Print(7, "만 예상은 빗나갔었소. 그 광경들을 본 트롤들");
                Print(7, "은 치밀어 오르는 분노에 집단적으로 베스퍼성");
                Print(7, "에 대한 야습을 개시했던 거요. 여지껏 당하고");
                Print(7, "만 살아왔던 한을 이때 모두 폭발시켰던 거라");
                Print(7, "오. 그래서 예상하지도 못했던 기습을 받은 베");
                Print(7, "스퍼성은 순식간에 전멸되었던 것이었소. 모두");
                Print(7, "가 다 우리가 자청한 일이나 다름없었던 거지");
                Print(7, "요.");
                Print(7, "");
                PressAnyKey();
            }
        }
    }

    // ==========================================
    // 5. Tomb (고분)
    // ==========================================
    if (mapname == "Tomb" || mapname == "TOMB") {
        if (at(89, 20) || at(88, 21)) {
            Clear();
            Print(7, " 당신은 여기서 뼈만 남은 인간의 해골을 보았");
            Print(7, "다. 아마도 오래전에 여기를 도굴하려다가 갇");
            Print(7, "혔던 것같다.");
        }
    }
}