#ifndef ATM_H
#define ATM_H

#include <string>

enum class AtmState { Working, NotWorking, Error };
enum class UserLevel { Guest, Admin };

class ATM
{
public:
    static const int MAX_MONEY = 100000;
    static const int MAX_TAPE_CM = 3000;
    static const int CHECK_CM = 10;
    static const int MIN_MONEY = 50;

    void Reset();
    bool Authorize(UserLevel newLevel, const std::string &password = "");
    void Logout();
    bool SetMoney(int sum);
    bool SetTapeLength(double meters);
    bool Withdraw(int sum);

    ATM &operator+=(const std::string &password);
    bool operator==(bool admin) const;
    ATM &operator=(int sum);
    ATM &operator>>(int &sum);

    AtmState GetState() const { return state; }
    int GetMoney() const { return money; }
    double GetTapeLength() const { return tapeCm / 100.0; }
    bool GetNoTape() const { return noTape; }
    UserLevel GetLevel() const { return level; }
    std::string GetStateName() const;
    std::string GetLevelName() const;

private:
    AtmState state;
    int money;
    int tapeCm;
    bool noTape;
    UserLevel level;

    static const std::string ADMIN_PASSWORD;

    bool IsAdmin() const { return level == UserLevel::Admin; }
    void UpdateState();
};

#endif
