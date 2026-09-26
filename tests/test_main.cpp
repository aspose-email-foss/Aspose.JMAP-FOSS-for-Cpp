// Test binary entry point for aspose-jmap-foss-cpp. Template-rendered (not an LLM task) - runs
// every AJ_TEST registered by any linked test file (see test_framework.hpp).

#include "test_framework.hpp"

#include <iostream>

int main() {
  int failed = 0;
  for (const auto& testCase : aspose_jmap_test::registry()) {
    try {
      testCase.fn();
      std::cout << "[PASS] " << testCase.name << "\n";
    } catch (const std::exception& e) {
      std::cout << "[FAIL] " << testCase.name << ": " << e.what() << "\n";
      failed++;
    } catch (...) {
      std::cout << "[FAIL] " << testCase.name << ": unknown exception\n";
      failed++;
    }
  }
  std::cout << aspose_jmap_test::registry().size() << " tests, " << failed << " failed\n";
  return failed == 0 ? 0 : 1;
}
