#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>

using namespace std;
namespace fs = filesystem;

int main()
{
    string input;
    fs::path currentPath = fs::current_path();

    while (true) {
        cout << "We are inside the: " << currentPath.string() << "\n\n";

        vector <fs::directory_entry> entries;
        for (const auto& entry: fs::directory_iterator(currentPath))
            entries.push_back(entry);

        sort(entries.begin(), entries.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
            if (a.is_directory() && !b.is_directory()) return true; // if a - a file, b - a folder
            if (!a.is_directory() && b.is_directory()) return false; // if a - a folder, b - a file
            return a.path().filename().string() < b.path().filename().string(); //sorting by alphabet
        });

        int index = 1;
        for (const auto& entry : entries) {
            string type = entry.is_directory() ? "[FOLDER]" : "[FILE]";
            cout << index++ << ". " << type << " " << entry.path().filename().string() << '\n';
        }

        cout << "Enter a number, '..' to go back, 'q' to quit: ";
        getline(cin, input);

        if (input == "q") break;
        if (input == "..") {
            currentPath = currentPath.parent_path(); // go back
            continue; // go to the next iteraton
        }

        try {
            int choice = stoi(input); // "2" -> 2

            if (choice >= 1 && choice <= entries.size()) {
                fs::path target = entries[choice - 1].path(); // -1, because of massive begins from 0
                if (fs::is_directory(target)) currentPath = target;
                else cout << "That's a file. I can't open it in a console mode.\n";
            }
            else cout << "There's no that number!\n";
        }
        catch (...) {
            cout << "You need to type '..' or 'q'!\n";
        }
    }

    return 0;
}
