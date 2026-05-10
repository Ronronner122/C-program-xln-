cmake_minimum_required(VERSION 3.18)
project(Evan_pet1 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "C:/Qt/6.5.3/mingw_64")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    Multimedia
    Network
)

qt_add_executable(Evan_pet1
    main.cpp
    widget.cpp
    widget.h
    pet.qrc
)

target_link_libraries(Evan_pet1 PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Network
)