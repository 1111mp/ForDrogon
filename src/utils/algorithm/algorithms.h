#pragma once

#include <vector>

namespace Utils
{

  class Algorithms
  {
  public:
    static int dichotomy(const std::vector<int> &source, const int target);
    static void selectionSort(std::vector<int> &source);
  };

}