cmake_minimum_required(VERSION 3.20)

## Project Settings
project(LIT)
set(CMAKE_CXX_STANDARD 20)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories("./include" "./ext/SQUINT/include" ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
set(BUILD_TESTING OFF) # disable Catch2 self tests

## Build Library
set(LIB_SRC
src/entity.cpp
src/renderer.cpp
)
add_library(LIT ${LIB_SRC})
target_link_libraries(LIT SDL2::SDL2 OpenGL::GL)

## Build Sample Apps
add_executable(solid_color sample_apps/solid_color/main.cpp)
target_link_libraries(solid_color LIT)

## Include Catch2 for Unit Tests
Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

## Create the tests
add_executable(tests tests/tests.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(tests)
