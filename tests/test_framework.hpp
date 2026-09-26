#pragma once

// Minimal, dependency-free test framework for aspose-jmap-foss-cpp.
// Template-rendered (not an LLM task) - do not hand-edit generated copies of this file;
// LLM-generated test files only ever USE the macros below, never redefine them.

#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace aspose_jmap_test {

struct TestCase {
  std::string name;
  std::function<void()> fn;
};

inline std::vector<TestCase>& registry() {
  static std::vector<TestCase> tests;
  return tests;
}

struct Registrar {
  Registrar(const std::string& name, std::function<void()> fn) {
    registry().push_back(TestCase{name, std::move(fn)});
  }
};

struct AssertionFailure : std::runtime_error {
  using std::runtime_error::runtime_error;
};

}  // namespace aspose_jmap_test

#define AJ_TEST(name)                                                         \
  static void name();                                                        \
  static ::aspose_jmap_test::Registrar name##_registrar(#name, name);        \
  static void name()

#define AJ_ASSERT_TRUE(cond)                                                  \
  do {                                                                        \
    if (!(cond)) {                                                            \
      std::ostringstream oss;                                                 \
      oss << __FILE__ << ":" << __LINE__ << ": AJ_ASSERT_TRUE failed: " #cond; \
      throw ::aspose_jmap_test::AssertionFailure(oss.str());                  \
    }                                                                         \
  } while (0)

#define AJ_ASSERT_FALSE(cond)                                                 \
  do {                                                                        \
    if (cond) {                                                               \
      std::ostringstream oss;                                                 \
      oss << __FILE__ << ":" << __LINE__ << ": AJ_ASSERT_FALSE failed: " #cond; \
      throw ::aspose_jmap_test::AssertionFailure(oss.str());                  \
    }                                                                         \
  } while (0)

#define AJ_ASSERT_EQ(actual, expected)                                        \
  do {                                                                        \
    if (!((actual) == (expected))) {                                          \
      std::ostringstream oss;                                                 \
      oss << __FILE__ << ":" << __LINE__                                      \
          << ": AJ_ASSERT_EQ failed: " #actual " != " #expected;              \
      throw ::aspose_jmap_test::AssertionFailure(oss.str());                  \
    }                                                                         \
  } while (0)

#define AJ_ASSERT_THROWS(expr, ExceptionType)                                 \
  do {                                                                        \
    bool aj_threw = false;                                                    \
    try {                                                                     \
      (expr);                                                                 \
    } catch (const ExceptionType&) {                                          \
      aj_threw = true;                                                        \
    }                                                                         \
    if (!aj_threw) {                                                          \
      std::ostringstream oss;                                                 \
      oss << __FILE__ << ":" << __LINE__                                      \
          << ": AJ_ASSERT_THROWS failed: " #expr " did not throw " #ExceptionType; \
      throw ::aspose_jmap_test::AssertionFailure(oss.str());                  \
    }                                                                         \
  } while (0)
