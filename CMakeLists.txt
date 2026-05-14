cmake_minimum_required(VERSION 3.28.1)
project(MotaSDL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=GBK")

# MinGW 运行时 DLL 路径
get_filename_component(MINGW_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)

set(SDL3_ROOT "${CMAKE_SOURCE_DIR}/lib/SDL3-3.4.8/x86_64-w64-mingw32")
set(SDL3_TTF_ROOT "${CMAKE_SOURCE_DIR}/lib/SDL3_ttf-3.2.2/x86_64-w64-mingw32")

add_executable(MotaSDL
	src/game/map.cpp src/game/monster.cpp src/game/player.cpp
	src/game/item.cpp
	src/event/event_manager.cpp
	src/render/display.cpp src/render/frame_print.cpp
	src/render/region_display.cpp src/ui/backpack.cpp
	terminal/main.cpp
	terminal/sdl_dialog.cpp terminal/sdl_key.cpp
	terminal/sdl_statusBar.cpp terminal/sdl_terminal.cpp
	terminal/console_cmd.cpp
	terminal/save_system.cpp
	logo/sdl_logo.rc
)
target_include_directories(MotaSDL PRIVATE
	src src/render terminal ${SDL3_ROOT}/include ${SDL3_ROOT}/include/SDL3
	${SDL3_TTF_ROOT}/include ${SDL3_TTF_ROOT}/include/SDL3_ttf
)
target_link_directories(MotaSDL PRIVATE ${SDL3_ROOT}/lib ${SDL3_TTF_ROOT}/lib)
target_link_libraries(MotaSDL SDL3 SDL3_ttf)
target_compile_definitions(MotaSDL PRIVATE SDL3_BUILD)

add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${SDL3_ROOT}/bin/SDL3.dll"
	"$<TARGET_FILE_DIR:MotaSDL>/SDL3.dll"
)
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${SDL3_TTF_ROOT}/bin/SDL3_ttf.dll"
	"$<TARGET_FILE_DIR:MotaSDL>/SDL3_ttf.dll"
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
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${CMAKE_SOURCE_DIR}/lib/simsun.ttc"
	"$<TARGET_FILE_DIR:MotaSDL>/simsun.ttc"
)
