cmake_minimum_required(VERSION 3.15)
project(in10 VERSION 1.0 LANGUAGES CXX)

#if(NOT CMAKE_CXX_COMPILER)
#  set(CMAKE_CXX_COMPILER clang++)
#endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(fmt REQUIRED)

set(SOURCES
    main.cpp
    EasyVector.cpp
    Gate.cpp
    Orientation.cpp
    Scene.cpp
    Tile.cpp
    WireGroup.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system fmt::fmt)
