cmake_minimum_required(VERSION 3.28)
project(
    atom.editor
    VERSION 0.1.0
    LANGUAGES CXX C)

cmake_policy(SET CMP0155 NEW)

include(FetchContent)
set(FETCHCONTENT_QUIET "ON")

set(CMAKE_CXX_STANDARD "23")
set(CMAKE_CXX_STANDARD_REQUIRED "ON")
enable_testing()

FetchContent_Declare(
    atom_engine
    GIT_REPOSITORY "https://github.com/shifu-dev/atom.engine.git"
    GIT_TAG "main"
    GIT_SHALLOW true
    EXCLUDE_FROM_ALL)

FetchContent_MakeAvailable(atom_engine)

# --------------------------------------------------------------------------------------------------
# atom.editor
# --------------------------------------------------------------------------------------------------

add_executable(atom.editor)

file(GLOB_RECURSE modules "source/*.cppm")
file(GLOB_RECURSE sources "source/*.cpp")

target_sources(atom.editor PUBLIC FILE_SET CXX_MODULES FILES "${modules}")
target_sources(atom.editor PUBLIC "${sources}")
target_link_libraries(atom.editor PUBLIC atom.engine)
