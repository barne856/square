cmake_minimum_required(VERSION 3.20)

## Project Settings
project(LIT)
set(CMAKE_CXX_STANDARD 20)
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
#find_package(SDL2 REQUIRED)
#find_package(SDL_IMAGE REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
include_directories("./include" "./ext/SQUINT/include" ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
set(BUILD_TESTING OFF) # disable Catch2 self tests

if(MSVC)
add_definitions(
    -DTW_STATIC
    -DTW_NO_LIB_PRAGMA
    -DTW_NO_DIRECT3D
    -DGLEW_STATIC
)
endif()

## Build Library
set(LIB_SRC
src/entity.cpp
src/renderer.cpp
src/sdl_gl.cpp
src/components/transform.cpp
)
add_library(LIT ${LIB_SRC})
target_link_libraries(LIT  ${SDL2_LIBRARIES}  ${SDL2IMAGE_LIBRARIES} OpenGL::GL GLEW::GLEW)

## Build Sample Apps
add_executable(solid_color sample_apps/solid_color/solid_color.cpp)
target_link_libraries(solid_color LIT)

add_executable(sample_scene sample_apps/sample_scene/sample_scene.cpp)
target_link_libraries(sample_scene LIT)

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
