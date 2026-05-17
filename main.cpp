#include <wx/wx.h>
#include <wx/listbox.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <ctime>

using namespace std;

// 1. Define the Outfit Struct
struct Outfit {
    string top;
    string bottom;
    string shoe;
    int score;
};

// 2. Define the MainFrame Class
class MainFrame : public wxFrame {
public:
    MainFrame() : wxFrame(NULL, wxID_ANY, "Outfit Randomizer", wxDefaultPosition, wxSize(500, 750)) {
        this->SetBackgroundColour(wxColour(245, 245, 245));

        wxPanel* panel = new wxPanel(this);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // --- Header ---
        wxStaticText* header = new wxStaticText(panel, wxID_ANY, "WARDROBE MANAGER", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        header->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        mainSizer->Add(header, 0, wxEXPAND | wxALL, 15);

        // --- Gender Selection ---
        wxStaticBoxSizer* genderGroup = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Step 1: Select Gender");
        genderMale = new wxRadioButton(panel, wxID_ANY, "Male", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
        genderFemale = new wxRadioButton(panel, wxID_ANY, "Female");
        genderGroup->Add(genderMale, 1, wxALL, 5);
        genderGroup->Add(genderFemale, 1, wxALL, 5);
        mainSizer->Add(genderGroup, 0, wxEXPAND | wxALL, 10);

        // --- Input Area ---
        wxStaticBoxSizer* inputGroup = new wxStaticBoxSizer(wxVERTICAL, panel, "Step 2: Add New Item (Auto-Detect Enabled)");
        itemInput = new wxTextCtrl(panel, wxID_ANY, "");
        itemInput->SetHint("e.g. Black Denim Jacket");
        inputGroup->Add(itemInput, 0, wxEXPAND | wxALL, 5);

        wxString cats[] = { "Tops", "Bottoms", "Shoes" };
        categoryChoice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, cats);
        categoryChoice->SetSelection(0);
        inputGroup->Add(new wxStaticText(panel, wxID_ANY, "Manual Fallback Category:"), 0, wxLEFT, 5);
        inputGroup->Add(categoryChoice, 0, wxEXPAND | wxALL, 5);

        wxButton* addBtn = new wxButton(panel, wxID_ANY, "Your Wardrobe");
        addBtn->SetBackgroundColour(wxColour(200, 230, 200));
        inputGroup->Add(addBtn, 0, wxALIGN_RIGHT | wxALL, 5);
        mainSizer->Add(inputGroup, 0, wxEXPAND | wxALL, 10);

        // --- Wardrobe List ---
        wxStaticText* listLabel = new wxStaticText(panel, wxID_ANY, "Current Wardrobe Inventory:");
        mainSizer->Add(listLabel, 0, wxLEFT | wxRIGHT, 10);
        debugLog = new wxListBox(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 120));
        mainSizer->Add(debugLog, 1, wxEXPAND | wxALL, 10);

        // --- Result Display ---
        wxStaticBoxSizer* resultGroup = new wxStaticBoxSizer(wxVERTICAL, panel, "Generated Outfit");
        resultDisplay = new wxStaticText(panel, wxID_ANY, "Add items and click generate!", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        resultDisplay->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        resultDisplay->SetForegroundColour(wxColour(0, 50, 150));
        resultGroup->Add(resultDisplay, 0, wxEXPAND | wxALL, 10);
        mainSizer->Add(resultGroup, 0, wxEXPAND | wxALL, 10);

        // --- Action Buttons ---
        wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
        wxButton* genBtn = new wxButton(panel, wxID_ANY, "GENERATE OUTFIT");
        wxButton* clearBtn = new wxButton(panel, wxID_ANY, "Reset All");

        genBtn->SetBackgroundColour(wxColour(100, 150, 255));
        genBtn->SetForegroundColour(*wxWHITE);

        btnSizer->Add(genBtn, 2, wxALL | wxEXPAND, 5);
        btnSizer->Add(clearBtn, 1, wxALL | wxEXPAND, 5);
        mainSizer->Add(btnSizer, 0, wxEXPAND | wxALL, 5);

        // --- Bindings ---
        addBtn->Bind(wxEVT_BUTTON, &MainFrame::OnAdd, this);
        genBtn->Bind(wxEVT_BUTTON, &MainFrame::OnGenerate, this);
        clearBtn->Bind(wxEVT_BUTTON, &MainFrame::OnClear, this);

        panel->SetSizer(mainSizer);
    }

private:
    wxRadioButton* genderMale, * genderFemale;
    wxTextCtrl* itemInput;
    wxChoice* categoryChoice;
    wxListBox* debugLog;
    wxStaticText* resultDisplay;
    vector<string> tops, bottoms, shoes;

    // --- Logic Methods ---

    string Normalize(string s) {
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        string res;
        for (char c : s) if (isalnum(c) || isspace(c)) res += c;
        return res;
    }

    // Your added Category Detection Logic
    string detectCategory(string s) {
        string lower = Normalize(s);

        if (lower.find("shoe") != string::npos || lower.find("sneaker") != string::npos ||
            lower.find("boot") != string::npos || lower.find("heels") != string::npos)
            return "shoes";

        if (lower.find("jean") != string::npos || lower.find("pant") != string::npos ||
            lower.find("short") != string::npos || lower.find("chino") != string::npos ||
            lower.find("skirt") != string::npos || lower.find("cargo") != string::npos ||
            lower.find("pantaloon") != string::npos)
            return "bottoms";

        if (lower.find("shirt") != string::npos || lower.find("tshirt") != string::npos ||
            lower.find("hoodie") != string::npos || lower.find("jacket") != string::npos ||
            lower.find("chemise") != string::npos || lower.find("tank top") != string::npos ||
            lower.find("polo") != string::npos || lower.find("crewneck") != string::npos ||
            lower.find("sweater") != string::npos || lower.find("scarf") != string::npos ||
            lower.find("top") != string::npos || lower.find("t-shirt") != string::npos ||
            lower.find("blazer") != string::npos)
            return "tops";

        return "unknown";
    }

    bool IsDuplicate(const string& newItem) {
        string n = Normalize(newItem);
        for (const auto& item : tops) if (Normalize(item) == n) return true;
        for (const auto& item : bottoms) if (Normalize(item) == n) return true;
        for (const auto& item : shoes) if (Normalize(item) == n) return true;
        return false;
    }

    bool IsGenderValid(const string& item, bool isMale) {
        string s = Normalize(item);
        vector<string> femaleOnly = { "skirt", "dress", "heels", "blouse" };
        if (isMale) {
            for (auto& keyword : femaleOnly) {
                if (s.find(keyword) != string::npos) return false;
            }
        }
        return true;
    }

    string DetectColor(string item) {
        string s = Normalize(item);
        if (s.find("black") != string::npos) return "black";
        if (s.find("white") != string::npos) return "white";
        if (s.find("blue") != string::npos) return "blue";
        if (s.find("grey") != string::npos) return "grey";
        return "neutral";
    }

    // --- Event Handlers ---

    void OnAdd(wxCommandEvent& event) {
        string name = itemInput->GetValue().ToStdString();
        if (name.empty()) return;

        if (IsDuplicate(name)) {
            wxMessageBox("This item is already in your wardrobe!", "Duplicate", wxICON_WARNING);
            return;
        }

        if (!IsGenderValid(name, genderMale->GetValue())) {
            wxMessageBox("This item doesn't match the selected gender profile.", "Validation Error", wxICON_ERROR);
            return;
        }

        // Automatic Detection Logic
        string detected = detectCategory(name);
        int finalCatIndex;

        if (detected == "tops") finalCatIndex = 0;
        else if (detected == "bottoms") finalCatIndex = 1;
        else if (detected == "shoes") finalCatIndex = 2;
        else {
            // If unknown, use the manual dropdown selection
            finalCatIndex = categoryChoice->GetSelection();
        }

        if (finalCatIndex == 0) tops.push_back(name);
        else if (finalCatIndex == 1) bottoms.push_back(name);
        else shoes.push_back(name);

        string catName = (finalCatIndex == 0) ? "Tops" : (finalCatIndex == 1 ? "Bottoms" : "Shoes");
        debugLog->AppendString("[" + catName + "] " + name);
        itemInput->Clear();
    }

    void OnGenerate(wxCommandEvent& event) {
        if (tops.empty() || bottoms.empty() || shoes.empty()) {
            wxMessageBox("Please add items to all categories first!", "Wardrobe Empty");
            return;
        }
        string t = tops[rand() % tops.size()];
        string b = bottoms[rand() % bottoms.size()];
        string s = shoes[rand() % shoes.size()];

        int score = (DetectColor(t) == DetectColor(b)) ? 9 : 6;

        wxString result;
        result.Printf("TOP: %s BOTTOM: %s SHOES: %s\nStyle Score: %d/10", t, b, s, score);
        resultDisplay->SetLabel(result);
        this->Layout();

        ofstream file("outfits_gui.txt", ios::app);
        file << "Gender: " << (genderMale->GetValue() ? "Male" : "Female") << "\n" << result.ToStdString() << "\n---\n";
        file.close();
    }

    void OnClear(wxCommandEvent& event) {
        tops.clear(); bottoms.clear(); shoes.clear();
        debugLog->Clear();
        resultDisplay->SetLabel("Wardrobe Reset.");
    }
};

// 3. App Class
class OutfitApp : public wxApp {
public:
    virtual bool OnInit() {
        srand(time(0));
        MainFrame* frame = new MainFrame();
        frame->Show(true);
        return true;
    }
};
wxIMPLEMENT_APP(OutfitApp);