cmake_minimum_required(VERSION 3.16)

project(mydb LANGUAGES CXX)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
 
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v6.1.9  # Replace with a version, tag, or commit hash
)
 
FetchContent_MakeAvailable(ftxui)

# Build type (default to Debug)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Collect source files
file(GLOB SRC_FILES
    src/*.cpp
    src/core/*.cpp
    src/query/*.cpp
    src/parser/*.cpp
    src/page/*.cpp
    src/heap_table/*.cpp
    src/context/*.cpp
    src/gnu_readline/*.cpp
    src/buffer_pool/*.cpp

)


# Create executable
add_executable(shell ${SRC_FILES})

# Include directories
target_include_directories(shell PRIVATE
    src
)

# Link readline
target_link_libraries(shell PRIVATE 
    readline
    ftxui::screen
    ftxui::dom
    ftxui::component
)


