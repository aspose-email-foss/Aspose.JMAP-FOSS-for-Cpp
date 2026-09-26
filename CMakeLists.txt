cmake_minimum_required(VERSION 3.15)
project(aspose-jmap-foss-cpp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

# Header-only library target - consumers link against this to get the include path
# and its one dependency (nlohmann::json); there is no compiled library artifact.
add_library(aspose-jmap-foss-cpp INTERFACE)
target_include_directories(aspose-jmap-foss-cpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(aspose-jmap-foss-cpp INTERFACE nlohmann_json::nlohmann_json)
target_compile_features(aspose-jmap-foss-cpp INTERFACE cxx_std_20)

if(WIN32)
  target_link_libraries(aspose-jmap-foss-cpp INTERFACE winhttp)
endif()

option(BUILD_TESTS "Build the aspose-jmap-foss-cpp unit tests" ON)
if(BUILD_TESTS)
  enable_testing()
  file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
  # Target name deliberately avoids "run_tests"/"RUN_TESTS": CMake's Visual Studio
  # generator reserves a case-insensitive "RUN_TESTS" meta-target (created by
  # enable_testing()) that invokes ctest as a build step - naming our own executable
  # target that (even in a different case) collides with it and breaks the build.
  add_executable(jmap_unit_tests ${TEST_SOURCES})
  target_link_libraries(jmap_unit_tests PRIVATE aspose-jmap-foss-cpp)
  add_test(NAME jmap_unit_tests COMMAND jmap_unit_tests)
endif()
