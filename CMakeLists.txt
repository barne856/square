cmake_minimum_required(VERSION 3.28)

## Project Settings
project(square)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -m64")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++abi")
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
#find_package(SDL2 REQUIRED)
#find_package(SDL_IMAGE REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
include_directories("./include" "./ext/SQUINT/include" ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
set(BUILD_TESTING OFF) # disable Catch2 self tests

add_library(squint)
target_sources(squint PUBLIC FILE_SET CXX_MODULES FILES
  ext/SQUINT/src/dimension.cpp
  ext/SQUINT/src/linalg.cpp
  ext/SQUINT/src/optimize.cpp
  ext/SQUINT/src/quantity.cpp
  ext/SQUINT/src/tensor.cpp
  ext/SQUINT/src/squint.cpp
)

if(MSVC)
add_definitions(
    -DTW_STATIC
    -DTW_NO_LIB_PRAGMA
    -DTW_NO_DIRECT3D
    -DGLEW_STATIC
)
endif()
# export module square;
# export import :char_mesh;
# export import :circle_mesh;
# export import :cube_mesh;
# export import :cylinder_mesh;
# export import :line_mesh;
# export import :sphere_mesh;
# export import :square_mesh;
# export import :torus_mesh;
# export import :color;
# export import :mesh;
# export import :transform;
# export import :basic_color;
# export import :basic_texture;
# export import :camera;
# export import :material;
# export import :entity;
# export import :renderer;
# export import :sdl_gl;
# export import :system;
## Build Library
set(LIB_SRC
src/square.cpp
include/square/components/meshes/char_mesh.cpp
include/square/components/meshes/circle_mesh.cpp
include/square/components/meshes/cube_mesh.cpp
include/square/components/meshes/cylinder_mesh.cpp
include/square/components/meshes/line_mesh.cpp
include/square/components/meshes/sphere_mesh.cpp
include/square/components/meshes/square_mesh.cpp
include/square/components/meshes/torus_mesh.cpp
include/square/components/color.cpp
include/square/components/mesh.cpp
include/square/components/transform.cpp
include/square/entities/materials/basic_color.cpp
include/square/entities/materials/basic_texture.cpp
include/square/entities/camera.cpp
include/square/entities/material.cpp
include/square/entity.cpp
include/square/renderer.cpp
include/square/sdl_gl.cpp
include/square/system.cpp
)
add_library(square)
target_sources(square PUBLIC FILE_SET CXX_MODULES FILES ${LIB_SRC})
target_link_libraries(square ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} OpenGL::GL GLEW::GLEW squint)

## Build Sample Apps
add_executable(solid_color sample_apps/solid_color/solid_color.cpp)
target_link_libraries(solid_color square squint)

add_executable(sample_scene sample_apps/sample_scene/sample_scene.cpp)
target_link_libraries(sample_scene square)

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
