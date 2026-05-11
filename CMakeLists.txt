cmake_minimum_required(VERSION 3.28.1)
project(MagicalTower)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=GBK")
add_definitions(-D_XOPEN_SOURCE_EXTENDED)

# 共享的游戏逻辑源文件
set(GAME_SOURCES
	src/backpack.cpp src/calc.cpp src/displayClass.cpp
	src/framePrintChar.cpp src/map.cpp src/monster.cpp
	src/playerClass.cpp src/regionDisplay.cpp
	src/event/event_manager.cpp
)

# ============================================================
# 目标 1: ncurses 版本
# ============================================================
add_executable(MagicalTower
	${GAME_SOURCES}
	src/Cursor.cpp src/dialog.cpp src/key.cpp
	src/main.cpp src/statusBar.cpp
	logo/logo.rc
)
target_include_directories(MagicalTower PRIVATE ./inc)
target_link_directories(MagicalTower PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(MagicalTower ncursesw)

add_custom_command(TARGET MagicalTower POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${CMAKE_SOURCE_DIR}/lib/ncursesw.dll"
	"$<TARGET_FILE_DIR:MagicalTower>/ncursesw.dll"
)

# ============================================================
# 目标 2: SDL3 终端版本
# ============================================================
set(SDL3_ROOT "${CMAKE_SOURCE_DIR}/terminal/sdl3/SDL3-3.4.8/x86_64-w64-mingw32")

add_executable(MotaSDL
	${GAME_SOURCES}
	terminal/main.cpp terminal/sdl_dialog.cpp terminal/sdl_key.cpp
	terminal/sdl_statusBar.cpp terminal/sdl_terminal.cpp
)
target_include_directories(MotaSDL PRIVATE
	./inc terminal ${SDL3_ROOT}/include ${SDL3_ROOT}/include/SDL3
)
target_link_directories(MotaSDL PRIVATE ${SDL3_ROOT}/lib)
target_link_libraries(MotaSDL SDL3)
target_compile_definitions(MotaSDL PRIVATE SDL3_BUILD)

add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${SDL3_ROOT}/bin/SDL3.dll"
	"$<TARGET_FILE_DIR:MotaSDL>/SDL3.dll"
)
