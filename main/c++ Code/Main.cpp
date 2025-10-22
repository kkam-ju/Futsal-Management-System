#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <map>

using namespace std;

// =====================================================================
// 클래스 및 구조체, 전역 정의
// =====================================================================

enum class LogicalPosition {
    ST, LW, RW, CM, LB, RB, GK, COUNT
};
const int NUM_POSITIONS = (int)LogicalPosition::COUNT;

class Player {
private:
    string name;
    int height;
    int weight;
    string bodyType;
    int number;
public:
    Player(string n = "", int h = 0, int w = 0, string b = "", int num = 0)
        : name(n), height(h), weight(w), bodyType(b), number(num) {
    }
    void showInfo(int idx) const {
        cout << idx + 1 << ". 이름: " << name
            << " | 키: " << height << " | 몸무게: " << weight
            << " | 체형: " << bodyType << " | 등번호: " << number << endl;
    }
    void editInfo(string n, int h, int w, string b, int num) {
        name = n; height = h; weight = w; bodyType = b; number = num;
    }
    int getNumber() const { return number; }
    string getName() const { return name; }
    int getHeight() const { return height; }
    int getWeight() const { return weight; }
};

class PlayerManager {
private:
    vector<Player> players;
public:
    void addPlayer(string name, int height, int weight, string bodyType, int number) {
        for (const auto& p : players) {
            if (p.getNumber() == number) {
                cout << "이미 사용 중인 등번호입니다. 등록 실패!\n";
                return;
            }
        }
        players.push_back(Player(name, height, weight, bodyType, number));
        cout << "참가자가 등록되었습니다!" << endl;
    }
    void showPlayers() const {
        cout << "\n----- 참가자 명단 -----\n";
        if (players.empty()) {
            cout << "현재 등록된 참가자가 없습니다.\n";
        }
        else {
            for (size_t i = 0; i < players.size(); i++) {
                players[i].showInfo(i);
            }
        }
        cout << "-----------------------\n";
    }
    bool editPlayer(int idx, string n, int h, int w, string b, int num) {
        if (idx >= 0 && idx < (int)players.size()) {
            for (size_t i = 0; i < players.size(); i++) {
                if ((int)i != idx && players[i].getNumber() == num) return false;
            }
            players[idx].editInfo(n, h, w, b, num);
            return true;
        }
        else {
            cout << "잘못된 번호입니다.\n";
            return false;
        }
    }
    void deletePlayer(int idx) {
        if (idx >= 0 && idx < (int)players.size()) {
            cout << "참가자가 삭제되었습니다!" << endl;
            players.erase(players.begin() + idx);
        }
        else {
            cout << "잘못된 번호입니다.\n";
        }
    }
    int getSize() const { return players.size(); }
    const vector<Player>& getPlayers() const { return players; }
};


struct Person {
    string name;
    int height;
    int weight;
    int number; // 등번호 추가
};

struct PricePolicy {
    int basePerHour = 60000;
    int weekendSurcharge = 5000;
    int extraPerPerson10p = 3000;
};

struct ReservationDetails {
    bool isReserved = false;
    string ymd = "";
    int total = 0;
    bool isWeekend = false;
    vector<pair<int, int>> reservedSlots;
    bool timeSlots[15] = { false };

    ReservationDetails(string date = "") : ymd(date) {
        fill(begin(timeSlots), end(timeSlots), false);
        timeSlots[4] = true;
        timeSlots[5] = true;
    }
};


// =====================================================================
// 범용 헬퍼 함수
// =====================================================================
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    cout << "\n아무 키나 누르면 계속합니다...";
    cin.get();
}

void showHeader(const string& title) {
    cout << "\n-------------------------------------\n";
    cout << "        " << title << "\n";
    cout << "-------------------------------------\n";
}

// =====================================================================
// 포메이션 설정 기능 관련 함수
// =====================================================================
map<LogicalPosition, int> getFormationMap(int formationChoice) {
    map<LogicalPosition, int> posMap;
    switch (formationChoice) {
    case 1:
        posMap[LogicalPosition::LW] = 0; posMap[LogicalPosition::RW] = 1;
        posMap[LogicalPosition::ST] = 2;
        posMap[LogicalPosition::LB] = 3; posMap[LogicalPosition::RB] = 4;
        posMap[LogicalPosition::CM] = 5;
        posMap[LogicalPosition::GK] = 6;
        break;
    case 2:
        posMap[LogicalPosition::ST] = 0;
        posMap[LogicalPosition::LW] = 1; posMap[LogicalPosition::CM] = 2; posMap[LogicalPosition::RW] = 3;
        posMap[LogicalPosition::LB] = 4; posMap[LogicalPosition::RB] = 5;
        posMap[LogicalPosition::GK] = 6;
        break;
    case 3:
    default:
        posMap[LogicalPosition::LW] = 0; posMap[LogicalPosition::ST] = 1; posMap[LogicalPosition::RW] = 2;
        posMap[LogicalPosition::LB] = 3; posMap[LogicalPosition::RB] = 4;
        posMap[LogicalPosition::CM] = 5;
        posMap[LogicalPosition::GK] = 6;
        break;
    }
    return posMap;
}

map<int, LogicalPosition> getReverseFormationMap(int formationChoice) {
    map<LogicalPosition, int> forwardMap = getFormationMap(formationChoice);
    map<int, LogicalPosition> reverseMap;
    for (const auto& pair : forwardMap) {
        reverseMap[pair.second] = pair.first;
    }
    return reverseMap;
}

bool compareHeight(const Person& a, const Person& b) { return a.height > b.height; }

// ★★★★★ 선수 목록 출력 시 등번호 표시하도록 수정 ★★★★★
void printNameList(const vector<Person>& nameList, const vector<string>& team1DisplaySlots, const vector<string>& team2DisplaySlots) {
    cout << "[등록된 선수 목록 (포메이션 배치용)]\n";
    if (nameList.empty()) {
        cout << "(없음)\n";
    }
    else {
        vector<string> allPlacedPlayers;
        allPlacedPlayers.insert(allPlacedPlayers.end(), team1DisplaySlots.begin(), team1DisplaySlots.end());
        allPlacedPlayers.insert(allPlacedPlayers.end(), team2DisplaySlots.begin(), team2DisplaySlots.end());

        for (const Person& p : nameList) {
            cout << "- " << p.name << " (등번호: " << p.number << ", "
                << p.height << "cm, " << p.weight << "kg)";
            bool placed = find(allPlacedPlayers.begin(), allPlacedPlayers.end(), p.name) != allPlacedPlayers.end();
            if (placed) cout << " (배치됨)";
            cout << "\n";
        }
    }
    cout << endl;
}

vector<string> getFormationLines(int formationChoice, const vector<string>& slots, const string& teamName) {
    vector<string> lines;
    auto getName = [&](int idx) -> string {
        if (idx >= 0 && idx < (int)slots.size() && !slots[idx].empty()) return slots[idx];
        return " ";
        };
    lines.push_back("[" + teamName + " - 포메이션 " + to_string(formationChoice) + "]");
    switch (formationChoice) {
    case 1:
        lines.push_back("[1]" + getName(0) + "        [2]" + getName(1));
        lines.push_back("      [3]" + getName(2));
        lines.push_back("[4]" + getName(3) + "        [5]" + getName(4));
        lines.push_back("      [6]" + getName(5));
        lines.push_back("      [7]" + getName(6));
        break;
    case 2:
        lines.push_back("        [1]" + getName(0));
        lines.push_back("[2]" + getName(1) + "    [3]" + getName(2) + "    [4]" + getName(3));
        lines.push_back("    [5]" + getName(4) + "    [6]" + getName(5));
        lines.push_back("        [7]" + getName(6));
        break;
    case 3:
        lines.push_back("[1]" + getName(0) + "    [2]" + getName(1) + "    [3]" + getName(2));
        lines.push_back("    [4]" + getName(3) + "    [5]" + getName(4));
        lines.push_back("        [6]" + getName(5));
        lines.push_back("        [7]" + getName(6));
        break;
    }
    return lines;
}

void printFormationsSideBySide(int f1, const vector<string>& team1Slots, int f2, const vector<string>& team2Slots) {
    vector<string> left = getFormationLines(f1, team1Slots, "팀1");
    vector<string> right = getFormationLines(f2, team2Slots, "팀2");
    size_t maxLines = max(left.size(), right.size());
    for (size_t i = 0; i < maxLines; ++i) {
        string l = (i < left.size()) ? left[i] : "";
        string r = (i < right.size()) ? right[i] : "";
        cout << l;
        if (l.length() < 40) cout << string(40 - l.length(), ' ');
        cout << r << "\n";
    }
    cout << "\n";
}

void showFormationOptions() {
    cout << "--- 선택 가능한 포메이션 ---\n\n";
    cout << "1번 포메이션 (2-1-2-1-1)" << " 2번 포메이션 (1-3-2-1)" << " 3번 포메이션 (3-2-1-1)\n";
    cout << "   [1]           [2]      " << "          [1]         " << "   [1]    [2]   [3]\n";
    cout << "          [3]             " << "   [2]    [3]    [4]  " << "       [4]   [5]\n";
    cout << "   [4]           [5]      " << "      [5]     [6]     " << "          [6]\n";
    cout << "          [6]             " << "          [7]         " << "          [7]\n";
    cout << "          [7]           \n";
}


// =====================================================================
// 메뉴별 기능 함수
// =====================================================================

string inputBodyType() {
    string bodyType;
    while (true) {
        cout << "체형 (마름/보통/건장): ";
        cin >> bodyType;
        if (bodyType == "마름" || bodyType == "보통" || bodyType == "건장") return bodyType;
        else cout << "잘못된 입력입니다. 다시 입력해주세요." << endl;
    }
}

void adminMenu(PlayerManager& pm) {
    int choice;
    while (true) {
        clearScreen();
        cout << "===== 관리자 창 =====\n1. 참가자 수정\n2. 참가자 삭제\n3. 관리자 창 종료\n선택 : ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (choice == 1) {
            pm.showPlayers();
            cout << "수정할 참가자 번호: ";
            int idx; cin >> idx; idx--;
            string n; int h, w, num;
            cout << "새 이름: "; cin >> n;
            cout << "새 키: "; cin >> h;
            cout << "새 몸무게: "; cin >> w;
            string b = inputBodyType();
            while (true) {
                cout << "새 등번호: "; cin >> num;
                if (!pm.editPlayer(idx, n, h, w, b, num)) cout << "등번호가 중복되었습니다. 다시 입력하세요.\n";
                else { cout << "참가자 정보가 수정되었습니다!" << endl; break; }
            }
        }
        else if (choice == 2) {
            pm.showPlayers();
            cout << "삭제할 참가자 번호: ";
            int idx; cin >> idx; idx--;
            pm.deletePlayer(idx);
        }
        else if (choice == 3) {
            cout << "관리자 창 종료" << endl; break;
        }
        else {
            cout << "잘못된 입력입니다." << endl;
        }
        pause();
    }
}

void formationMenu(PlayerManager& pm,
    vector<string>& logicalSlotsTeam1,
    vector<string>& logicalSlotsTeam2,
    int& formationChoice1,
    int& formationChoice2) {
    clearScreen();
    const vector<Player>& players = pm.getPlayers();
    if (players.empty()) {
        cout << "먼저 참가자를 1명 이상 등록해주세요.\n";
        pause();
        return;
    }

    showFormationOptions();
    cout << "== 팀1 포메이션 선택 (현재: " << formationChoice1 << ") ==\n";
    while (true) {
        cout << "선택 (1~3): "; cin >> formationChoice1;
        if (cin.good() && formationChoice1 >= 1 && formationChoice1 <= 3) break;
        cout << "1~3 사이의 숫자를 입력하세요.\n";
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "\n== 팀2 포메이션 선택 (현재: " << formationChoice2 << ") ==\n";
    while (true) {
        cout << "선택 (1~3): "; cin >> formationChoice2;
        if (cin.good() && formationChoice2 >= 1 && formationChoice2 <= 3) break;
        cout << "1~3 사이의 숫자를 입력하세요.\n";
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore();

    string input;
    while (true) {
        clearScreen();
        cout << "== 선수 배치 시스템 ==\n\n";

        vector<string> displaySlotsTeam1(NUM_POSITIONS, "");
        vector<string> displaySlotsTeam2(NUM_POSITIONS, "");
        map<LogicalPosition, int> map1 = getFormationMap(formationChoice1);
        map<LogicalPosition, int> map2 = getFormationMap(formationChoice2);

        for (int i = 0; i < NUM_POSITIONS; ++i) {
            LogicalPosition currentPos = (LogicalPosition)i;
            if (!logicalSlotsTeam1[i].empty()) displaySlotsTeam1[map1[currentPos]] = logicalSlotsTeam1[i];
            if (!logicalSlotsTeam2[i].empty()) displaySlotsTeam2[map2[currentPos]] = logicalSlotsTeam2[i];
        }

        printFormationsSideBySide(formationChoice1, displaySlotsTeam1, formationChoice2, displaySlotsTeam2);

        // ★★★★★ Person 객체 생성 시 등번호(getNumber)도 함께 전달 ★★★★★
        vector<Person> nameList;
        for (const auto& p : players) {
            nameList.push_back({ p.getName(), p.getHeight(), p.getWeight(), p.getNumber() });
        }
        sort(nameList.begin(), nameList.end(), compareHeight);
        printNameList(nameList, displaySlotsTeam1, displaySlotsTeam2);

        cout << "명령어: 배치 [이름 또는 등번호] [팀] [위치] | 제거 [팀] [위치] | 교체 [팀1] [위치1] [팀2] [위치2] | 나가기\n";
        cout << "명령어 입력: ";
        getline(cin, input);

        if (input == "나가기") break;

        stringstream ss(input);
        string command;
        ss >> command;

        if (command == "배치") {
            string identifier; int teamNum, pos;
            ss >> identifier >> teamNum >> pos;
            if (identifier.empty() || ss.fail()) continue;

            string targetPlayerName = "";
            int playerNumber = -1;
            try {
                playerNumber = stoi(identifier);
            }
            catch (...) {
                playerNumber = -1;
            }

            if (playerNumber != -1) {
                bool found = false;
                for (const auto& p : players) {
                    if (p.getNumber() == playerNumber) {
                        targetPlayerName = p.getName();
                        found = true;
                        break;
                    }
                }
                if (!found) continue;
            }
            else {
                bool found = false;
                for (const auto& p : players) {
                    if (p.getName() == identifier) {
                        targetPlayerName = identifier;
                        found = true;
                        break;
                    }
                }
                if (!found) continue;
            }

            bool isAlreadyPlaced = false;
            for (const auto& playerName : logicalSlotsTeam1) if (playerName == targetPlayerName) isAlreadyPlaced = true;
            for (const auto& playerName : logicalSlotsTeam2) if (playerName == targetPlayerName) isAlreadyPlaced = true;
            if (isAlreadyPlaced) continue;

            if (teamNum != 1 && teamNum != 2) continue;
            int displaySlotIndex = pos - 1;

            map<int, LogicalPosition> reverseMap = getReverseFormationMap(teamNum == 1 ? formationChoice1 : formationChoice2);
            if (reverseMap.count(displaySlotIndex)) {
                LogicalPosition targetPos = reverseMap[displaySlotIndex];
                if ((int)targetPos < logicalSlotsTeam1.size()) {
                    vector<string>& targetLogicalSlots = (teamNum == 1) ? logicalSlotsTeam1 : logicalSlotsTeam2;
                    targetLogicalSlots[(int)targetPos] = targetPlayerName;
                }
            }
        }
        else if (command == "제거") {
            int teamNum, pos;
            ss >> teamNum >> pos; if (ss.fail()) continue;
            int displaySlotIndex = pos - 1;
            map<int, LogicalPosition> reverseMap = getReverseFormationMap(teamNum == 1 ? formationChoice1 : formationChoice2);
            if (reverseMap.count(displaySlotIndex)) {
                LogicalPosition targetPos = reverseMap[displaySlotIndex];
                if ((int)targetPos < logicalSlotsTeam1.size()) {
                    vector<string>& targetLogicalSlots = (teamNum == 1) ? logicalSlotsTeam1 : logicalSlotsTeam2;
                    targetLogicalSlots[(int)targetPos] = "";
                }
            }
        }
        else if (command == "교체") {
            int t1, p1, t2, p2;
            ss >> t1 >> p1 >> t2 >> p2; if (ss.fail()) continue;
            if (!(t1 >= 1 && t1 <= 2 && t2 >= 1 && t2 <= 2 && p1 >= 1 && p1 <= 7 && p2 >= 1 && p2 <= 7)) continue;

            map<int, LogicalPosition> rMap1 = getReverseFormationMap(t1 == 1 ? formationChoice1 : formationChoice2);
            map<int, LogicalPosition> rMap2 = getReverseFormationMap(t2 == 1 ? formationChoice1 : formationChoice2);

            if (rMap1.count(p1 - 1) && rMap2.count(p2 - 1)) {
                LogicalPosition pos1 = rMap1[p1 - 1];
                LogicalPosition pos2 = rMap2[p2 - 1];
                if ((int)pos1 < logicalSlotsTeam1.size() && (int)pos2 < logicalSlotsTeam1.size()) {
                    string& name1 = (t1 == 1) ? logicalSlotsTeam1[(int)pos1] : logicalSlotsTeam2[(int)pos1];
                    string& name2 = (t2 == 1) ? logicalSlotsTeam1[(int)pos2] : logicalSlotsTeam2[(int)pos2];
                    swap(name1, name2);
                }
            }
        }
    }
}

void makeNewReservation(int totalPlayers, map<string, ReservationDetails>& allReservations) {
    PricePolicy price;
    auto clearCin = []() { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); };
    auto inputLine = [&](const string& prompt) {
        cout << prompt;
        string s; getline(cin, s);
        while (s.empty()) { cout << "  → 입력을 해주세요: "; getline(cin, s); }
        return s;
        };
    auto dayOfWeek = [](int y, int m, int d) {
        static int t[] = { 0,3,2,5,0,3,5,1,4,6,2,4 };
        if (m < 3) y -= 1;
        return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
        };
    auto parseYMD = [&](const string& ymd, int& y, int& m, int& d) {
        if (ymd.size() != 10 || ymd[4] != '-' || ymd[7] != '-') return false;
        try { y = stoi(ymd.substr(0, 4)); m = stoi(ymd.substr(5, 2)); d = stoi(ymd.substr(8, 2)); }
        catch (...) { return false; }
        if (y < 1900 || m < 1 || m>12 || d < 1 || d>31) return false;
        return true;
        };
    auto isWeekendYMD = [&](const string& ymd) {
        int y, m, d;
        if (!parseYMD(ymd, y, m, d)) return false;
        int w = dayOfWeek(y, m, d);
        return(w == 0 || w == 6);
        };
    auto pricePerHour = [&](bool isWeekend, int players) {
        int p = price.basePerHour;
        if (isWeekend) p += price.weekendSurcharge;
        if (players > 10) p += (players - 10) * price.extraPerPerson10p;
        return p;
        };

    clearScreen();
    showHeader("신규 예약 진행");

    string ymd = inputLine("예약 날짜 입력 (YYYY-MM-DD): ");
    int y, m, d;
    while (!parseYMD(ymd, y, m, d)) ymd = inputLine("형식 오류. 예: 2025-08-22 : ");

    if (allReservations.find(ymd) == allReservations.end()) {
        cout << "\n[알림] 새로운 날짜(" << ymd << ")의 예약판을 불러옵니다.\n";
        allReservations[ymd] = ReservationDetails(ymd);
    }
    else {
        cout << "\n[알림] 기존 날짜(" << ymd << ")의 예약판을 이어갑니다.\n";
    }

    ReservationDetails& currentDayDetails = allReservations.at(ymd);
    bool weekend = isWeekendYMD(ymd);

    while (true) {
        cout << "\n[예약 가능 시간대]\n";
        cout << left << setw(6) << "번호" << setw(14) << "시간대" << setw(12) << "/시간 가격" << "상태\n";
        cout << "------------------------------------------------------\n";
        int perHour = pricePerHour(weekend, totalPlayers);
        for (int idx = 1; idx <= 14; ++idx) {
            string slot = to_string(7 + idx) + ":00-" + to_string(8 + idx) + ":00";
            cout << left << setw(6) << idx << setw(14) << slot << setw(12) << (to_string(perHour) + "원") << (currentDayDetails.timeSlots[idx] ? "예약 불가" : "예약 가능") << "\n";
        }

        cout << "원하는 시작 시간대 번호 선택 (종료하려면 0): ";
        int slotNo; cin >> slotNo;
        if (cin.fail() || slotNo < 0 || slotNo > 14) { clearCin(); continue; }
        if (slotNo == 0) break;

        cout << "이용 시간(시간) 입력 (1~2): ";
        int hours; cin >> hours;
        if (cin.fail() || hours < 1 || hours > 2) { clearCin(); continue; }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        int startHour = 7 + slotNo;
        int endHour = startHour + hours;

        bool isReservable = true;
        if (endHour > 22) isReservable = false;
        if (currentDayDetails.timeSlots[slotNo]) isReservable = false;
        if (hours == 2 && (slotNo == 14 || currentDayDetails.timeSlots[slotNo + 1])) isReservable = false;

        if (!isReservable) {
            cout << "\n[안내] 선택한 시간대는 예약할 수 없습니다. 다시 선택해주세요.\n";
            continue;
        }

        int totalForThisSlot = pricePerHour(weekend, totalPlayers) * hours;
        clearScreen();
        showHeader("예약 정보");
        cout << "날짜       : " << ymd << (weekend ? " (주말)\n" : " (평일)\n");
        cout << "시간대     : " << startHour << ":00 ~ " << endHour << ":00 (" << hours << "시간)\n";
        cout << "인원       : " << totalPlayers << "명\n";
        cout << "요금       : " << totalForThisSlot << "원\n";
        cout << "더치페이   : 1인당 " << totalForThisSlot / totalPlayers << "원\n";

        currentDayDetails.isReserved = true;
        currentDayDetails.ymd = ymd;
        currentDayDetails.isWeekend = weekend;
        currentDayDetails.reservedSlots.push_back({ startHour, endHour });
        currentDayDetails.total += totalForThisSlot;

        currentDayDetails.timeSlots[slotNo] = true;
        if (hours == 2) currentDayDetails.timeSlots[slotNo + 1] = true;

        cout << "\n예약되었습니다! (같은 날짜 추가 예약 가능)\n";
        pause();
    }
}

void cancelReservation(map<string, ReservationDetails>& allReservations) {
    clearScreen();
    showHeader("예약 취소");

    if (allReservations.empty()) {
        cout << "취소할 예약 내역이 없습니다.\n";
        pause();
        return;
    }

    cout << "[현재 예약된 날짜 목록]\n";
    bool hasAnyReservation = false;
    for (const auto& pair : allReservations) {
        if (pair.second.isReserved) {
            cout << "- " << pair.first << "\n";
            hasAnyReservation = true;
        }
    }

    if (!hasAnyReservation) {
        cout << "취소할 예약 내역이 없습니다.\n";
        pause();
        return;
    }

    cout << "\n취소를 원하는 날짜를 입력하세요 (YYYY-MM-DD): ";
    string ymd;
    getline(cin, ymd);

    if (allReservations.count(ymd) && allReservations.at(ymd).isReserved) {
        allReservations.erase(ymd);
        cout << "\n" << ymd << " 날짜의 모든 예약이 취소되었습니다.\n";
    }
    else {
        cout << "\n해당 날짜의 예약 내역을 찾을 수 없습니다.\n";
    }
    pause();
}


// =====================================================================
// 메인 프로그램 흐름 제어
// =====================================================================

void showSummary(const PlayerManager& pm,
    const vector<string>& logicalSlotsTeam1,
    const vector<string>& logicalSlotsTeam2,
    int formationChoice1,
    int formationChoice2,
    const map<string, ReservationDetails>& allReservations,
    int totalPeople) {
    clearScreen();
    cout << "===========================================\n";
    cout << "              종합 현황 보기\n";
    cout << "===========================================\n";

    pm.showPlayers();

    cout << "\n----- 포메이션 현황 -----\n";
    vector<string> displaySlotsTeam1(NUM_POSITIONS, "");
    vector<string> displaySlotsTeam2(NUM_POSITIONS, "");
    map<LogicalPosition, int> map1 = getFormationMap(formationChoice1);
    map<LogicalPosition, int> map2 = getFormationMap(formationChoice2);
    for (int i = 0; i < NUM_POSITIONS; ++i) {
        LogicalPosition currentPos = (LogicalPosition)i;
        if (!logicalSlotsTeam1[i].empty()) displaySlotsTeam1[map1[currentPos]] = logicalSlotsTeam1[i];
        if (!logicalSlotsTeam2[i].empty()) displaySlotsTeam2[map2[currentPos]] = logicalSlotsTeam2[i];
    }
    printFormationsSideBySide(formationChoice1, displaySlotsTeam1, formationChoice2, displaySlotsTeam2);
    cout << "---------------------------\n";

    cout << "\n----- 풋살장 전체 예약 현황 -----\n";
    bool hasAnyReservation = false;
    for (const auto& pair : allReservations) {
        if (pair.second.isReserved) {
            hasAnyReservation = true;
            const ReservationDetails& details = pair.second;
            cout << "날짜     : " << details.ymd << (details.isWeekend ? " (주말)" : " (평일)") << "\n";
            cout << "예약 시간 : ";
            for (size_t i = 0; i < details.reservedSlots.size(); ++i) {
                cout << details.reservedSlots[i].first << ":00~" << details.reservedSlots[i].second << ":00";
                if (i < details.reservedSlots.size() - 1) cout << ", ";
            }
            cout << "\n";
            cout << "일일 총액: " << details.total << "원\n";
            if (totalPeople > 0) {
                cout << "더치페이 : 1인당 " << details.total / totalPeople << "원\n";
            }
            cout << "---\n";
        }
    }
    if (!hasAnyReservation) {
        cout << "아직 예약된 내역이 없습니다.\n";
    }
    cout << "---------------------------\n";
}

void reservationHubMenu(int totalPlayers, map<string, ReservationDetails>& allReservations) {
    while (true) {
        clearScreen();
        cout << "===== 풋살장 예약 관리 =====\n";
        cout << "1. 신규 예약 진행\n";
        cout << "2. 예약 취소\n";
        cout << "3. 메인 메뉴로 돌아가기\n";
        cout << "선택: ";
        int choice;
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1:
            makeNewReservation(totalPlayers, allReservations);
            break;
        case 2:
            cancelReservation(allReservations);
            break;
        case 3:
            return;
        default:
            cout << "잘못된 선택입니다.\n";
            pause();
            break;
        }
    }
}


void showWelcomeScreen() {
    clearScreen();
    cout << "****************************************\n";
    cout << "*                                      *\n";
    cout << "*     모블 FC 풋살장 관리 프로그램     *\n";
    cout << "*                                      *\n";
    cout << "****************************************\n\n";
    cout << "엔터 키를 눌러 시작하세요...";
    cin.get();
}

void showMenu() {
    clearScreen();
    cout << "===========================================\n";
    cout << "        모블 fc 풋살장 관리 시스템\n";
    cout << "===========================================\n";
    cout << "1. 참가자 등록\n";
    cout << "2. 참가자 목록 보기\n";
    cout << "3. 관리자 창 (수정/삭제)\n";
    cout << "4. 포메이션 설정\n";
    cout << "5. 풋살장 예약 관리\n";
    cout << "6. 종합 현황 보기\n";
    cout << "7. 종료\n";
    cout << "-------------------------------------------\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    showWelcomeScreen();

    int totalPeople = 0;
    while (true) {
        clearScreen();
        cout << "===========================================\n";
        cout << "        모블 fc 풋살장 관리 시스템\n";
        cout << "===========================================\n";
        cout << "오늘 경기할 총 인원을 입력해주세요 (최소 4명, 최대 14명): ";
        cin >> totalPeople;
        if (cin.good() && totalPeople >= 4 && totalPeople <= 14) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
        cout << "잘못된 입력입니다. 4에서 14 사이의 숫자를 입력해주세요.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    PlayerManager pm;
    int choice;

    vector<string> savedLogicalSlotsTeam1(NUM_POSITIONS, "");
    vector<string> savedLogicalSlotsTeam2(NUM_POSITIONS, "");
    int savedFormation1 = 1;
    int savedFormation2 = 1;

    map<string, ReservationDetails> allReservations;

    while (true) {
        showMenu();
        cout << "오늘 총원: " << totalPeople << "명 | 현재 등록: " << pm.getSize() << "명\n";
        cout << "메뉴 선택: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "잘못된 입력입니다. 숫자를 입력해주세요.\n";
            pause();
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1: {
            clearScreen();
            if (pm.getSize() >= totalPeople) {
                cout << "설정한 총 인원(" << totalPeople << "명)이 모두 등록되었습니다.\n";
                pause();
                break;
            }
            string n; int h, w, num;
            cout << "(" << pm.getSize() + 1 << "/" << totalPeople << ") 번째 선수 등록\n";
            cout << "이름 입력: "; cin >> n;
            cout << "키(cm): "; cin >> h;
            cout << "몸무게(kg): "; cin >> w;
            string b = inputBodyType();
            while (true) {
                cout << "등번호: "; cin >> num;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                int beforeSize = pm.getSize();
                pm.addPlayer(n, h, w, b, num);
                if (pm.getSize() > beforeSize) break;
            }
            pause();
            break;
        }
        case 2:
            clearScreen();
            pm.showPlayers();
            pause();
            break;
        case 3:
            adminMenu(pm);
            break;
        case 4:
            formationMenu(pm, savedLogicalSlotsTeam1, savedLogicalSlotsTeam2, savedFormation1, savedFormation2);
            break;
        case 5:
            reservationHubMenu(totalPeople, allReservations);
            break;
        case 6:
            showSummary(pm, savedLogicalSlotsTeam1, savedLogicalSlotsTeam2, savedFormation1, savedFormation2, allReservations, totalPeople);
            pause();
            break;
        case 7:
            cout << "프로그램 종료" << endl;
            return 0;
        default:
            cout << "잘못된 선택입니다.\n";
            pause();
            break;
        }
    }
    return 0;
}