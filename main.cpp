#include "atm.h"
#include "ui.h"

#ifdef _WIN32
#include <windows.h>
#endif

std::vector<std::string> Row(const std::string &command, const std::string &result, const ATM &atm)
{
    return {command, result, atm.GetStateName(), std::to_string(atm.GetMoney()), Num(atm.GetTapeLength()),
            atm.GetNoTape() ? "YES" : "NO", atm.GetLevelName()};
}

const std::vector<std::string> HEAD = {"Operator", "Result", "State", "Money", "Tape", "No tape", "User"};

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    SaveOutputTo("output.txt");

    PrintBox("<------- ATM: OPERATORS OVERLOADING ------->");

    ATM demo;
    demo.Reset();

    PrintBox("Testing operator += (login with password)");
    std::vector<std::vector<std::string>> rows;
    demo += "0000";
    rows.push_back(Row("atm += \"0000\"", (demo == true) ? "ADMIN" : "GUEST", demo));
    demo += "1234";
    rows.push_back(Row("atm += \"1234\"", (demo == true) ? "ADMIN" : "GUEST", demo));
    PrintTable(HEAD, rows);

    PrintBox("Testing operator = (set money)");
    rows.clear();
    demo = 1003;
    rows.push_back(Row("atm = 1003", demo.GetMoney() == 1003 ? "OK" : "REJECTED", demo));
    demo = 1000;
    rows.push_back(Row("atm = 1000", demo.GetMoney() == 1000 ? "OK" : "REJECTED", demo));
    PrintTable(HEAD, rows);

    PrintBox("Testing operator >> (withdraw money)");
    rows.clear();
    demo.SetTapeLength(0.25);
    demo.Logout();
    int sum = 102;
    demo >> sum;
    rows.push_back(Row("atm >> 102", sum > 0 ? "GOT " + std::to_string(sum) : "REJECTED", demo));
    sum = 300;
    demo >> sum;
    rows.push_back(Row("atm >> 300", sum > 0 ? "GOT " + std::to_string(sum) : "REJECTED", demo));
    PrintTable(HEAD, rows);

    std::ifstream fin("input.txt");
    if (!fin) {
        Warn("Cannot open input.txt");
        return 1;
    }

    PrintBox("Commands from input.txt");
    ATM atm;
    atm.Reset();
    rows.clear();
    std::string command;
    while (fin >> command) {
        std::string text, result;
        if (command == "login") {
            std::string password;
            fin >> password;
            atm += password;
            text = "atm += \"" + password + "\"";
            result = (atm == true) ? "ADMIN" : "GUEST";
        } else if (command == "logout") {
            atm.Logout();
            text = "Logout()";
            result = "OK";
        } else if (command == "money") {
            int value;
            fin >> value;
            atm = value;
            text = "atm = " + std::to_string(value);
            result = atm.GetMoney() == value ? "OK" : "REJECTED";
        } else if (command == "tape") {
            double value;
            fin >> value;
            text = "SetTapeLength(" + Num(value) + ")";
            result = atm.SetTapeLength(value) ? "OK" : "REJECTED";
        } else if (command == "withdraw") {
            int value;
            fin >> value;
            text = "atm >> " + std::to_string(value);
            atm >> value;
            result = value > 0 ? "GOT " + std::to_string(value) : "REJECTED";
        }
        rows.push_back(Row(text, result, atm));
    }
    PrintTable(HEAD, rows);
    Done("Results saved to output.txt");
    return 0;
}
