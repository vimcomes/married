cmake_minimum_required(VERSION 3.20)

project(desire_simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Curses REQUIRED)
find_package(Threads REQUIRED)

add_executable(desire_simulator
    src/main.cpp
    src/app/DesireSimulator.cpp
    src/core/DesireList.cpp
    src/core/History.cpp
    src/ui/Terminal.cpp
    src/ui/Layout.cpp
    src/ui/TableRenderer.cpp
    src/ui/HudRenderer.cpp
)

target_link_libraries(desire_simulator PRIVATE ${CURSES_LIBRARIES} Threads::Threads)
target_include_directories(desire_simulator PRIVATE ${CURSES_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)
