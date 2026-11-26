#include "atm.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

class TeeBuf : public std::streambuf
{
public:
    TeeBuf(std::streambuf *first, std::streambuf *second) : a(first), b(second) {}

protected:
    int overflow(int c) override
    {
        if (c == EOF) return 0;
        if (a->sputc(c) == EOF || b->sputc(c) == EOF) return EOF;
        return c;
    }
    int sync() override
    {
        return (a->pubsync() == 0 && b->pubsync() == 0) ? 0 : -1;
    }

private:
    std::streambuf *a;
    std::streambuf *b;
};

std::ofstream fout("output.txt");
TeeBuf tee(std::cout.rdbuf(), fout.rdbuf());
std::streambuf *consoleBuf = std::cout.rdbuf(&tee);

struct RestoreCout
{
    ~RestoreCout()
    {
        std::cout.flush();
        std::cout.rdbuf(consoleBuf);
    }
} restoreCout;

void print(const std::string &text)
{
    std::cout << text << std::endl;
}

void title(const std::string &text)
{
    const int width = 78;
    int left = (width - 2 - static_cast<int>(text.size())) / 2;
    int right = width - 2 - static_cast<int>(text.size()) - left;
    print(std::string(width, '-'));
    print("|" + std::string(left, ' ') + text + std::string(right, ' ') + "|");
    print(std::string(width, '-'));
}

void section(const std::string &text)
{
    print("");
    print("[ " + text + " ]");
}

void blockBegin(const std::string &name)
{
    const int width = 40;
    int left = (width - 2 - static_cast<int>(name.size())) / 2;
    int right = width - 2 - static_cast<int>(name.size()) - left;
    print(std::string(left, '-') + " " + name + " " + std::string(right, '-'));
}

void blockEnd()
{
    print(std::string(40, '-'));
}

void field(const std::string &name, const std::string &value)
{
    std::ostringstream s;
    s << std::left << std::setw(14) << name << ": " << value;
    print(s.str());
}

std::string line(const std::vector<int> &widths)
{
    std::string s;
    for (int w : widths)
        s += "+" + std::string(w + 2, '-');
    return s + "+";
}

std::string row(const std::vector<std::string> &cells, const std::vector<int> &widths)
{
    std::ostringstream s;
    for (size_t i = 0; i < cells.size(); i++)
        s << "| " << std::left << std::setw(widths[i]) << cells[i] << " ";
    s << "|";
    return s.str();
}

std::string num(double value, int digits = 2)
{
    std::ostringstream s;
    s << std::fixed << std::setprecision(digits) << value;
    return s.str();
}

void showState(const std::string &name, const ATM &atm)
{
    blockBegin(name);
    field("State", atm.GetStateName());
    field("Money", std::to_string(atm.GetMoney()) + " UAH");
    field("Tape", num(atm.GetTapeLength()) + " m");
    field("No tape", atm.GetNoTape() ? "YES" : "NO");
    field("User", atm.GetLevelName());
    blockEnd();
}

std::vector<std::string> stateCells(const ATM &atm)
{
    return {atm.GetStateName(), std::to_string(atm.GetMoney()), num(atm.GetTapeLength()),
            atm.GetNoTape() ? "YES" : "NO", atm.GetLevelName()};
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::ifstream fin("input.txt");
    if (!fin) {
        print("ERROR: cannot open input.txt");
        return 1;
    }

    ATM atm;
    atm.Reset();

    title("ATM OPERATORS TEST (VARIANT 10)");
    section("OPERATORS");
    print("atm += \"password\"  -> login");
    print("atm == true        -> check if user is ADMIN");
    print("atm = sum          -> set money");
    print("atm >> sum         -> withdraw, sum gets the given money");
    section("INITIAL STATE AFTER Reset()");
    showState("STATE", atm);

    section("COMMANDS FROM input.txt");
    std::vector<int> w = {2, 14, 8, 11, 5, 4, 7, 5};
    print(line(w));
    print(row({"#", "Operator", "Result", "State", "Money", "Tape", "No tape", "User"}, w));
    print(line(w));

    std::string command;
    int step = 1;
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
            int sum;
            fin >> sum;
            atm = sum;
            text = "atm = " + std::to_string(sum);
            result = atm.GetMoney() == sum ? "OK" : "REJECTED";
        } else if (command == "tape") {
            double value;
            fin >> value;
            text = "tape " + num(value);
            result = atm.SetTapeLength(value) ? "OK" : "REJECTED";
        } else if (command == "withdraw") {
            int sum;
            fin >> sum;
            text = "atm >> " + std::to_string(sum);
            atm >> sum;
            result = sum > 0 ? "GOT " + std::to_string(sum) : "REJECTED";
        }

        std::vector<std::string> cells = {std::to_string(step++), text, result};
        for (const std::string &cell : stateCells(atm))
            cells.push_back(cell);
        print(row(cells, w));
    }
    print(line(w));

    section("FINAL STATE");
    showState("STATE", atm);
    print("");
    print(">> Results saved to output.txt");
    return 0;
}
