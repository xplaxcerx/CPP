#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <termios.h>
#include <unistd.h>

using namespace std;

int mygetch()
{
    struct termios oldt, newt;
    int c;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
}

struct Club
{
    string name;
    string address;
    string theme;
    int openingHour;
    int openingMinute;
    int closingHour;
    int closingMinute;
    int workingTime;
};

int addClub(vector<Club> &clubs)
{
    Club newClub;

    cout << "Название: ";
    getline(cin >> ws, newClub.name);

    cout << "Адрес: ";
    getline(cin >> ws, newClub.address);

    cout << "Тематика: ";
    getline(cin >> ws, newClub.theme);

    cout << "Время открытия (часы минуты): ";
    cin >> newClub.openingHour >> newClub.openingMinute;

    cout << "Время закрытия (часы минуты): ";
    cin >> newClub.closingHour >> newClub.closingMinute;

    newClub.workingTime = (newClub.closingHour * 60 + newClub.closingMinute) - (newClub.openingHour * 60 + newClub.openingMinute);
    clubs.push_back(newClub);

    return clubs.size();
}

void printClubData(const Club &club)
{
    cout << "| " << club.name << "\t| " << club.address << "\t| " << club.theme << "\t| " << club.workingTime << " минут";
}

void printAllData(const vector<Club> &clubs)
{
    cout << "ID | Название\t\t| Адрес\t\t| Тема\t| Время работы\n";
    for (size_t i = 0; i < clubs.size(); ++i)
    {
        cout << i << " ";
        printClubData(clubs[i]);
        cout << endl;
    }
}

int removeClub(vector<Club> &clubs, int index)
{
    if (index >= 0 && index < clubs.size())
    {
        clubs.erase(clubs.begin() + index);
    }
    return clubs.size();
}

void changeClub(vector<Club> &clubs, int index)
{
    if (index >= 0 && index < clubs.size())
    {
        clubs.erase(clubs.begin() + index);
        addClub(clubs);
    }
}

int loadData(vector<Club> &clubs)
{
    ifstream file("clubs.txt");
    if (!file.is_open())
        return 0;

    int size;
    file >> size;
    file.ignore();

    for (int i = 0; i < size; i++)
    {
        Club club;
        getline(file, club.name);
        getline(file, club.address);
        getline(file, club.theme);
        file >> club.openingHour >> club.openingMinute;
        file >> club.closingHour >> club.closingMinute;

        club.workingTime = (club.closingHour * 60 + club.closingMinute) - (club.openingHour * 60 + club.openingMinute);
        file.ignore();

        clubs.push_back(club);
    }

    file.close();
    return clubs.size();
}

void save(const vector<Club> &clubs)
{
    ofstream file("clubs.txt");
    file << clubs.size() << endl;
    for (const auto &club : clubs)
    {
        file << club.name << endl;
        file << club.address << endl;
        file << club.theme << endl;
        file << club.openingHour << " " << club.openingMinute << endl;
        file << club.closingHour << " " << club.closingMinute << endl;
    }
    file.close();
}

int main()
{
    vector<Club> clubs;
    int data_size = loadData(clubs);
    int changed;
    int removed;

    while (true)
    {
        cout << "[1] - добавить клуб\n[2] - вывести все клубы\n"
             << "[3] - вывести клуб с караоке и самой большой длительностью работы\n[4] - изменить данные клуба\n"
             << "[5] - удалить клуб\n[q] - выход\n> ";
        char key = mygetch();
        cout << "\n";

        switch (key)
        {
        case '1':
            data_size = addClub(clubs);
            break;
        case '2':
            printAllData(clubs);
            break;
        case '3':
        {
            int longestIndex = -1;
            for (size_t i = 0; i < clubs.size(); ++i)
            {
                if (clubs[i].theme[0] == 'K' || clubs[i].theme[0] == 'k')
                {
                    if (longestIndex == -1 || clubs[i].workingTime > clubs[longestIndex].workingTime)
                    {
                        longestIndex = i;
                    }
                }
            }

            if (longestIndex != -1)
            {
                cout << "Клуб с караоке и самой большой длительностью работы:\n";
                printClubData(clubs[longestIndex]);
                cout << endl;
            }
            else
            {
                cout << "Нет клуба с караоке\n";
            }
        }
        break;
        case '4':
            printAllData(clubs);
            cout << "Номер изменяемого клуба: ";
            cin >> changed;
            if (changed >= 0 && changed < data_size)
                changeClub(clubs, changed);
            break;
        case '5':
            printAllData(clubs);
            cout << "Номер удаляемого клуба: ";
            cin >> removed;
            if (removed >= 0 && removed < data_size)
            {
                data_size = removeClub(clubs, removed);
            }
            break;
        case 'q':
            save(clubs);
            return 0;
        default:
            cout << "Неизвестная команда\n";
            break;
        }
    }

    return 0;
}
