// wardrobe.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <random>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

string generateId() {
    const char* hex = "0123456789abcdef";
    string id;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 15);
    for (int i=0; i<8; i++) id += hex[dis(gen)];
    return id;
}

struct ClothingItem {
    string id;
    string name;
    string category;
    string photo;
    string tags;
    string date;
    bool favorite;
    string created_at;
};

class Wardrobe {
private:
    vector<ClothingItem> items;
    string dataFile = "wardrobe.json";

    string currentDate() {
        time_t t = time(nullptr);
        char buf[11];
        strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));
        return string(buf);
    }

    string currentTime() {
        time_t t = time(nullptr);
        char buf[30];
        strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S%z", localtime(&t));
        return string(buf);
    }

    void load() {
        ifstream f(dataFile);
        if (!f.is_open()) return;
        json j;
        f >> j;
        for (auto& item : j) {
            ClothingItem ci;
            ci.id = item["id"];
            ci.name = item["name"];
            ci.category = item["category"];
            ci.photo = item["photo"];
            ci.tags = item["tags"];
            ci.date = item["date"];
            ci.favorite = item["favorite"];
            ci.created_at = item["created_at"];
            items.push_back(ci);
        }
    }

    void save() {
        json j = json::array();
        for (auto& i : items) {
            j.push_back({
                {"id", i.id},
                {"name", i.name},
                {"category", i.category},
                {"photo", i.photo},
                {"tags", i.tags},
                {"date", i.date},
                {"favorite", i.favorite},
                {"created_at", i.created_at}
            });
        }
        ofstream f(dataFile);
        f << setw(2) << j << endl;
    }

public:
    Wardrobe() { load(); }

    void add(const string& name, const string& category, const string& photo,
             const string& tags, const string& date) {
        ClothingItem item;
        item.id = generateId();
        item.name = name;
        item.category = category;
        item.photo = photo;
        item.tags = tags;
        item.date = date.empty() ? currentDate() : date;
        item.favorite = false;
        item.created_at = currentTime();
        items.push_back(item);
        save();
        cout << "✅ Added: " << name << " (ID: " << item.id << ")\n";
    }

    void list(const string& category) {
        vector<ClothingItem> filtered = items;
        if (!category.empty()) {
            filtered.clear();
            for (auto& i : items) {
                string c1 = i.category, c2 = category;
                transform(c1.begin(), c1.end(), c1.begin(), ::tolower);
                transform(c2.begin(), c2.end(), c2.begin(), ::tolower);
                if (c1 == c2) filtered.push_back(i);
            }
        }
        if (filtered.empty()) {
            cout << "No items.\n";
            return;
        }
        cout << "\n📋 Wardrobe (" << filtered.size() << " items):\n";
        for (size_t i=0; i<filtered.size(); i++) {
            auto& item = filtered[i];
            string fav = item.favorite ? " ⭐" : "";
            string tags = item.tags.empty() ? "" : " | " + item.tags;
            cout << "  " << i+1 << ". " << item.name << " [" << item.category << "]" << fav << tags << "\n";
        }
    }

    void search(const string& term) {
        string t = term;
        transform(t.begin(), t.end(), t.begin(), ::tolower);
        vector<ClothingItem> results;
        for (auto& i : items) {
            string n = i.name, c = i.category, tg = i.tags;
            transform(n.begin(), n.end(), n.begin(), ::tolower);
            transform(c.begin(), c.end(), c.begin(), ::tolower);
            transform(tg.begin(), tg.end(), tg.begin(), ::tolower);
            if (n.find(t) != string::npos || c.find(t) != string::npos || tg.find(t) != string::npos) {
                results.push_back(i);
            }
        }
        if (results.empty()) {
            cout << "No matches.\n";
            return;
        }
        cout << "\n🔍 Found " << results.size() << " item(s):\n";
        for (size_t i=0; i<results.size(); i++) {
            auto& item = results[i];
            string fav = item.favorite ? " ⭐" : "";
            string tags = item.tags.empty() ? "" : " | " + item.tags;
            cout << "  " << i+1 << ". " << item.name << " [" << item.category << "]" << fav << tags << "\n";
        }
    }

    void show(const string& id) {
        for (auto& item : items) {
            if (item.id == id) {
                cout << "\n👔 " << item.name << "\n";
                cout << "  ID: " << item.id << "\n";
                cout << "  Category: " << (item.category.empty() ? "Uncategorized" : item.category) << "\n";
                cout << "  Photo: " << (item.photo.empty() ? "No photo" : item.photo) << "\n";
                cout << "  Tags: " << (item.tags.empty() ? "None" : item.tags) << "\n";
                cout << "  Date: " << item.date << "\n";
                cout << "  Favorite: " << (item.favorite ? "Yes" : "No") << "\n";
                cout << "  Added: " << item.created_at << "\n";
                return;
            }
        }
        cout << "Item " << id << " not found.\n";
    }

    void favorite(const string& id) {
        for (auto& item : items) {
            if (item.id == id) {
                item.favorite = !item.favorite;
                save();
                string status = item.favorite ? "⭐ added to" : "❌ removed from";
                cout << "✅ " << status << " favorites: " << item.name << "\n";
                return;
            }
        }
        cout << "Item " << id << " not found.\n";
    }

    void stats() {
        if (items.empty()) {
            cout << "No items.\n";
            return;
        }
        map<string, int> categories;
        for (auto& i : items) {
            string cat = i.category.empty() ? "Uncategorized" : i.category;
            categories[cat]++;
        }
        cout << "\n📊 Statistics:\n";
        for (auto& kv : categories) {
            cout << "  " << kv.first << ": " << kv.second << " item(s)\n";
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: wardrobe <command> [options]\n";
        return 1;
    }
    Wardrobe w;
    string cmd = argv[1];

    if (cmd == "add") {
        if (argc < 3) { cerr << "add <name> [--category CAT] [--photo PATH] [--tags TAGS] [--date DATE]\n"; return 1; }
        string name = argv[2];
        string category, photo, tags, date;
        for (int i=3; i<argc; i++) {
            if (string(argv[i]) == "--category" && i+1 < argc) category = argv[++i];
            if (string(argv[i]) == "--photo" && i+1 < argc) photo = argv[++i];
            if (string(argv[i]) == "--tags" && i+1 < argc) tags = argv[++i];
            if (string(argv[i]) == "--date" && i+1 < argc) date = argv[++i];
        }
        w.add(name, category, photo, tags, date);
    } else if (cmd == "list") {
        string category;
        for (int i=2; i<argc; i++) {
            if (string(argv[i]) == "--category" && i+1 < argc) category = argv[++i];
        }
        w.list(category);
    } else if (cmd == "search") {
        if (argc < 3) { cerr << "search <term>\n"; return 1; }
        w.search(argv[2]);
    } else if (cmd == "show") {
        if (argc < 3) { cerr << "show <id>\n"; return 1; }
        w.show(argv[2]);
    } else if (cmd == "favorite") {
        if (argc < 3) { cerr << "favorite <id>\n"; return 1; }
        w.favorite(argv[2]);
    } else if (cmd == "stats") {
        w.stats();
    } else {
        cerr << "Unknown command. Use add, list, search, show, favorite, stats.\n";
        return 1;
    }
    return 0;
}
