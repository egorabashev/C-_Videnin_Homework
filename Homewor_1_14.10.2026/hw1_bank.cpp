#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
using Command = void (*)(int&);
void Deposit(int& sum) {
  int k;
  std::cin >> k;
  if (k <= 0) {
    std::cout << "error\n";
    return;
  }
  sum += k;
}
void Withdraw(int& sum) {
  int k;
  std::cin >> k;
  if (k <= 0 || k > sum) {
    std::cout << "error\n";
    return;
  }
  sum -= k;
}
void Balance(int& sum) { std::cout << sum << '\n'; }
void Reset(int& sum) { sum = 0; }
void Double(int& sum) { sum *= 2; }
const char* const kNames[] = {"deposit", "withdraw", "balance", "reset",
                              "double"};
const Command kFuncs[] = {Deposit, Withdraw, Balance, Reset, Double};
const int kCount = sizeof(kNames) / sizeof(kNames[0]);
int main() {
  int sum = 0;
  char cmd[32];
  while (std::cin >> std::setw(sizeof(cmd)) >> cmd) {
    if (std::strcmp(cmd, "stop") == 0) {
      break;
    }
    bool found = false;
    for (int i = 0; i < kCount; ++i) {
      if (std::strcmp(cmd, kNames[i]) == 0) {
        kFuncs[i](sum);
        found = true;
        break;
      }
    }
    if (!found) {
      std::cout << "error\n";
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }
  return 0;
}