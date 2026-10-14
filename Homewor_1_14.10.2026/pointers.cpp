#include <cstddef>
#include <iomanip>
#include <iostream>
int main() {
  int groups, exams;
  std::cin >> groups >> exams;
  int* sizes = new int[groups];
  int total_students = 0;
  for (int g = 0; g < groups; ++g) {
    std::cin >> sizes[g];
    total_students += sizes[g];
  }
  std::size_t bytes = groups * sizeof(int**) +
                      total_students * sizeof(int*) +
                      total_students * exams * sizeof(int);
  void* memory = ::operator new(bytes);
  char* cursor = static_cast<char*>(memory);
  int*** journal = reinterpret_cast<int***>(cursor);
  cursor += groups * sizeof(int**);
  int** student_rows = reinterpret_cast<int**>(cursor);
  cursor += total_students * sizeof(int*);
  int* cells = reinterpret_cast<int*>(cursor);
  for (int g = 0; g < groups; ++g) {
    journal[g] = student_rows;
    student_rows += sizes[g];
    for (int s = 0; s < sizes[g]; ++s) {
      journal[g][s] = cells;
      cells += exams;
    }
  }
  for (int g = 0; g < groups; ++g) {
    for (int s = 0; s < sizes[g]; ++s) {
      for (int e = 0; e < exams; ++e) {
        std::cin >> journal[g][s][e];
      }
    }
  }
  std::cout << std::fixed << std::setprecision(1);
  for (int g = 0; g < groups; ++g) {
    if (sizes[g] == 0 || exams == 0) {
      std::cout << "Group " << g + 1 << ": no data\n";
      continue;
    }
    int group_sum = 0;
    int best = 0;
    int best_sum = -1;
    for (int s = 0; s < sizes[g]; ++s) {
      int student_sum = 0;
      for (int e = 0; e < exams; ++e) {
        student_sum += journal[g][s][e];
      }
      group_sum += student_sum;
      if (student_sum > best_sum) {
        best_sum = student_sum;
        best = s;
      }
    }
    double average = static_cast<double>(group_sum) / (sizes[g] * exams);
    std::cout << "Group " << g + 1 << ": average " << average
              << ", best student " << best + 1 << '\n';
  }
  ::operator delete(memory);
  delete[] sizes;
  return 0;
}