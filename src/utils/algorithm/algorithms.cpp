#include "algorithms.h"

namespace Utils
{
  // O(log2n)
  int Algorithms::dichotomy(const std::vector<int> &source, const int target)
  {
    unsigned int low = 0, hight = source.size() - 1;

    while (low <= hight)
    {
      unsigned int mid = (low + hight) / 2;
      auto guess = source[mid];

      if (target == guess)
      {
        return mid;
      }
      else if (target < guess)
      {
        hight = mid - 1;
      }
      else
      {
        low = mid + 1;
      }
    }

    return -1;
  }

  // O(n^2)
  void Algorithms::selectionSort(std::vector<int> &source)
  {
    unsigned int min;
    for (unsigned int i = 0; i < source.size(); ++i)
    {
      min = i;
      for (unsigned int j = i + 1; j < source.size(); ++j)
      {
        if (source[j] < source[min])
          min = j;
      }
      std::swap(source[i], source[min]);
    }
  }
}