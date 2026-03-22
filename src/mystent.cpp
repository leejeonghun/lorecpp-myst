#include "mystent.h"
#include "mystsub.h"
#include "uhanx.h"
#include "mystbatt.h"
#include "mystend.h"
#include "mysttalk.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

void EnterMode() {
    if (mapname == "Ground1" || mapname == "GROUND1") {
        if (at(20, 11)) {
            if (wantenter("로어성")) {
                if (party.year < 700) {
                    mapname = "Lore";
                    Load();
                    Scroll(true);
                    if (getbit(35)) PutMap(41, 10, 44);
                    if (getbit(36)) PutMap(22, 50, 44);
                    if (getbit(37)) PutMap(22, 54, 44);
                    if (getbit(38)) PutMap(18, 38, 47);
                } else {
                    mapname = "Dark";
                    Load();
                    Scroll(true);
                }
            }
        }
        if (at(76, 57)) {
            if (party.year > 699) {
                Clear();
                Print(7, " 당신이  라스트디치 성으로  들어가려 했지만");
                Print(7, "그곳 주민으로 등록되지 않은 이방인이란 이유");
                Print(7, "로 쫒겨났다.");
                return;
            }
            if (wantenter("라스트디치성")) {
                mapname = "LastDtch";
                Load();
                Scroll(true);
                if (getbit(39)) PutMap(61, 58, 44);
                if (getbit(41)) PutMap(34, 30, 47);
            }
        }
        if (at(17, 89)) {
            if (party.year > 699) {
                Clear();
                Print(7, " 당신이 메너스로 들어가려 했지만  이미 이곳");
                Print(7, "은 폐광이 된데다가 입구마저 함몰되어 도저히");
                Print(7, "들어갈 수가 없었다.");
                return;
            }
            if (wantenter("메너스")) {
                mapname = "Menace";
                Load();
                Scroll(true);
            }
        }
    }

    if (mapname == "LastDtch" || mapname == "LASTDTCH") {
        if (at(38, 10) || at(39, 10)) {
            if (wantenter("가이아 게이트")) {
                if (!getbit(50)) {
                    DefaultEnter();
                    if (getbit(40)) PutMap(17, 24, 27);
                    if (getbit(42)) PutMap(56, 40, 27);
                } else {
                    absolute_set_xy = true;
                    pas_x = Map_Header.enterX;
                    pas_y = Map_Header.enterY;
                    mapname = "Light";
                    Load();
                    Scroll(true);
                    absolute_set_xy = false;
                    pas_x1 = 0; pas_y1 = 0;
                }
            }
        }
    }

    if (mapname == "Valiant" || mapname == "VALIANT" || mapname == "Light" || mapname == "LIGHT") {
        if (at(38, 10) || at(39, 10)) {
            if (wantenter("로어 게이트")) {
                DefaultEnter();
                if (getbit(39)) PutMap(61, 58, 44);
                if (getbit(41)) PutMap(34, 30, 47);
            }
        }
    }

    if (mapname == "Ground2" || mapname == "GROUND2") {
        if (at(19, 26)) {
            if (wantenter("배리언트 피플즈성")) {
                if (!getbit(50)) mapname = "Valiant";
                else mapname = "Light";
                Load();
                Scroll(true);
                if (!getbit(50)) {
                    if (getbit(40)) PutMap(17, 24, 27);
                    if (getbit(42)) PutMap(56, 40, 27);
                }
            }
        }
        if (at(31, 82)) {
            if (wantenter("가이아 테라성")) {
                mapname = "Gaea";
                Load();
                if (getbit(43)) PutMap(25, 9, 44);
                if (getbit(44)) PutMap(11, 36, 44);
                Scroll(true);
            }
        }
        if (at(82, 47)) {
            if (wantenter("오크 마을")) {
                mapname = "OrcTown";
                Load();
                if (getbit(57)) PutMap(10, 39, 34);
                if (getbit(58)) PutMap(9, 19, 34);
                if (getbit(59)) PutMap(13, 12, 34);
                if (getbit(60)) PutMap(21, 9, 34);
                if (getbit(61)) PutMap(43, 12, 34);
                if (getbit(62)) PutMap(44, 30, 34);
                if (getbit(63)) PutMap(35, 29, 34);
                if (getbit(64)) PutMap(41, 39, 34);
                if (getbit(53)) { PutMap(25, 42, 43); PutMap(25, 43, 43); }
                if (getbit(54)) { PutMap(24, 20, 43); PutMap(26, 20, 43); }
                if (getbit(55)) PutMap(23, 18, 43);
                if (getbit(56)) { PutMap(25, 17, 43); PutMap(26, 17, 43); }
                Scroll(true);
            }
        }
    }

    if (mapname == "Ground3" || mapname == "GROUND3") {
        if (at(69, 29) && wantenter("베스퍼성")) {
            absolute_set_xy = true;
            mapname = "Vesper";
            if (pas_x1 == 1) { pas_x = 6; pas_y = 38; }
            if (pas_x1 == -1) { pas_x = 70; pas_y = 38; }
            if (pas_y1 == 1) { pas_x = 38; pas_y = 7; }
            if (pas_y1 == -1) { pas_x = 38; pas_y = 69; }
            Load();
            if (getbit(65)) PutMap(30, 19, 43);
            if (getbit(66)) PutMap(50, 19, 43);
            if (getbit(67)) PutMap(64, 26, 43);
            if (getbit(68)) PutMap(61, 33, 43);
            if (getbit(69)) PutMap(40, 40, 43);
            if (getbit(70)) PutMap(22, 40, 43);
            if (getbit(71)) PutMap(20, 26, 43);
            Scroll(true);
            absolute_set_xy = false;
            return;
        }
        if (at(81, 15) && wantenter("트롤 마을")) {
            mapname = "TrolTown";
            Load();
            if (getbit(46)) PutMap(37, 17, 44);
            if (getbit(72)) { PutMap(25, 8, 44); PutMap(26, 8, 44); }
            if (getbit(73)) PutMap(17, 19, 44);
            if (getbit(74)) PutMap(34, 19, 44);
            if (getbit(75)) { PutMap(13, 32, 44); PutMap(13, 33, 44); }
            if (getbit(76)) { PutMap(38, 32, 44); PutMap(38, 33, 44); }
            if (getbit(77)) { PutMap(24, 67, 44); PutMap(25, 67, 44); PutMap(26, 67, 44); }
            if (getbit(78)) PutMap(6, 31, 44);
            if (getbit(79)) PutMap(9, 30, 44);
            if (getbit(80)) PutMap(7, 33, 44);
            if (getbit(81)) PutMap(11, 35, 44);
            if (getbit(82)) PutMap(40, 30, 44);
            if (getbit(83)) PutMap(41, 35, 44);
            if (getbit(84)) PutMap(43, 32, 44);
            if (getbit(85)) PutMap(45, 34, 44);
            Scroll(true);
            return;
        }
        if (at(81, 76)) {
            Clear();
            if (getbit(10)) {
                if (!getbit(9)) {
                    Print(7, " 당신이 기묘한 비석 앞에 섰을때 그것은 트롤");
                    Print(7, "글로 적힌 주문이란걸 알았다. 당신은 낮은 소");
                    Print(7, "리로 그 주문을 읖조렸다.  순간 비석이  둘로");
                    Print(7, "쪼개어지며 에테르체의 형상이 나타났다. 그리");
                    Print(7, "고는 서서히 생체 프라즈마질의 트롤 형상으로");
                    Print(7, "변해갔다.  그것이 완전한 트롤의 형상으로 변");
                    Print(7, "하자  내게 무어라 알수 없는 말을 지껄이더니");
                    Print(7, "다시 당신의 모습으로 닮아갔다.  당신의 모습");
                    Print(7, "으로 변한 플라즈마체의 기운은 서서히 당신의");
                    Print(7, "몸속에 스며 들어갔다. 그 알수 없는 존재와의");
                    Print(7, "결합이 끝났을때  당신은  그 존재가 지껄였던");
                    Print(7, "말이 트롤족의 말이었음을 알게 되었다.  그리");
                    Print(7, "고  이제 당신이  트롤족의 말을 할 수 있다는");
                    Print(7, "걸 깨닭게 되었다.");
                    setbit(9);
                } else {
                    Print(7, " 당신 앞에는  반으로 쪼개어진 비석만 남아있");
                    Print(7, "었다.");
                }
            } else {
                Print(7, " 당신 앞에는 기묘한 도형이 새겨진 비석이 서");
                Print(7, "있었다.");
            }
        }
    }

    if (mapname == "Ground4" || mapname == "GROUND4") {
        if (at(82, 16)) {
            if (getbit(86)) {
                Clear();
                Print(7, " 당신은  코볼트킹을 가둬놓기 위해 만든 로드");
                Print(7, "안의 결계 때문에 안으로 들어갈 수 없었다.");
                return;
            }
            if (wantenter("코볼트 마을")) {
                mapname = "Kobold";
                Load();
                Scroll(true);
                if (party.etc[30] == 0) party.etc[30] = ((std::rand() % 5 + 1) << 4) + (std::rand() % 5 + 1);
                if (party.etc[31] == 0) party.etc[31] = std::rand() % 31 + 1;
            }
        }
        if (at(20, 39)) {
            if (wantenter("에인션트이블 신전")) {
                absolute_set_xy = true;
                mapname = "Ancient";
                if (pas_x1 == 1) { pas_x = 6; pas_y = 15; }
                if (pas_x1 == -1) { pas_x = 14; pas_y = 15; }
                if (pas_y1 == 1) { pas_x = 10; pas_y = 7; }
                if (pas_y1 == -1) { pas_x = 10; pas_y = 24; }
                Load();
                if (getbit(28)) for (int i_idx = 10; i_idx <= 11; i_idx++) PutMap(i_idx, 17, 44);
                if (getbit(23)) for (int i_idx = 10; i_idx <= 11; i_idx++) PutMap(i_idx, 13, 44);
                Scroll(true);
                absolute_set_xy = false;
                return;
            }
        }
        if (at(48, 57)) {
            if (wantenter("오두막집")) {
                absolute_set_xy = true;
                mapname = "Hut";
                if (pas_x1 == 1) { pas_x = 6; pas_y = 15; }
                if (pas_x1 == -1) { pas_x = 25; pas_y = 15; }
                if (pas_y1 == 1) { pas_x = 15; pas_y = 7; }
                if (pas_y1 == -1) { pas_x = 15; pas_y = 24; }
                Load();
                if (getbit(204)) PutMap(8, 13, 35);
                if (getbit(205)) PutMap(8, 14, 35);
                Scroll(true);
                absolute_set_xy = false;
                return;
            }
        }
        if (at(26, 16)) {
            Clear();
            Print(12, " 여기는 ''발하라 전당''.");
            Print(12, " 시간 관계상 제작을 하지 못했음.");
            Print(12, "");
            Print(7, "                       제작자 씀.");
        }
    }

    if (mapname == "Ancient" || mapname == "ANCIENT") {
        if (at(10, 23)) {
            if (getbit(12)) {
                if (getbit(11)) return;
                Clear();
                Print(7, " 당신은 코볼트글로 되어 있는 기둥을 보았다.");
                Print(7, "그리고 그곳에 적힌 글을 읽자  당신의 머리속");
                Print(7, "을 꽉 채우는 무언가를 느꼈다.");
                setbit(11);
            } else {
                Print(7, " 당신 앞에는  처음 보는 문자가 적힌  기둥이");
                Print(7, "서 있었다.");
            }
        }
    }

    if (mapname == "Ground5" || mapname == "GROUND5") {
        if (at(39, 36)) {
            if (wantenter("드라코니안 마을")) {
                mapname = "DracTown";
                Load();
                if (getbit(111)) PutMap(24, 98, 44);
                if (getbit(112)) PutMap(24, 92, 44);
                if (getbit(113)) PutMap(25, 81, 44);
                if (getbit(114)) PutMap(34, 100, 44);
                if (getbit(115)) PutMap(36, 93, 44);
                if (getbit(116)) PutMap(42, 84, 44);
                if (getbit(117)) PutMap(33, 82, 44);
                if (getbit(118)) PutMap(41, 70, 44);
                if (getbit(119)) PutMap(32, 22, 44);
                if (getbit(120)) PutMap(40, 33, 44);
                if (getbit(121)) PutMap(16, 19, 44);
                if (getbit(122)) PutMap(16, 35, 44);
                if (getbit(123)) PutMap(15, 48, 44);
                if (getbit(47)) PutMap(15, 57, 44);
                if (getbit(104)) PutMap(77, 73, 44);
                if (getbit(105)) PutMap(67, 66, 44);
                if (getbit(106)) PutMap(67, 73, 44);
                if (getbit(107)) PutMap(62, 71, 44);
                if (getbit(108)) PutMap(67, 88, 44);
                if (getbit(109)) PutMap(62, 79, 44);
                if (getbit(110)) PutMap(65, 64, 44);
                if (getbit(103)) { PutMap(14, 105, 44); PutMap(15, 106, 44); }
                if (getbit(102)) { for (int j_idx = 59; j_idx <= 61; j_idx++) for (int i_idx = 47; i_idx <= 48; i_idx++) PutMap(i_idx, j_idx, 44); }
                if (getbit(101)) { for (int i_idx = 24; i_idx <= 27; i_idx++) PutMap(i_idx, 54, 44); }
                if (getbit(100)) { for (int j_idx = 59; j_idx <= 61; j_idx++) PutMap(84, j_idx, 44); }
                if (getbit(99)) { for (int i_idx = 77; i_idx <= 79; i_idx++) PutMap(i_idx, 88, 44); }
                if (getbit(98)) { for (int i_idx = 59; i_idx <= 61; i_idx++) PutMap(i_idx, 48, 44); }
                if (getbit(97)) { for (int i_idx = 73; i_idx <= 75; i_idx++) PutMap(i_idx, 40, 44); }
                if (getbit(96)) { for (int i_idx = 94; i_idx <= 96; i_idx++) PutMap(i_idx, 37, 44); }
                if (getbit(95)) { PutMap(102, 20, 44); PutMap(102, 21, 42); PutMap(101, 21, 44); PutMap(103, 21, 44); }
                Scroll(true);
            }
        }
        if (at(15, 31)) {
            if (wantenter("임페리엄 마이너성")) {
                mapname = "Imperium";
                Load();
                Scroll(true);
            }
        }
    }

    if (mapname == "Tomb" || mapname == "TOMB") {
        if (wantexit()) {
            DefaultExit();
            if (getbit(111)) PutMap(24, 98, 44);
            if (getbit(112)) PutMap(24, 92, 44);
            if (getbit(113)) PutMap(25, 81, 44);
            if (getbit(114)) PutMap(34, 100, 44);
            if (getbit(115)) PutMap(36, 93, 44);
            if (getbit(116)) PutMap(42, 84, 44);
            if (getbit(117)) PutMap(33, 82, 44);
            if (getbit(118)) PutMap(41, 70, 44);
            if (getbit(119)) PutMap(32, 22, 44);
            if (getbit(120)) PutMap(40, 33, 44);
            if (getbit(121)) PutMap(16, 19, 44);
            if (getbit(122)) PutMap(16, 35, 44);
            if (getbit(123)) PutMap(15, 48, 44);
            if (getbit(47)) PutMap(15, 57, 44);
            if (getbit(104)) PutMap(77, 73, 44);
            if (getbit(105)) PutMap(67, 66, 44);
            if (getbit(106)) PutMap(67, 73, 44);
            if (getbit(107)) PutMap(62, 71, 44);
            if (getbit(108)) PutMap(67, 88, 44);
            if (getbit(109)) PutMap(62, 79, 44);
            if (getbit(110)) PutMap(65, 64, 44);
            if (getbit(103)) { PutMap(14, 105, 44); PutMap(15, 106, 44); }
            if (getbit(102)) { for (int j_idx = 59; j_idx <= 61; j_idx++) for (int i_idx = 47; i_idx <= 48; i_idx++) PutMap(i_idx, j_idx, 44); }
            if (getbit(101)) { for (int i_idx = 24; i_idx <= 27; i_idx++) PutMap(i_idx, 54, 44); }
            if (getbit(100)) { for (int j_idx = 59; j_idx <= 61; j_idx++) PutMap(84, j_idx, 44); }
            if (getbit(99)) { for (int i_idx = 77; i_idx <= 79; i_idx++) PutMap(i_idx, 88, 44); }
            if (getbit(98)) { for (int i_idx = 59; i_idx <= 61; i_idx++) PutMap(i_idx, 48, 44); }
            if (getbit(97)) { for (int i_idx = 73; i_idx <= 75; i_idx++) PutMap(i_idx, 40, 44); }
            if (getbit(96)) { for (int i_idx = 94; i_idx <= 96; i_idx++) PutMap(i_idx, 37, 44); }
            if (getbit(95)) { PutMap(102, 20, 44); PutMap(102, 21, 42); PutMap(101, 21, 44); PutMap(103, 21, 44); }
            
            // name_hidden에 백업된 원래 이름 복원
            for (int i = 2; i <= 5; i++) {
                player[i].name_len = player[i].name_hidden_len;
                std::memcpy(player[i].name, player[i].name_hidden,
                            player[i].name_hidden_len);
                if (player[i].name_hidden_len < 17)
                    player[i].name[player[i].name_hidden_len] = '\0';
                player[i].name_hidden_len = 0;
            }
            Display_Condition();
        }
    }

    if (mapname == "HdsGate" || mapname == "HDSGATE") {
        if (at(21, 33)) {
            if (party.etc[25] == 2) {
                if (wantenter("하데스 테라")) {
                    DefaultEnter();
                }
            } else {
                Clear();
                Print(13, " 지하 세계의 입구에서 무언가 강한 힘이 뿜어");
                Print(13, "져 나와 당신을 밀쳐내 버렸다.");
            }
        }
    }

    if (mapname == "Vesper" || mapname == "VESPER") TalkMode();
    if (mapname == "TrolTown" || mapname == "TROLTOWN") TalkMode();
}

void Sign() {
    SDL_Event ev_flush; while(SDL_PollEvent(&ev_flush)) {};
    // Clear key buffer logic omitted
    Clear();
    if (mapname == "OrcTown" || mapname == "ORCTOWN") { TalkMode(); return; }
    if (mapname == "Vesper" || mapname == "VESPER") { TalkMode(); return; }
    if (mapname == "TrolTown" || mapname == "TROLTOWN") { TalkMode(); return; }
    if (mapname == "Tomb" || mapname == "TOMB") { TalkMode(); return; }

    yline = 30; hany = 16;
    Print(7, "푯말에 쓰여있기로 ...");
    Print(7, ""); Print(7, "");

    if (mapname == "Lore" || mapname == "LORE") {
        if (at(51, 84)) {
            Print(15, "          이곳은 로어성입니다.");
            Print(15, "          여러분을 환영합니다.");
            Print(7, ""); Print(7, "");
            Print(10, "         이곳의 성주 로드안 씀");
        }
        if (at(51, 15)) Print(15, "      로어 왕립 죄수 수용소");
        if (at(24, 31)) {
            Print(15, "             로어 주점");
            Print(15, "");
            Print(15, "      ( 코리아 위스키 전문점 )");
        }
        if (at(51, 58)) {
            if (getbit(50)) {
                Print(15, "     이제 우리를 위협하던 네 종족은");
                Print(15, "     정의의 심판에 의해 무릎을 꿓고");
                Print(15, "     말았다. 이제부터는 새로운 인류");
                Print(15, "     의 역사가 시작 될 것이다.");
            } else {
                Print(15, "   674년 1월 17일. 아프로디테의 베스퍼성이");
                Print(15, "   호전적인  트롤족에 의해  참변을 당했다.");
                Print(15, "   이를 계기로  로어성과  그 이외의 성들은");
                Print(15, "   오크족, 트롤족, 코볼트족,  드라코니안족");
                Print(15, "   에 대한 무차별 정벌을 선포하는 바이다.");
                Print(15, "   이로 인해  인간 이외의 다른 종족에 대한");
                Print(15, "   호의를 일체 중지하며 이제는 적대 관계로");
                Print(15, "   그들을 대할 것이다.");
            }
        }
    }

    if (mapname == "Dark" || mapname == "DARK") {
        if (at(51, 84)) {
            Print(15, "          이곳은 로어성입니다.");
            Print(15, "          여러분을 환영합니다.");
            Print(7, ""); Print(7, "");
            Print(10, "         이곳의 성주 로드안 씀");
        }
        if (at(51, 18) || at(52, 18)) Print(15, "      로어 왕립 죄수 수용소");
        if (at(24, 31)) {
            Print(15, "             로어 주점");
            Print(15, "");
            Print(15, "( 코리아 위스키가 잘 팔린다는 말 않겠음 )");
        }
    }

    if (mapname == "LastDtch" || mapname == "LASTDTCH") {
        if (at(39, 8)) Print(15, " 이곳은 가이아 테라로 통하는 게이트 입니다.");
        if (at(39, 68)) {
            Print(7, "");
            Print(15, " 선을 위해서라면 어떤 위험이나 어려움에도");
            Print(15, "굴하지 않는 사람들이 있는 곳, 라스트 디치.");
            Print(7, "");
            Print(15, "           여러분을 환영합니다.");
            Print(7, ""); Print(7, "");
            Print(13, "            이곳의 성주로부터");
        }
    }

    if (mapname == "HdsGate" || mapname == "HDSGATE") {
        if (at(13, 22)) {
            Print(15, "      진정한 길은 마음으로 보아야한다");
            Print(15, "              당신 주위에 있는       ");
            Print(15, "       인위적인 시각 요소를 배제하라");
            Print(15, "          그리고 다시 눈을 떴을때");
            Print(15, "            이미 길은 당신 앞에");
            Print(15, "             펼쳐져 있을 것이다");
        }
    }

    if (mapname == "Valiant" || mapname == "VALIANT" || mapname == "Light" || mapname == "LIGHT") {
        if (at(39, 8)) Print(15, " 이곳은 로어 대륙으로 통하는 게이트 입니다.");
        if (at(39, 68)) {
            Print(7, "");
            Print(15, "   여기는 가이아 테라의 배리언트 피플즈");
            Print(15, "  불의에 대해 가장 강력한 저항을 하는 곳");
            Print(7, "");
            Print(15, "           여러분을 환영합니다.");
            Print(7, ""); Print(7, "");
            Print(13, "            이곳의 성주로부터");
        }
    }

    if (mapname == "Gaea" || mapname == "GAEA") {
        if (at(24, 26)) {
            Print(7, "");
            Print(15, "        가이아 테라성의 보물창고");
            Print(15, "            훔쳐가면 죽어 ~~~");
        }
        if (at(29, 26)) {
            Print(7, "");
            Print(15, "   가이아 테라성에 오신 것을 환영합니다");
            Print(15, "    이곳은 쓰레기 종량제 시범 성입니다");
            Print(7, "");
            Print(15, "           여러분을 환영합니다.");
            Print(7, ""); Print(7, "");
            Print(13, "            이곳의 성주로부터");
        }
    }

    if (mapname == "Ground3" || mapname == "GROUND3") {
        Print(7, ""); Print(7, "");
        Print(15, "   이 광산은 아직 개발 중이라 위험하므로");
        Print(15, "              출입을 금지함");
    }

    if (mapname == "Kobold" || mapname == "KOBOLD") {
        if (at(9, 30)) {
            Print(7, ""); Print(7, "");
            cPrint(10, 11, " 여기서는 믿을만한 기술이 ", "투시", " 밖에는 없소");
            Print(10, "    상당히 난해한 곳이 많으니 주의하시오");
            Print(7, ""); Print(7, ""); Print(7, "");
            Print(13, "                  로드안");
        }
        if (at(54, 63)) {
            Print(7, "");
            Print(7, "  당신이 원한다면");
            Print(7, "         당신 앞의 용암을 건너게 해주겠소");
            m[0] = ""; // Array init missed in initial port
            m[1] = "     용암을 건너겠다";
            m[2] = "     건널 필요는 없다";
            int k_val = Select(180, 2, 2, false, true);
            if (k_val == 1) { pas_x = 58; pas_y = 63; Silent_Scroll(); }
            else PutMap(48, 73, 0);
        }
    }

    if (mapname == "DracTown" || mapname == "DRACTOWN") {
        if (at(92, 94)) {
            Print(7, ""); Print(7, "");
            Print(15, "     이곳은 신성한 드라코니안 묘지이므로");
            Print(15, "          함부로 회손 시키지 마시오");
        }
    }

    if (mapname == "Imperium" || mapname == "IMPERIUM") {
        if (at(24, 19) || at(26, 19)) {
            Print(7, ""); Print(7, "");
            Print(11, "                크리스탈 가게");
            Print(15, "                Crystal  Shop");
            Print(7, "      ( 국제화시대에 부응하기 위해 썼음 )  ");
        }
        if (at(34, 23)) {
            Print(7, ""); Print(7, "");
            Print(11, "                도서관");
        }
    }
}
