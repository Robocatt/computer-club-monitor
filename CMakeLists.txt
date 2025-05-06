cmake_minimum_required(VERSION 3.16)
project(ClubMonitor LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(lib STATIC utils.cpp)
target_compile_options(lib PRIVATE -O3)


add_executable(main main.cpp)
target_link_libraries(main PRIVATE lib)


option(BUILD_TESTING "Enable unit tests" ON)

if(BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
  )
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  include(GoogleTest)

  add_executable(tests
      tests/test_time.cpp
      tests/test_name.cpp
  )
  target_link_libraries(tests PRIVATE lib gtest_main)
  target_compile_definitions(tests PRIVATE UNIT_TEST)
  target_compile_options(tests PRIVATE -Wall -Wextra -pedantic -O3)

  gtest_discover_tests(tests DISCOVERY_TIMEOUT 10)
endif()
