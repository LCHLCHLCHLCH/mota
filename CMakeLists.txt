cmake_minimum_required(VERSION 3.28.1)
project(MagicalTower)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=GBK")
add_definitions(-D_XOPEN_SOURCE_EXTENDED)

# MinGW 运行时 DLL 路径
get_filename_component(MINGW_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)

# 共享的游戏逻辑源文件（不含渲染层）
set(GAME_SOURCES
	src/game/map.cpp src/game/monster.cpp src/game/player.cpp
	src/event/event_manager.cpp
	src/render/display.cpp src/render/frame_print.cpp
	src/render/region_display.cpp src/ui/backpack.cpp
)

# ============================================================
# 目标 1: ncurses 版本
# ============================================================
add_executable(MagicalTower
	${GAME_SOURCES}
	src/main.cpp
	src/render/cursor.cpp src/render/key.cpp src/render/status_bar.cpp
	src/event/dialog.cpp
	logo/logo.rc
)
target_include_directories(MagicalTower PRIVATE src src/render)
target_link_directories(MagicalTower PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(MagicalTower ncursesw)

add_custom_command(TARGET MagicalTower POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${CMAKE_SOURCE_DIR}/lib/ncursesw.dll"
	"$<TARGET_FILE_DIR:MagicalTower>/ncursesw.dll"
)
add_custom_command(TARGET MagicalTower POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${MINGW_BIN}/libstdc++-6.dll"
	"$<TARGET_FILE_DIR:MagicalTower>/libstdc++-6.dll"
)
add_custom_command(TARGET MagicalTower POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${MINGW_BIN}/libgcc_s_seh-1.dll"
	"$<TARGET_FILE_DIR:MagicalTower>/libgcc_s_seh-1.dll"
)
add_custom_command(TARGET MagicalTower POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${MINGW_BIN}/libwinpthread-1.dll"
	"$<TARGET_FILE_DIR:MagicalTower>/libwinpthread-1.dll"
)

# ============================================================
# 目标 2: SDL3 终端版本
# ============================================================
set(SDL3_ROOT "${CMAKE_SOURCE_DIR}/terminal/sdl3/SDL3-3.4.8/x86_64-w64-mingw32")

add_executable(MotaSDL
	${GAME_SOURCES}
	terminal/main.cpp
	terminal/sdl_dialog.cpp terminal/sdl_key.cpp
	terminal/sdl_statusBar.cpp terminal/sdl_terminal.cpp
	terminal/console_cmd.cpp
	terminal/save_system.cpp
)
target_include_directories(MotaSDL PRIVATE
	src src/render terminal ${SDL3_ROOT}/include ${SDL3_ROOT}/include/SDL3
)
target_link_directories(MotaSDL PRIVATE ${SDL3_ROOT}/lib)
target_link_libraries(MotaSDL SDL3)
target_compile_definitions(MotaSDL PRIVATE SDL3_BUILD)

add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${SDL3_ROOT}/bin/SDL3.dll"
	"$<TARGET_FILE_DIR:MotaSDL>/SDL3.dll"
)
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${MINGW_BIN}/libstdc++-6.dll"
	"$<TARGET_FILE_DIR:MotaSDL>/libstdc++-6.dll"
)
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${MINGW_BIN}/libgcc_s_seh-1.dll"
	"$<TARGET_FILE_DIR:MotaSDL>/libgcc_s_seh-1.dll"
)
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${MINGW_BIN}/libwinpthread-1.dll"
	"$<TARGET_FILE_DIR:MotaSDL>/libwinpthread-1.dll"
)
