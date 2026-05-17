#include <iostream>
#include <string>
#define NOMINMAX
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <cctype>
#include <algorithm>
#include <vector>
#include <fstream>
using namespace std;

// ---------- COLOR ----------
void setColor(int text, int bg = 0) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text + (bg * 16));
}
// ---------- WHITE BACKGROUND ----------
void setWhiteBackground() {
    system("color F0");
}
// ---------- CLEAR INPUT ----------
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
// ---------- VALID TEXT ----------
bool isValidText(const string& input) {
    if (input.empty() || input.length() > 50) return false;
    for (char c : input)
        if (isdigit((unsigned char)c)) return false;
    return true;
}
// ---------- INPUT ----------
string getValidText(string message) {
    string input;
    while (true) {
        setColor(1, 15);
        cout << message;
        getline(cin, input);
        if (!isValidText(input)) {
            setColor(4, 15);
            cout << "Invalid text! Try again...\n";
            continue;
        }
        return input;
    }
}
// ---------- NUMBER ----------
int getValidNumber(string message, int min, int max) {
    int value;
    while (true) {
        setColor(1, 15);
        cout << message;
        cin >> value;
        if (cin.fail()) {
            setColor(4, 15);
            cout << "Numbers only!\n";
            clearInput();
        }
        else if (value < min || value > max) {
            setColor(4, 15);
            cout << "Enter between " << min << " and " << max << ".\n";
            clearInput();
        }
        else {
            clearInput();
            return value;
        }
    }
}
// ---------- NORMALIZATION ----------
string normalizeAdvanced(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    string result;
    for (char c : s)
        if (!isspace(c) && c != '-') result += c;
    return result;
}
// ---------- REMOVE COLOR ONLY ----------
string removeColorOnly(string s) {
    string colors[] = { "black","white","blue","navy","grey","beige","brown" };
    s = normalizeAdvanced(s);
    for (string c : colors) {
        size_t pos;
        while ((pos = s.find(c)) != string::npos)
            s.erase(pos, c.length());
    }
    return s;
}
// ---------- SIMILARITY ----------
bool isSimilar(string a, string b) {
    a = normalizeAdvanced(a);
    b = normalizeAdvanced(b);
    if (a == b) return true;
    int match = 0;
    for (char c : a)
        if (b.find(c) != string::npos) match++;
    double sim = (double)match / max(a.length(), b.length());
    return sim >= 0.7;
}
// ---------- DUPLICATE CHECK ----------
bool isDuplicateGlobal(string input,
    string tops[], int topSize,
    string bottoms[], int bottomSize,
    string shoes[], int shoesSize)
{
    string cleanInput = removeColorOnly(input);
    for (int i = 0; i < topSize; i++)
        if (removeColorOnly(tops[i]) == cleanInput) return true;
    for (int i = 0; i < bottomSize; i++)
        if (removeColorOnly(bottoms[i]) == cleanInput) return true;
    for (int i = 0; i < shoesSize; i++)
        if (removeColorOnly(shoes[i]) == cleanInput) return true;
    return false;
}
// CATEGORY VALIDATION
// detect if item belongs to another category
string detectCategory(string s) {
    s = normalizeAdvanced(s);
    if (s.find("shoe") != string::npos || s.find("sneaker") != string::npos || s.find("boot") != string::npos
        || s.find("heels") != string::npos)
        return "shoes";

    if (s.find("jean") != string::npos || s.find("pant") != string::npos || s.find("short") != string::npos ||
        s.find("chino") != string::npos || s.find("skirt") != string::npos || s.find("chemise") != string::npos
        || s.find("cargo") != string::npos || s.find("pantaloon") != string::npos)
        return "bottoms";

    if (s.find("shirt") != string::npos || s.find("tshirt") != string::npos || s.find("hoodie") != string::npos
        || s.find("jacket") != string::npos || s.find("chemise") != string::npos || s.find("tank top") != string::npos
        || s.find("polo") != string::npos || s.find("crewneck") != string::npos || s.find("sweater") != string::npos
        || s.find("scarf") != string::npos || s.find("top") != string::npos || s.find("t-shirt") != string::npos
        || s.find("blazer") != string::npos)
        return "tops";
    return "unknown";
}
// validate input belongs to correct category
bool isWrongCategory(string input, string expected) {
    string cat = detectCategory(input);
    if (cat == "unknown") return false; // allow unknown items
    return cat != expected;
}
// ---------- INPUT WITH CATEGORY + DUPLICATE LOOP ----------
string getUniqueItem(string message,
    string tops[], int topSize,
    string bottoms[], int bottomSize,
    string shoes[], int shoeSize,
    string expectedCategory)
{
    string input;
    while (true) {
        input = getValidText(message);
        // CATEGORY CHECK 
        if (isWrongCategory(input, expectedCategory)) {
            setColor(4, 15);
            cout << "Invalid item for this category! Try again...\n";
            continue;
        }
        // DUPLICATE CHECK
        if (isDuplicateGlobal(input, tops, topSize, bottoms, bottomSize, shoes, shoeSize)) {
            setColor(4, 15);
            cout << "This item is repeated, put other item.\n";
            continue;
        }
        return input;
    }
}
// ---------- COLOR ----------
string detectColor(string item) {
    string s = normalizeAdvanced(item);
    if (s.find("black") != string::npos) return "black";
    if (s.find("white") != string::npos) return "white";
    if (s.find("blue") != string::npos) return "blue";
    if (s.find("navy") != string::npos) return "navy";
    if (s.find("grey") != string::npos) return "grey";
    if (s.find("beige") != string::npos) return "beige";
    if (s.find("brown") != string::npos) return "brown";
    if (s.find("pink") != string::npos) return "pink";
    if (s.find("purple") != string::npos) return "purple";
    if (s.find("green") != string::npos) return "green";
    return "unknown";
}
// ---------- SCORE ----------
int calculateScore(string c1, string c2) {
    int score = 0;
    if (c1 == c2) score += 3;
    if (c1 == "black" || c1 == "white" || c1 == "grey" || c1 == "beige")
        score += 2;
    if (c2 == "black" || c2 == "white" || c2 == "grey" || c2 == "beige")
        score += 2;
    if (c1 == "blue" && c2 == "white") score += 3;
    if (c1 == "navy" && c2 == "beige") score += 3;
    return score;
}
string getMatchQuality(int score) {
    if (score >= 8) return "Excellent";
    if (score >= 5) return "Good";
    if (score >= 3) return "Average";
    return "Weak";
}
// ---------- OUTFIT ----------
struct Outfit {
    string top;
    string bottom;
    string shoe;
    int score;
};
vector<Outfit> pool;
vector<string> usedOutfits;
string makeKey(string a, string b, string c) {
    return a + "|" + b + "|" + c;
}
void loadUsedOutfits() {
    ifstream file("outfits.txt");
    string line;
    while (getline(file, line)) {
        if (!line.empty())
            usedOutfits.push_back(line);
    }
    file.close();
}
void showSavedOutfits() {
    ifstream file("outfits.txt");
    if (!file.is_open()) {
        setColor(4, 15);
        cout << "No saved outfits found!\n";
        return;
    }
    string line;
    int count = 1;
    setColor(0, 15);
    cout << "\n========== SAVED OUTFITS ==========\n";
    while (getline(file, line)) {
        if (!line.empty()) {
            cout << count++ << ") " << line << "\n";
        }
    }
    file.close();
}
void deleteSavedOutfit() {
    ifstream file("outfits.txt");
    vector<string> lines;
    string line;
    // Read The INFO 
    while (getline(file, line)) {
        if (!line.empty())
            lines.push_back(line);
    }
    file.close();
    if (lines.empty()) {
        setColor(4, 15);
        cout << "No outfits to delete!\n";
        return;
    }
    // Show INFO
    setColor(0, 15);
    cout << "\n========== SAVED OUTFITS ==========\n";
    for (int i = 0; i < lines.size(); i++) {
        cout << i + 1 << ") " << lines[i] << "\n";
    }
    cout << "\nEnter numbers to delete: ";
    vector<int> toDelete;
    int x;
    string input;
    getline(cin, input);
    string num = "";
    for (char c : input) {
        if (isdigit(c)) {
            num += c;
        }
        else {
            if (!num.empty()) {
                int x = stoi(num);
                if (x >= 1 && x <= lines.size()) {
                    toDelete.push_back(x - 1);
                }
                num = "";
            }
        }
    }
    // If last number didn't close
    if (!num.empty()) {
        int x = stoi(num);
        if (x >= 1 && x <= lines.size()) {
            toDelete.push_back(x - 1);
        }
    }
    if (toDelete.empty()) {
        setColor(4, 15);
        cout << "Nothing deleted.\n";
        return;
    }
    // Arranged
    sort(toDelete.begin(), toDelete.end(), greater<int>());
    for (int idx : toDelete) {
        if (idx >= 0 && idx < lines.size())
            lines.erase(lines.begin() + idx);
    }
    // Rewrite
    ofstream out("outfits.txt");
    for (string& l : lines) {
        out << l << endl;
    }
    out.close();
    setColor(2, 15);
    cout << "Selected outfits deleted successfully!\n";
}
void saveUsedOutfits() {
    ofstream file("outfits.saved files");
    for (auto& x : usedOutfits)
        file << x << endl;
    file.close();
}
bool isUsed(string k) {
    for (auto& x : usedOutfits)
        if (x == k) return true;
    return false;
}
// ---------- BEST GENERATOR ----------
Outfit generateBest(string tops[], int topC,
    string bottoms[], int botC,
    string shoes[], int shoeC) {
    pool.clear();
    for (int i = 0; i < topC; i++) {
        for (int j = 0; j < botC; j++) {
            for (int k = 0; k < shoeC; k++) {
                string tc = detectColor(tops[i]);
                string bc = detectColor(bottoms[j]);
                int score = calculateScore(tc, bc);
                pool.push_back({ tops[i], bottoms[j], shoes[k], score });
            }
        }
    }
    sort(pool.begin(), pool.end(),
        [](Outfit a, Outfit b) { return a.score > b.score; });
    int limit = min((int)pool.size(), 5);
    return pool[rand() % limit];
}
// ---------- USER ----------
struct UserProfile {
    int gender;
    int occasion;
    int season;
};
UserProfile getUserProfile() {
    UserProfile u;
    setColor(0, 15);
    cout << "\n========== USER INFO ==========\n";
    u.gender = getValidNumber("1) Male\n2) Female\nChoose Gender: ", 1, 2);
    u.occasion = getValidNumber("\n1) Casual\n2) Formal\n3) Sport\nChoose Occasion: ", 1, 3);
    u.season = getValidNumber("\n1) Spring\n2) Summer\n3) Autumn\n4) Winter\nChoose Season: ", 1, 4);
    return u;
}
// ---------- MENU ----------
void showMenu() {
    setColor(0, 15);
    cout << "===============================\n";
    cout << "   OUTFIT RANDOMIZER SYSTEM    \n";
    cout << "===============================\n";
    cout << "[1] Generate Outfit\n";
    cout << "[2] Show Saved Outfits\n";
    cout << "[3] Delete Outfit\n";
    cout << "[4] Exit\n\n";
}
// ---------- MAIN ----------
void generateOutfit() {
    UserProfile user = getUserProfile();
    string tops[200], bottoms[200], shoes[200];
    int topCount = getValidNumber("\nHow many TOPS? ", 1, 200);
    for (int i = 0; i < topCount; i++)
        tops[i] = getUniqueItem(to_string(i + 1) + ") ",
            tops, i, bottoms, 0, shoes, 0, "tops");
    int bottomCount = getValidNumber("\nHow many BOTTOMS? ", 1, 200);
    for (int i = 0; i < bottomCount; i++)
        bottoms[i] = getUniqueItem(to_string(i + 1) + ") ",
            tops, topCount, bottoms, i, shoes, 0, "bottoms");
    int shoeCount = getValidNumber("\nHow many SHOES? ", 1, 200);
    for (int i = 0; i < shoeCount; i++)
        shoes[i] = getUniqueItem(to_string(i + 1) + ") ",
            tops, topCount, bottoms, bottomCount, shoes, i, "shoes");
    Outfit best = generateBest(tops, topCount, bottoms, bottomCount, shoes, shoeCount);
    string key = makeKey(best.top, best.bottom, best.shoe);
    if (!isUsed(key))
        usedOutfits.push_back(key);
    saveUsedOutfits();
    setColor(0, 15);
    cout << "\n========== YOUR OUTFIT ==========\n";
    cout << "Top: " << best.top << "\n";
    cout << "Bottom: " << best.bottom << "\n";
    cout << "Shoes: " << best.shoe << "\n";
    string quality = getMatchQuality(best.score);
    setColor(2, 15);
    cout << "Match Quality: " << quality << "\n";
    ofstream file("outfits.txt", ios::app);
    if (file.is_open()) {
        file << "Top: " << best.top
            << " | Bottom: " << best.bottom
            << " | Shoes: " << best.shoe
            << " | Score: " << best.score << "/10\n"; \
            file.close();
    }
}
// ---------- MAIN ----------
int main() {
    setWhiteBackground();
    srand(time(0));
    loadUsedOutfits();
    int choice;
    do {
        system("cls");
        showMenu();
        choice = getValidNumber("Choose Option: ", 1, 4);
        if (choice == 1) {
            system("cls");
            generateOutfit();
            system("pause");
        }
        else if (choice == 2) {
            system("cls");
            showSavedOutfits();
            system("pause");
        }
        else if (choice == 3) {
            system("cls");
            deleteSavedOutfit();
            system("pause");
        }
    } while (choice != 4);
}