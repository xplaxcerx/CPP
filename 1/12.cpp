#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <curses.h>
#include <iomanip>

class Club {
private:
    std::string name;
    std::string address;
    std::string theme;
    int openingHour;
    int openingMinute;
    int closingHour;
    int closingMinute;
    int workingTime;

public:

    Club() : openingHour(0), openingMinute(0), closingHour(0), closingMinute(0), workingTime(0) {}

    ~Club() {}
    std::string getName() const { return name; }
    std::string getAddress() const { return address; }
    std::string getTheme() const { return theme; }
    int getOpeningHour() const { return openingHour; }
    int getOpeningMinute() const { return openingMinute; }
    int getClosingHour() const { return closingHour; }
    int getClosingMinute() const { return closingMinute; }
    int getWorkingTime() const { return workingTime; }

    void inputData() {
        std::cin.ignore();
        
        std::cout << "Name: ";
        std::getline(std::cin, name);

        std::cout << "Address: ";
        std::getline(std::cin, address);

        std::cout << "Theme: ";
        std::getline(std::cin, theme);

        do {
            std::cout << "Opening time (hours minutes): ";
            std::cin >> openingHour >> openingMinute;
            if (!validateTime(openingHour, openingMinute)) {
                std::cout << "Error: Invalid time. Enter hours (0-23) and minutes (0-59)\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }
        } while (!validateTime(openingHour, openingMinute));

        do {
            std::cout << "Closing time (hours minutes): ";
            std::cin >> closingHour >> closingMinute;
            if (!validateTime(closingHour, closingMinute)) {
                std::cout << "Error: Invalid time. Enter hours (0-23) and minutes (0-59)\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }
        } while (!validateTime(closingHour, closingMinute));

        calculateWorkingTime();
    }

    bool hasKaraoke() const {
        std::string lowerTheme = theme;
        std::transform(lowerTheme.begin(), lowerTheme.end(), lowerTheme.begin(), ::tolower);
        return lowerTheme.find("karaoke") != std::string::npos;
    }

    void printData(int id) const {
        std::cout << std::setw(2) << id << " | "
                  << std::setw(20) << std::left << name << "| "
                  << std::setw(20) << address << "| "
                  << std::setw(20) << theme << "| "
                  << std::setfill('0') << std::setw(2) << openingHour << ":"
                  << std::setw(2) << openingMinute << "-"
                  << std::setw(2) << closingHour << ":"
                  << std::setw(2) << closingMinute
                  << std::setfill(' ') << " (" << workingTime << " min)\n";
    }

    void saveToFile(std::ofstream& file) const {
        file << name << '\n'
             << address << '\n'
             << theme << '\n'
             << openingHour << ' ' << openingMinute << '\n'
             << closingHour << ' ' << closingMinute << '\n';
    }

    bool loadFromFile(std::ifstream& file) {
        if (std::getline(file, name) &&
            std::getline(file, address) &&
            std::getline(file, theme) &&
            file >> openingHour >> openingMinute &&
            file >> closingHour >> closingMinute) {
            file.ignore(); // Skip newline
            calculateWorkingTime();
            return true;
        }
        return false;
    }

private:
    bool validateTime(int hour, int minute) const {
        return (hour >= 0 && hour < 24 && minute >= 0 && minute < 60);
    }

    void calculateWorkingTime() {
        workingTime = closingHour * 60 + closingMinute - openingHour * 60 - openingMinute;
        if (workingTime < 0) {
            workingTime += 24 * 60;
        }
    }
};

class ClubManager {
private:
    Club* clubs;  
    int size;     
    int capacity; 
    const std::string filename = "clubs.txt";

public:
   
    ClubManager() : clubs(nullptr), size(0), capacity(100) {
        clubs = new Club[capacity]; 
        loadFromFile();
    }

    ~ClubManager() {
        if (clubs != nullptr) {
            delete[] clubs;  
        }
    }

    void addClub() {
        if (size >= capacity) {
            std::cout << "Error: Maximum number of clubs reached\n";
            return;
        }

        clubs[size].inputData();
        size++;
        saveToFile();
        std::cout << "Club added successfully\n";
    }
    void printAllClubs() const {
        if (size == 0) {
            std::cout << "Club list is empty\n";
            return;
        }

        std::cout << "ID | Name                | Address             | Theme               | Working Hours\n";
        std::cout << "---+--------------------+--------------------+--------------------+------------------\n";
        for (int i = 0; i < size; ++i) {
            clubs[i].printData(i);
        }
    }

    void findKaraokeClub() const {
        if (size == 0) {
            std::cout << "Club list is empty\n";
            return;
        }

        int maxTime = -1;
        int foundIndex = -1;

        for (int i = 0; i < size; ++i) {
            if (clubs[i].hasKaraoke() && clubs[i].getWorkingTime() > maxTime) {
                maxTime = clubs[i].getWorkingTime();
                foundIndex = i;
            }
        }

        if (foundIndex != -1) {
            std::cout << "Found club with karaoke and longest working hours:\n";
            std::cout << "ID | Name                | Address             | Theme               | Working Hours\n";
            std::cout << "---+--------------------+--------------------+--------------------+------------------\n";
            clubs[foundIndex].printData(foundIndex);
        } else {
            std::cout << "No clubs with karaoke found\n";
        }
    }

    void editClub() {
        if (size == 0) {
            std::cout << "Club list is empty\n";
            return;
        }

        printAllClubs();
        std::cout << "Enter club number to edit: ";
        int index;
        std::cin >> index;

        if (index >= 0 && index < size) {
            clubs[index].inputData();
            saveToFile();
            std::cout << "Club edited successfully\n";
        } else {
            std::cout << "Error: Invalid club number\n";
        }
    }

    void removeClub() {
        if (size == 0) {
            std::cout << "Club list is empty\n";
            return;
        }

        printAllClubs();
        std::cout << "Enter club number to delete: ";
        int index;
        std::cin >> index;

        if (index >= 0 && index < size) {
            for (int i = index; i < size - 1; ++i) {
                clubs[i] = clubs[i + 1];
            }
            size--;
            saveToFile();
            std::cout << "Club successfully deleted\n";
        } else {
            std::cout << "Error: Invalid club number\n";
        }
    }

private:
    void saveToFile() {
        std::ofstream file(filename);
        if (!file) {
            std::cout << "Error: Could not open file for saving\n";
            return;
        }

        file << size << '\n';
        for (int i = 0; i < size; ++i) {
            clubs[i].saveToFile(file);
        }
        std::cout << "Data successfully saved to " << filename << "\n";
    }
    void loadFromFile() {
        std::ifstream file(filename);
        if (!file) {
            std::ofstream newFile(filename);
            if (newFile) {
                newFile << "0\n";
                std::cout << "Created new " << filename << " file\n";
            } else {
                std::cout << "Error: Could not create " << filename << "\n";
            }
            return;
        }

        int fileSize;
        file >> fileSize;
        file.ignore(); 

        size = 0;
        for (int i = 0; i < fileSize && i < capacity; ++i) {
            if (clubs[i].loadFromFile(file)) {
                size++;
            } else {
                std::cout << "Error: Could not read club data\n";
                break;
            }
        }
        std::cout << "Successfully loaded " << size << " clubs from " << filename << "\n";
    }
};

int main() {
    ClubManager manager;

    while (true) {
        std::cout << "\nClub Management System:\n"
                  << "[1] - Add new club\n"
                  << "[2] - Show all clubs\n"
                  << "[3] - Find club with karaoke and longest working hours\n"
                  << "[4] - Edit club data\n"
                  << "[5] - Delete club\n"
                  << "[q] - Exit\n>";

        char choice = _getch();
        std::cout << '\n';

        switch (choice) {
            case '1':
                manager.addClub();
                break;
            case '2':
                manager.printAllClubs();
                break;
            case '3':
                manager.findKaraokeClub();
                break;
            case '4':
                manager.editClub();
                break;
            case '5':
                manager.removeClub();
                break;
            case 'q':
                std::cout << "Goodbye!\n";
                return 0;
            default:
                std::cout << "Unknown command\n";
        }
    }
}
 