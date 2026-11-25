#include "atm.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

std::ofstream fout;

void print(const std::string &text)
{
    std::cout << text << std::endl;
    fout << text << std::endl;
}

std::string status(const ATM &atm)
{
    std::ostringstream s;
    s << "    стан: " << atm.GetStateName()
      << " | гроші: " << atm.GetMoney() << " грн"
      << " | стрічка: " << std::fixed << std::setprecision(2) << atm.GetTapeLength() << " м"
      << " | немає стрічки: " << (atm.GetNoTape() ? "так" : "ні")
      << " | права: " << atm.GetLevelName();
    return s.str();
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::ifstream fin("input.txt");
    fout.open("output.txt");

    if (!fin) {
        print("Не вдалося відкрити input.txt");
        return 1;
    }

    ATM atm;
    atm.Reset();

    print("Банкомат після Reset():");
    print(status(atm));

    std::string command;
    int step = 1;
    while (fin >> command) {
        bool ok = false;
        std::ostringstream line;
        line << step++ << ". ";

        if (command == "login") {
            std::string password;
            fin >> password;
            atm += password;
            ok = (atm == true);
            line << "atm += \"" << password << "\"  (авторизація), atm == true -> "
                 << (ok ? "адміністратор" : "гість");
        } else if (command == "logout") {
            atm.Logout();
            ok = true;
            line << "Вихід з системи";
        } else if (command == "money") {
            int sum;
            fin >> sum;
            atm = sum;
            ok = atm.GetMoney() == sum;
            line << "atm = " << sum << "  (зміна обсягу грошей)";
        } else if (command == "tape") {
            double meters;
            fin >> meters;
            ok = atm.SetTapeLength(meters);
            line << "Зміна довжини стрічки на " << meters << " м";
        } else if (command == "withdraw") {
            int sum;
            fin >> sum;
            int asked = sum;
            atm >> sum;
            ok = sum > 0;
            line << "atm >> " << asked << "  (зняття), видано: " << sum << " грн";
        } else {
            line << "Невідома команда: " << command;
        }

        line << " -> " << (ok ? "виконано" : "відмова");
        print(line.str());
        print(status(atm));
    }

    print("Результати записано у файл output.txt");
    return 0;
}
