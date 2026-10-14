#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
const int kRows = 5;
const int kCols = 10;
bool InBounds(int x, int y) {
  return x >= 0 && x < kCols && y >= 0 && y < kRows;
}
void Clear(char board[kRows][kCols]) {
  for (int y = 0; y < kRows; ++y) {
    for (int x = 0; x < kCols; ++x) {
      board[y][x] = ' ';
    }
  }
}
bool Put(char board[kRows][kCols], int x, int y, char c) {
  if (!InBounds(x, y)) {
    return false;
  }
  board[y][x] = c;
  return true;
}
bool Text(char board[kRows][kCols], int x, int y, const char* word) {
  int len = std::strlen(word);
  if (len == 0 || !InBounds(x, y) || !InBounds(x + len - 1, y)) {
    return false;
  }
  for (int i = 0; i < len; ++i) {
    Put(board, x + i, y, word[i]);
  }
  return true;
}
void Print(const char board[kRows][kCols]) {
  for (int y = 0; y < kRows; ++y) {
    for (int x = 0; x < kCols; ++x) {
      std::cout << board[y][x];
    }
    std::cout << '\n';
  }
}
int main() {
  char board[kRows][kCols];
  Clear(board);
  char cmd[32];
  while (std::cin >> std::setw(sizeof(cmd)) >> cmd) {
    if (std::strcmp(cmd, "quit") == 0) {
      break;
    } else if (std::strcmp(cmd, "clear") == 0) {
      Clear(board);
    } else if (std::strcmp(cmd, "put") == 0) {
      int x, y;
      char c;
      std::cin >> x >> y >> c;
      if (!Put(board, x, y, c)) {
        std::cout << "error\n";
      }
    } else if (std::strcmp(cmd, "text") == 0) {
      int x, y;
      char word[64];
      std::cin >> x >> y >> std::setw(sizeof(word)) >> word;
      if (!Text(board, x, y, word)) {
        std::cout << "error\n";
      }
    } else if (std::strcmp(cmd, "print") == 0) {
      Print(board);
    } else {
      std::cout << "error\n";
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }
  return 0;
}