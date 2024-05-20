cmake_minimum_required(VERSION 3.28)
project(
    atom.editor
    VERSION 0.1.0
    LANGUAGES CXX C)

cmake_policy(SET CMP0155 NEW)

find_package(atom_core REQUIRED)
find_package(atom_logging REQUIRED)
find_package(atom_engine REQUIRED)

# --------------------------------------------------------------------------------------------------
# atom.editor
# --------------------------------------------------------------------------------------------------

add_executable(atom.editor)

file(GLOB_RECURSE modules "source/*.cppm")
file(GLOB_RECURSE sources "source/*.cpp")

target_sources(
    atom.editor
    PUBLIC FILE_SET CXX_MODULES FILES "${modules}"
    PUBLIC "${sources}")

target_link_libraries(atom.editor PRIVATE atom::atom.core atom::atom.logging atom::atom.engine)

# --------------------------------------------------------------------------------------------------
# install phase
# --------------------------------------------------------------------------------------------------

install(
    TARGETS atom.editor
    RUNTIME DESTINATION "bin")
