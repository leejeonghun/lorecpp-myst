#include "mysttime.h"
#include "mystsub.h"
#include "uhanx.h"
#include "mystbatt.h"
#include "mystend.h"
#include "voice.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>

void ViewMap(std::string Map_no, int _x, int _y) {
    std::string upper_filename = Map_no;
    for (auto& c : upper_filename) c = toupper((unsigned char)c);
    std::string mfile = "assets/" + upper_filename + ".M";
    std::ifstream f(mfile, std::ios::binary);
    if (!f) return;

    Map_Header_Type aux_header;
    f.read((char*)&aux_header, sizeof(aux_header));
    int X_len = aux_header.xmax;
    
    int start_x = _x - 4;
    int start_y = _y - 5;
    uint8_t map_buf[11][9];

    for (int j_idx = 0; j_idx < 11; j_idx++) {
        int r = start_y + j_idx;
        if (r >= 1 && r <= aux_header.ymax) {
            f.seekg(sizeof(Map_Header_Type) + (r - 1) * X_len + (start_x - 1));
            f.read((char*)map_buf[j_idx], 9);
        } else {
            std::memset(map_buf[j_idx], 55, 9);
        }
    }

    bool Dark = !(party.hour >= 7 && party.hour <= 17);
    if (Dark && !((party.day % 30 + 1) >= 10 && (party.day % 30 + 1) <= 20)) {
        for (int j_idx = 0; j_idx < 11; j_idx++) std::memset(map_buf[j_idx], 55, 9);
    }

    _IMAGE_DIRECTION = _STORE;
    for (int j_idx = 0; j_idx < 11; j_idx++) {
        for (int i_idx = 0; i_idx < 9; i_idx++) {
            Image(i_idx + 1, j_idx + 1, &font[map_buf[j_idx][i_idx] & 0x7F], Dark);
        }
    }
    _IMAGE_DIRECTION = _DIRECT;
    for (int j_idx = 0; j_idx < 11; j_idx++) {
        for (int i_idx = 0; i_idx < 9; i_idx++) {
            Image(i_idx + 1, j_idx + 1, &font[map_buf[j_idx][i_idx] & 0x7F], Dark);
        }
    }
    Clear();
    Print(7, " 당신은 수정구슬을 통해 낯선 풍경을 보았다.");
    PressAnyKey();
    OverLap(600, true);
    Silent_Scroll();
}

void Time_Event(byte event_no) {
    int battle_res = 0;
    switch (event_no) {
        case 0: {
            Clear();
            Print(7, " 갑자기 허공 속에서  에인션트 이블의 음성이");
            Print(7, "들려왔다.");
            Delay(1000);
            PressAnyKey();
            {
                std::string p1name = FromPas(player[1].name, player[1].name_len);
                Print(9, " " + p1name + ", 정말 급한 일이 생겼습니다.");
            }
            Print(9, " 방금 당신이 로드안에 대해 반란을 결심한 것");
            Print(9, "을 로드안이 알아챘습니다.  아마 그는 천리안");
            Print(9, "과 독심으로  당신을 계속  관찰했던것 같습니");
            Print(9, "다.  지금 로어성에서는 당신과 배리언트 피플");
            Print(9, "즈의 사람들을 제거할 용사들을 모으기 시작했");
            Print(9, "습니다.  그리고 적어도 몇일후면 이곳으로 진");
            Print(9, "격한다고 합니다.  당신의 능력이라면  충분히");
            Print(9, "자신을 방어할 수 있겠지만  모든 배리언트 피");
            Print(9, "플즈의 사람들을 보호해 주기란 역부족 일것입");
            Print(9, "니다.  그래서 한시바삐 이곳 사람들을 안전한");
            Print(9, "곳으로 이동 시켜 주었으면 합니다. 당신은 그");
            Print(9, "들이 살기 좋을만하고 절대로 로드안의 투시능");
            Print(9, "력이 미치지 않을 곳을 찾아 주십시요. 그리고");
            Print(9, "그 위치를 알려주시면 나머지는 제가 담당하겠");
            Print(9, "습니다.");
            Print(9, "");
            PressAnyKey();
            mapname = "HdsGate";
            Load();
            Silent_Scroll();
            Print(9, " 지금 당신이 서있는 곳은  '하데스 게이트'라");
            Print(9, "는 곳입니다. 이곳은 로어세계의 6번째 테라인");
            Print(9, "하데스 테라로  이동하는 문이  있는 곳이기도");
            Print(9, "합니다. 하데스 테라는 로어 대륙의 지하에 있");
            Print(9, "으며  로드안과 나의 투시능력이  전혀 통하지");
            Print(9, "못하는 곳입니다.  당신은 이곳을 통해 하데스");
            Print(9, "테라로 간후 배리언트 피플즈의 주민들이 안전");
            Print(9, "하게  살 수 있는 곳을  찾아내면 되는 것입니");
            Print(9, "다. 아마 당신의 투시능력이 큰 도움을 줄것입");
            Print(9, "니다.  제가 당신에게 크로매틱 크리스탈을 드");
            Print(9, "리겠습니다.  당신이 가장 좋은 장소를 찾았을");
            Print(9, "때 이 크리스탈을 사용하십시요.  그리면 저의");
            Print(9, "투시력과  그 크리스탈이 서로 교감하여  나의");
            Print(9, "투시력이 그곳으로 작용할 것입니다.");
            Print(9, " 당신의 성공을 빌겠습니다.");
            Print(9, "");
            party.crystal[8]++;
            PressAnyKey();
            break;
        }
        case 1: {
            mapname = "Dome";
            Load();
            for (int j_idx = 1; j_idx <= Map_Header.ymax; j_idx++) {
                for (int i_idx = 1; i_idx <= Map_Header.xmax; i_idx++) {
                    if (Real_Map(i_idx, j_idx) == 0) PutMap(i_idx, j_idx, 47);
                }
            }
            PutMap(26, 14, 35);
            PutMap(13, 27, 44);
            PutMap(22, 35, 47);
            PutMap(39, 29, 31);
            PutMap(19, 54, 44);
            PutMap(30, 63, 47);
            PutMap(19, 73, 47);
            PutMap(38, 41, 31);
            pas_x = 25;
            pas_y = 50;
            Silent_Scroll();
            Clear();
            Print(9, " 휴...  저의 염력을 총동원하여 어느 정도 사");
            Print(9, "람이 살 수 있는 도시를  건설해  보았습니다.");
            Print(9, "이제는 배리언트 피플즈의 사람들을 여기로 공");
            Print(9, "간이동 시켜 보겠습니다.  이번에도 조금 기다");
            Print(9, "려야 할 것입니다");
            watch_year = party.year;
            watch_day = party.day;
            watch_hour = party.hour;
            watch_min = party.min;
            watch_sec = party.sec;
            Time_Watch = true;
            time_event_no = 2;
            watch_min += 30;
            if (watch_min > 59) {
                watch_min -= 60;
                watch_hour++;
                if (watch_hour > 23) {
                    watch_hour -= 24;
                    watch_day++;
                    if (watch_day > 359) {
                        watch_day -= 360;
                        watch_year++;
                    }
                }
            }
            break;
        }
        case 2: {
            int old_x = pas_x, old_y = pas_y;
            mapname = "Dome";
            Load();
            pas_x = old_x; pas_y = old_y;
            while (!(Real_Map(pas_x, pas_y) >= 27 && Real_Map(pas_x, pas_y) <= 47)) pas_x++;
            Silent_Scroll();
            Clear();
            Print(9, " 이곳으로 오기 원하는  모든 배리언트 피플즈");
            Print(9, "의 사람들을 이곳으로 불렀습니다.  여기의 사");
            Print(9, "람들과 이야기를 나눈후 성밖으로 나올때 당신");
            Print(9, "을 지상세계로 이동 시켜 주겠습니다. 좋은 이");
            Print(9, "야기 많이 나누시기 바랍니다.");
            setbit(32);
            break;
        }
        case 3: {
            Clear();
            Print(7, " 당신이 길을 가던중 당신의 동료였던 그 사람");
            Print(7, "들을 만나게 되었다.");
            Delay(1000);
            PressAnyKey();
            {
                std::string p1name = FromPas(player[1].name, player[1].name_len);
                Print(6, " 드디어 만났군, " + p1name + ".");
            }
            Print(6, " 로드안을 배신하겠다는 자네의 생각이 옳았다");
            Print(6, "네.  우리가 자네와 헤어진후 로드안에게 갔을");
            Print(6, "때  로드안은 우리에게 멸망당한 네 종족의 원");
            Print(6, "혼을 봉인하는 일을 시켰다네. 우리는 쾌히 승");
            Print(6, "락하며 그 일을 해주었지. 하지만 우리의 도움");
            Print(6, "이 더 이상 필요없게 되자 로드안은 그 자신을");
            Print(6, "위해  우리의 동료였던 카노푸스를  악의 전사");
            Print(6, "네크로만서로 개조하여 다른 차원으로 보내 버");
            Print(6, "렸다네.  우리는 그 사실을 알아채자 로어성을");
            Print(6, "탈출하여 자네를 찾아 여기까지 왔다네.  자네");
            Print(6, "와 다시 합류하여 로드안과 싸우고 싶네.");
            for (int i_idx = 1; i_idx <= 5; i_idx++) Print(6, "");
            m[0] = "";
            m[1] = "쾌히 승락하겠네";
            m[2] = "하지만 이미 늦었네";
            if (Select(230, 2, 2, false, true) != 2) {
                // name_hidden에 백업된 원래 이름 복원
                for (int i_idx = 2; i_idx <= 5; i_idx++) {
                    player[i_idx].name_len = player[i_idx].name_hidden_len;
                    std::memcpy(player[i_idx].name, player[i_idx].name_hidden,
                                player[i_idx].name_hidden_len);
                    if (player[i_idx].name_hidden_len < 17)
                        player[i_idx].name[player[i_idx].name_hidden_len] = '\0';
                    player[i_idx].name_hidden_len = 0;
                }
                Display_Condition();
                setbit(49);
            }
            watch_year = party.year;
            watch_day = party.day;
            watch_hour = party.hour;
            watch_min = party.min;
            watch_sec = party.sec;
            Time_Watch = true;
            time_event_no = 4;
            watch_min += 30;
            if (watch_min > 59) {
                watch_min -= 60;
                watch_hour++;
                if (watch_hour > 23) {
                    watch_hour -= 24;
                    watch_day++;
                    if (watch_day > 359) {
                        watch_day -= 360;
                        watch_year++;
                    }
                }
            }
            break;
        }
        case 4: {
            Clear();
            Print(7, " 당신이 길을 가던중 갑자기 강한 기운이 둘러");
            Print(7, "싸고 있음을 알아챘다.  기척을 없에고 투시로");
            Print(7, "주위를 보았을때 사방을 포위하고 있는 로어성");
            Print(7, "의 병사들을 볼 수 있었다. 로드안이 천리안으");
            Print(7, "로  우리들을 찾아내어  이곳으로 대군을 보낸");
            Print(7, "것임에 틀림없었다.  우리들은 곧 벌어질 전투");
            Print(7, "를 위해 태세를 갖추었다. 당신이 투시로 주위");
            Print(7, "를 경계하던중 놀랍게도 대군을 직접 지휘하는");
            Print(7, "로드안을 보았다.  그가 직접 나섰다면 그만큼");
            Print(7, "나의 승산도 줄어든다는걸  알고있었다.  이제");
            Print(7, "결론은 단 한가지였다.  바로 로드안이 있는쪽");
            Print(7, "으로 뚫고 들어가 그와 상대하는 것이었다. 그");
            Print(7, "리고 내가 그를 꺽어버린다면 대부분의 병사들");
            Print(7, "은 사기를 잃고  도망가버릴 것이기 때문이다.");
            Print(7, "일행은  서로에게 신호를 보내며  나의 생각에");
            Print(7, "동의를 표했다. 이제는 지체할 시간이 없었다.");
            Print(7, "우리들은  곧바로 로드안이 있는 쪽을 향해 뚫");
            Print(7, "고 들어가기 시작했다.");
            Print(7, "");
            PressAnyKey();

            // RunBattle 람다 : 0 = 계속, 1 = JUMP_TO_THE_FUTURE(강제패배이벤트 등), 2 = 도망(종료)
            auto RunBattle = [&](int count, int e1, int c1, int e2=0, int c2=0, int e3=0, int c3=0, bool no_run=false) -> int {
                for (int b_idx = 0; b_idx < count; b_idx++) {
                    Clear();
                    enemynumber = c1 + c2 + c3;
                    int idx = 1;
                    for (int k = 0; k < c1; k++) joinenemy(idx++, e1);
                    for (int k = 0; k < c2; k++) joinenemy(idx++, e2);
                    for (int k = 0; k < c3; k++) joinenemy(idx++, e3);
                    
                    do {
                        DisplayEnemies(true);
                        BattleMode(!no_run, false);
                        if (party.etc[6] == 254) return 2; // 도망 시 바로 함수 이탈
                        if (party.etc[6] == 2) {
                            Clear();
                            Print(7, " 일행은 너무 많은 적들에게 포위되어서 도망갈 수가 없었다.");
                            PressAnyKey();
                        }
                    } while (party.etc[6] != 0);
                    
                    EndBattle(battle_res);
                    if (battle_res == 1) return 1; // 패배시 미래로 JUMP 플래그 반환
                }
                return 0; // 이기면 다음 전투로 진행
            };

            int jump_flag = 0;

            // 1. 적 67(8) * 5회 반복
            jump_flag = RunBattle(5, 67, 8);
            if (jump_flag == 2) return;
            if (jump_flag == 1) goto JUMP_TO_THE_FUTURE;

            // 2. 적 67(6), 68(1), 69(1) * 4회 반복
            jump_flag = RunBattle(4, 67, 6, 68, 1, 69, 1);
            if (jump_flag == 2) return;
            if (jump_flag == 1) goto JUMP_TO_THE_FUTURE;

            // 3. 적 69(4), 68(4) * 2회 반복
            jump_flag = RunBattle(2, 69, 4, 68, 4);
            if (jump_flag == 2) return;
            if (jump_flag == 1) goto JUMP_TO_THE_FUTURE;

            // 4. 적 68(4), 69(3), 70(1) * 1회 (도망불가: BattleMode(FALSE, FALSE))
            jump_flag = RunBattle(1, 68, 4, 69, 3, 70, 1, true);
            if (jump_flag == 2) return;
            if (jump_flag == 1) goto JUMP_TO_THE_FUTURE;

            // 5. 적 71(6), 72(1), 73(1) * 1회 
            jump_flag = RunBattle(1, 71, 6, 72, 1, 73, 1);
            if (jump_flag == 2) return;
            if (jump_flag == 1) goto JUMP_TO_THE_FUTURE;

            // 모든 연속전투를 이겼을 경우 -> 진엔딩 루트
            Clear();
            Print(7, " 당신은  힘겹게 로드안을 물리쳤다.  그리고");
            Print(7, "예상대로  로어성의 병사들은  자신의 군주가");
            Print(7, "패하자 뿔뿔이 흩어져 버렸다. 벌판에는 당신");
            Print(7, "이 죽인 수 많은 시체가 널려있었다.");
            mapname = "Lore";
            Load();
            pas_x = 51; pas_y = 31;
            PutMap(51, 28, 42);
            PressAnyKey();
            OverLap(1000, false);
            Print(7, "얼마후 로어성에는 성대한 의식이 행해졌다.");
            
            face = 1;
            if (player[1].classtype == magic) face += 8;
            for (int j_idx = 11; j_idx >= 3; j_idx--) {
                if (j_idx < 11) PutTile(5, j_idx + 1, 42);
                PutSprite(5, j_idx, face);
                Delay(800);
            }
            
            face = 0;
            if (player[1].classtype == magic) face += 8;
            PutTile(5, 3, 42);
            PutSprite(5, 3, face);
            
            Print(7, " 그것은  바로 새로운 군주를 탄생 시키는 자");
            Print(7, "리였다. 진정한 선을 행할 수 있는 자만이 설");
            Print(7, "수 있는 자리.");
            Delay(1500);
            
            for (int i_idx = 1; i_idx <= 4; i_idx++) {
                if (i_idx > 1) PutTile(i_idx - 1, 3, Real_Map(pas_x - 5 + i_idx - 1, pas_y - 3));
                PutSprite(i_idx, 3, 21);
                Delay(800);
            }
            Delay(1500);
            PutTile(5, 3, 54);
            
            for (int i_idx = 4; i_idx >= 1; i_idx--) {
                PutTile(i_idx, 3, Real_Map(pas_x - 5 + i_idx, pas_y - 3));
                if (i_idx > 1) {
                    PutSprite(i_idx - 1, 3, 22);
                }
                Delay(800);
            }
            Print(7, " 그 의식의 주인공은 바로 당신이었다.  새로");
            Print(7, "운  로어 세계의 역사를 창조하는  그 순간을");
            Print(7, "모든 사람들은 두손 모아 찬미하였다.");
            PressAnyKey();
            
            End_Demo(2);
            return; // 엔딩 연출 이후 완전 종료 처리

// 미래로 시간 점프하는 이벤트 처리부 (패배 연출의 일부)
JUMP_TO_THE_FUTURE:
            Scroll(true);
            Clear();
            Print(7, " 이제는 로드안에게 패하는구나하며 모든것을");
            Print(7, "단념하려는 순간  빛나는 광채가 당신을 둘러");
            Print(7, "싸기 시작했다. 그리고는 빨려 들어가듯 당신");
            Print(7, "을 감싸던 광채는 차원의 틈속으로 당신을 몰");
            Print(7, "아넣었다.");
            Delay(1000);
            PressAnyKey();
            Delete_Scroll_Screen();
            
            mapname = "Ground1";
            Load();
            pas_x = 58; pas_y = 79;
            party.year = 795;
            party.day = 18;
            party.hour = 9;
            party.min = 0;
            party.sec = 0;
            
            for (int i_idx = 1; i_idx <= 6; i_idx++) {
                if (player[i_idx].name_len > 0) {
                    player[i_idx].hp = 1;
                    player[i_idx].poison = 0;
                    player[i_idx].unconscious = 0;
                    player[i_idx].dead = 0;
                }
            }
            
            Display_Condition();
            Print(7, " 얼마나 아득한 순간이 지났을까...");
            Print(7, " 당신은 문득 정신을 차렸다.  그리고 당신이");
            Print(7, "본 것은 뭔가 낯선듯한 로어 대륙의 모습이었");
            Print(7, "다.");
            OverLap(500, true);
            PressAnyKey();
            Print(7, " 당신의 귓가에서 귀에 익은 목소리가 울려퍼");
            Print(7, "졌다.");
            Print(7, "");
            Print(10, " 나는 당신을 이런 운명으로 끌어들였던 알비");
            Print(10, "레오라는 예언자이오. 물론 당신은 나를 알겠");
            Print(10, "지요.  나는 이곳의 존재가 아니라서  이곳의");
            Print(10, "역사에 관여할 자격은 없소.  하지만  당신의");
            Print(10, "위급한 상황을 보고는 참을 수가 없어서 차원");
            Print(10, "이탈 마법으로 당신을 795년으로 오게했소.");
            Print(10, "");
            Print(10, " 이미 로어 세계는  당신을 잊혀진 인물로 평");
            Print(10, "가할 정도의 세월이 지났소.  결코 당신을 알");
            Print(10, "아볼 사람은 이곳에 없을거요. 세상의 최강자");
            Print(10, "였던 로드안은 이제 기력이 많이 약해져서 지");
            Print(10, "금 당신의 맞수로서 충분하리라 생각하오. 이");
            Print(10, "제 당신은 별 제약없이 로어성으로 들어갈 수");
            Print(10, "있을거요.");
            Print(10, " 거기서  최후의 결전을 벌여  진정한 정의를");
            Print(10, "실현하시오. 행운을 빌겠소.");
            Print(10, "");
            PressAnyKey();
            break;
        }
        case 5: {
            switch (party.etc[26]) {
                case 0:
                    if (mapname == "Ground2" && on(92, 12)) {
                        enemynumber = 1;
                        joinenemy(1, 60);
                        do {
                            DisplayEnemies(true);
                            BattleMode(false, true);
                            if (party.etc[6] == 254) return;
                            if (party.etc[6] == 2) Clear();
                        } while (party.etc[6] != 0);
                        Clear();
                        Print(15, " 당신은 오크의 원혼을 크리스탈 볼 속에 봉인");
                        Print(15, "시켰다.");
                        party.etc[26]++;
                    } else {
                        ViewMap("Ground2", 92, 12);
                    }
                    break;
                case 1:
                    if (mapname == "Ground3" && on(35, 34)) {
                        enemynumber = 1;
                        joinenemy(1, 61);
                        do {
                            DisplayEnemies(true);
                            BattleMode(false, true);
                            if (party.etc[6] == 254) return;
                            if (party.etc[6] == 2) Clear();
                        } while (party.etc[6] != 0);
                        Clear();
                        Print(15, " 당신은 트롤의 원혼을 크리스탈 볼 속에 봉인");
                        Print(15, "시켰다.");
                        party.etc[26]++;
                    } else {
                        ViewMap("Ground3", 35, 34);
                    }
                    break;
                case 2:
                    if (mapname == "Ground4" && on(11, 91)) {
                        enemynumber = 1;
                        joinenemy(1, 62);
                        do {
                            DisplayEnemies(true);
                            BattleMode(false, true);
                            if (party.etc[6] == 254) return;
                            if (party.etc[6] == 2) Clear();
                        } while (party.etc[6] != 0);
                        Clear();
                        Print(15, " 당신은 코볼트의 원혼을 크리스탈 볼 속에 봉");
                        Print(15, "인 시켰다.");
                        party.etc[26]++;
                    } else {
                        ViewMap("Ground4", 11, 91);
                    }
                    break;
                case 3:
                    if (mapname == "Ground1" && on(44, 50)) {
                        enemynumber = 1;
                        joinenemy(1, 63);
                        do {
                            DisplayEnemies(true);
                            BattleMode(false, true);
                            if (party.etc[6] == 254) return;
                            if (party.etc[6] == 2) Clear();
                        } while (party.etc[6] != 0);
                        Clear();
                        Print(15, " 당신은 드라콘의 원혼을 크리스탈 볼 속에 봉");
                        Print(15, "인 시켰다.");
                        party.etc[26]++;
                        party.etc[20] = 11;
                    } else {
                        ViewMap("Ground1", 44, 50);
                    }
                    break;
                default:
                    Clear();
                    Print(7, " 크리스탈 볼 속에는  아무것도 떠오르지 않았");
                    Print(7, "다.");
                    break;
            }
            break;
        }
    }
}