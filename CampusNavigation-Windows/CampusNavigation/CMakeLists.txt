cmake_minimum_required(VERSION 3.18)
project(CampusNavigation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    CampusNavigation
    main.cpp
    LGraph.cpp
    Algorithm.cpp
)
