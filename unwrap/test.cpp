#define UNWRAP_TEST_LIBRARY
#include "test.hpp"

__attribute__((__visibility__("default")))
bool success() noexcept {
  return true;
}

__attribute__((__visibility__("default")))
unwrap_test* unwrap_tests{nullptr};

template <class T>
constexpr T* reverse(T* head) noexcept {
  T* list = nullptr;
  while (head) {
    const auto next = head->next;
    head->next = list;
    list = head;
    head = next;
  }
  return list;
}

__attribute__((__visibility__("default")))
int main() {
  for (auto test = reverse(unwrap_tests); test; test = test->next) {
    test->func();
  }
  return EXIT_SUCCESS;
}
