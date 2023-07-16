cmake_minimum_required(VERSION 3.5)

project(qtTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
    Sql
    )

set(SOURCES
    src/main.cpp
    src/Window.cpp
    src/ScreenThread.cpp)

set(HEADERS
    include/Window.h
    include/ScreenThread.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(qtTest PRIVATE include)
target_link_libraries(qtTest PRIVATE Qt5::Core Qt5::Widgets Qt5::Sql)
