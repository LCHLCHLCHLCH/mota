cmake_minimum_required(VERSION 3.28.1)

project(MagicalTower)

aux_source_directory(./src SRC_LIST)
aux_source_directory(./src/event SRC_LIST)

include_directories(./inc)
link_directories(./lib)

add_executable(${PROJECT_NAME} ${SRC_LIST} ./logo/logo.rc)

target_link_libraries(${PROJECT_NAME} ncursesw)

add_definitions(-DNCURSES_STATIC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=GBK")
