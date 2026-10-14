#include <iomanip>
#include <iostream>
struct MinMax {
  int min;
  int max;
};
double Avg(const int* temps, int n) {
  int sum = 0;
  for (int i = 0; i < n; ++i) {
    sum += temps[i];
  }
  return static_cast<double>(sum) / n;
}
MinMax FindMinMax(const int* temps, int n) {
  MinMax result = {temps[0], temps[0]};
  for (int i = 1; i < n; ++i) {
    if (temps[i] < result.min) {
      result.min = temps[i];
    }
    if (temps[i] > result.max) {
      result.max = temps[i];
    }
  }
  return result;
}
int CountFrostDays(const int* temps, int n) {
  int cnt = 0;
  for (int i = 0; i < n; ++i) {
    if (temps[i] < 0) {
      ++cnt;
    }
  }
  return cnt;
}
int main() {
  int n;
  std::cin >> n;
  int* temps = new int[n];
  for (int i = 0; i < n; ++i) {
    std::cin >> temps[i];
  }
  MinMax mm = FindMinMax(temps, n);
  std::cout << std::fixed << std::setprecision(1) << Avg(temps, n) << '\n';
  std::cout << mm.min << ' ' << mm.max << '\n';
  std::cout << CountFrostDays(temps, n) << '\n';
  delete[] temps;
  return 0;
}