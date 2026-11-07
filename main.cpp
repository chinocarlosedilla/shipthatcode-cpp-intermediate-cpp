#include <cstddef>
#include <iostream>

// Return a pointer to the LAST occurrence of the largest value in [first, last).
// Return last itself when the range is empty.
const int* lastMax(const int* first, const int* last) {
  // TODO: walk the range with a pointer and remember where the best value sits
  const int* best = first;

  for(const int* i = first; i < last; ++i) {
    if(*best <= *i) {
      best = i;
    }
  }
  return best;
}

// Reverse the elements of [first, last) in place. last is one past the final element.
void reverseRange(int* first, int* last) {
  // TODO: swap elements from both ends, moving the two pointers toward each other
  const int dist = (last - first) / 2;
  int swapper;
  for(int i=1; i<=dist; ++i) {
    swapper = *(last-1);
    *(last-1) = *first;
    *first = swapper;
    ++first;
    --last;
  }
}

int main() {
    int a[1000];
    int n = 0;
    std::cin >> n;
    for (int* p = a; p != a + n; ++p) std::cin >> *p;

    const int* best = lastMax(a, a + n);
    if (best == a + n) {
        std::cout << "empty\n";
        return 0;
    }
    std::ptrdiff_t index = best -a;  // TODO: how far is best from the start of a?
    std::cout << "max=" << *best << " index=" << index << "\n";

    reverseRange(a, a + n);
    for (const int* p = a; p != a + n; ++p) {
        if (p != a) std::cout << " ";
        std::cout << *p;
    }
    std::cout << "\n";
    return 0;
}
