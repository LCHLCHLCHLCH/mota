cmake_minimum_required(VERSION 3.28.1)
project(MotaSDL)

get_filename_component(MINGW_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)

set(SDL3_ROOT "${CMAKE_SOURCE_DIR}/lib/SDL3-3.4.8/x86_64-w64-mingw32")
set(SDL3_TTF_ROOT "${CMAKE_SOURCE_DIR}/lib/SDL3_ttf-3.2.2/x86_64-w64-mingw32")

add_library(lua SHARED
	lua-5.5.0/lapi.c lua-5.5.0/lauxlib.c lua-5.5.0/lbaselib.c lua-5.5.0/lcode.c
	lua-5.5.0/lcorolib.c lua-5.5.0/lctype.c lua-5.5.0/ldblib.c lua-5.5.0/ldebug.c
	lua-5.5.0/ldo.c lua-5.5.0/ldump.c lua-5.5.0/lfunc.c lua-5.5.0/lgc.c
	lua-5.5.0/linit.c lua-5.5.0/liolib.c lua-5.5.0/llex.c lua-5.5.0/lmathlib.c
	lua-5.5.0/lmem.c lua-5.5.0/loadlib.c lua-5.5.0/lobject.c lua-5.5.0/lopcodes.c
	lua-5.5.0/loslib.c lua-5.5.0/lparser.c lua-5.5.0/lstate.c lua-5.5.0/lstring.c
	lua-5.5.0/lstrlib.c lua-5.5.0/ltable.c lua-5.5.0/ltablib.c lua-5.5.0/ltm.c
	lua-5.5.0/lundump.c lua-5.5.0/lutf8lib.c lua-5.5.0/lvm.c lua-5.5.0/lzio.c
)
target_include_directories(lua PRIVATE lua-5.5.0)
set_target_properties(lua PROPERTIES PREFIX "")

add_executable(MotaSDL
	src/game/map.cpp src/game/monster.cpp src/game/player.cpp
	src/game/item.cpp src/game/tile_data.cpp src/game/zone.cpp
	src/event/event_manager.cpp
	src/render/display.cpp src/render/frame_print.cpp
	src/render/region_display.cpp src/ui/backpack.cpp
	src/ui/monster_book.cpp src/ui/victory.cpp src/ui/notebook.cpp
	terminal/main.cpp
	terminal/sdl_dialog.cpp terminal/sdl_key.cpp
	terminal/sdl_statusBar.cpp terminal/sdl_terminal.cpp
	terminal/console_cmd.cpp
	terminal/save_system.cpp
	terminal/sdl_3dwindow.cpp
	src/script/lua_state.cpp src/script/lua_bridge.cpp
	logo/sdl_logo.rc
)

target_include_directories(MotaSDL PRIVATE
	src src/render terminal src/script
	${SDL3_ROOT}/include ${SDL3_ROOT}/include/SDL3
	${SDL3_TTF_ROOT}/include ${SDL3_TTF_ROOT}/include/SDL3_ttf
	lua-5.5.0
)
target_link_directories(MotaSDL PRIVATE ${SDL3_ROOT}/lib ${SDL3_TTF_ROOT}/lib)
target_link_libraries(MotaSDL SDL3 SDL3_ttf lua)
target_compile_definitions(MotaSDL PRIVATE SDL3_BUILD)

add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${SDL3_ROOT}/bin/SDL3.dll"   "$<TARGET_FILE_DIR:MotaSDL>/SDL3.dll"
)
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${SDL3_TTF_ROOT}/bin/SDL3_ttf.dll" "$<TARGET_FILE_DIR:MotaSDL>/SDL3_ttf.dll"
)
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"$<TARGET_FILE:lua>"  "$<TARGET_FILE_DIR:MotaSDL>/$<TARGET_FILE_NAME:lua>"
)
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${MINGW_BIN}/libstdc++-6.dll"   "$<TARGET_FILE_DIR:MotaSDL>/libstdc++-6.dll"
)
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${MINGW_BIN}/libgcc_s_seh-1.dll" "$<TARGET_FILE_DIR:MotaSDL>/libgcc_s_seh-1.dll"
)
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${MINGW_BIN}/libwinpthread-1.dll" "$<TARGET_FILE_DIR:MotaSDL>/libwinpthread-1.dll"
)
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${CMAKE_SOURCE_DIR}/lib/simsun.ttc" "$<TARGET_FILE_DIR:MotaSDL>/simsun.ttc"
)
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/map" "$<TARGET_FILE_DIR:MotaSDL>/map"
)

# 地图 Lua 文件变更时独立同步到输出目录
# （POST_BUILD 仅在重链接时执行，纯 Lua 改动不会触发，故单独处理）
file(GLOB MAP_LUA_FILES "${CMAKE_SOURCE_DIR}/map/*.lua")
add_custom_command(
	OUTPUT "${CMAKE_BINARY_DIR}/map_synced.stamp"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/map" "$<TARGET_FILE_DIR:MotaSDL>/map"
	COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/map_synced.stamp"
	DEPENDS ${MAP_LUA_FILES}
)
add_custom_target(map_sync ALL DEPENDS "${CMAKE_BINARY_DIR}/map_synced.stamp")
