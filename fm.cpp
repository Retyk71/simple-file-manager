#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <fstream>

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

        cout << "Enter a number, 'n' to create, 'r' to read, 'c' to copy, '..' to go back, 'q' to quit: ";
        getline(cin, input);

        if (input == "q") break;
        if (input == "..") {
            currentPath = currentPath.parent_path(); // go back
            continue; // go to the next iteraton
        }
        if (input == "r") {
           cout << "\nWhich file do you want to read? Enter the number: ";
            string file_num_str;
            getline(cin, file_num_str);

            try {
                int file_num = stoi(file_num_str); // "31" -> 31 for example

                if (file_num >= 1 && file_num <= entries.size()) {
                    fs::path targetFile = entries[file_num - 1].path(); // takes the file from this folder

                    if (fs::is_directory(targetFile)) cout << "This is a folder, not a file!\n";
                    else {
                        ifstream file(targetFile);

                        if (!file.is_open()) cout << "Error: cannot open the file!\n";
                        else {
                            cout << "\n== Content of " << targetFile.filename().string() << " ===\n\n";
                            string line;
                            while (getline(file, line)) cout << line << '\n';
                            file.close();
                            cout << "\n=== End of file ===\n";
                        }
                    }
                }
                else cout << "Invalid number!\n";
            }
            catch (...) {
                cout << "You need to enter a number!\n";
            }
            cout << "\nPress Enter to continue...";
            string dummy;
            getline(cin, dummy);
            continue;
        }

        if (input == "c") {
            cout << "\nWhich file do you want to copy? Enter the number: ";
            string file_num_str;
            getline(cin, file_num_str);

            try {
                int file_num = stoi(file_num_str);

                if (file_num >= 1 && file_num <= entries.size()) {
                    fs::path sourceFile = entries[file_num - 1].path();

                    if (fs::is_directory(sourceFile))
                        cout << "Cannot cope a folders yet. Select a file.\n";
                    else {
                        cout << "Enter destination path (e.g., 'copy.txt' or 'C:/folder/copy.txt'): ";
                        string dest_str;
                        getline(cin, dest_str);

                        if (dest_str.empty()) cout << "Destination path cannot be empty!\n";
                        else {
                            fs::path dest_path(dest_str);

                            if (dest_path.is_relative() && dest_path.parent_path().empty())
                                dest_path = currentPath / dest_str;

                            try {
                                fs::copy(sourceFile, dest_path, fs::copy_options::overwrite_existing);
                                cout << "File copied succesfully to: " << dest_path.string() << '\n';
                            }
                            catch(const fs::filesystem_error& e) {
                                cout << "Error copying file: " << e.what() << '\n';
                            }
                        }
                    }
                }
                else cout << "Invalid number!\n";
            }
            catch (...) {
                cout << "You need to enter a number!\n";
            }
            cout << "\nPress Enter to continue...";
            string dummy;
            getline(cin, dummy);
            continue;
        }

        if (input == "n") {
            cout << "\nEnter the name for the new file (e.g., 'file.txt'): ";
            string filename;
            getline(cin, filename);

            if (filename.empty()) cout << "Filename cannot be empty!\n";
            else {
                fs::path new_file_path = currentPath / filename;
                bool cancelled{false};

                if (fs::exists(new_file_path)) {
                    string answer;
                    bool valid_answer{false};

                    while (!valid_answer) {
                        cout << "File already exists! Overwrite? (y/n): ";
                        getline(cin, answer);

                        if (answer == "y" || answer == "Y") valid_answer = true;
                        else if (answer == "n" || answer == "N") {
                            valid_answer = true;
                            cancelled = true;
                            cout << "Cancelled.\n";
                        }
                        else cout << "Incorrect answer! Please enter 'y' or 'n'.\n";
                    }
                }

                if (!cancelled) {
                    try {
                        ofstream file(new_file_path);
                        if (!file.is_open()) cout << "Error: cannot create the file!\n";
                        else {
                            cout << "Enter the file content (press Enter on an empty line to finish):\n";
                            string line;
                            while (true) {
                                getline(cin, line);
                                if (line.empty()) break;
                                file << line << '\n';
                            }
                            file.close();
                            cout << "\nFile created successfully: " << new_file_path.string() << '\n';
                        }
                    }
                    catch (const exception& e) {
                        cout << "Error creating file: " << e.what() << '\n';
                    }
                }
            }
            cout << "\nPress Enter to continue...";
            string dummy;
            getline(cin, dummy);
            continue;
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
