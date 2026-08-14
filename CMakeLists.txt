cmake_minimum_required(VERSION 3.28.1)
project(MotaSDL)

get_filename_component(MINGW_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)

# 本机直连 github 失败时（schannel 问题），可用 HTTPS_PROXY/HTTP_PROXY 环境变量走代理
if(DEFINED ENV{HTTPS_PROXY} AND NOT DEFINED CMAKE_HTTPS_PROXY)
    set(CMAKE_HTTPS_PROXY "$ENV{HTTPS_PROXY}")
endif()
if(DEFINED ENV{HTTP_PROXY} AND NOT DEFINED CMAKE_HTTP_PROXY)
    set(CMAKE_HTTP_PROXY "$ENV{HTTP_PROXY}")
endif()

# ============================================================
# SDL3 / SDL3_ttf 最小化构建（FetchContent 自动下载源码）
# 仅保留游戏需要的 VIDEO/RENDER/事件，禁用音频/手柄/传感器等
# ============================================================
include(FetchContent)

# 允许 FetchContent_Populate（SDL3_ttf 的 vendored FreeType 需要手动放置源码）
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

# --- SDL3 最小化配置 ---
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_GPU OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)

# --- SDL3_ttf 最小化配置（仅 FreeType，无 HarfBuzz/SVG） ---
set(BUILD_SHARED_LIBS ON)
set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
set(SDLTTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
set(SDLTTF_PLUTOSVG OFF CACHE BOOL "" FORCE)
set(SDLTTF_INSTALL OFF CACHE BOOL "" FORCE)
set(SDLTTF_SAMPLE OFF CACHE BOOL "" FORCE)

FetchContent_Declare(SDL3
    URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.8/SDL3-3.4.8.tar.gz
)
FetchContent_Declare(SDL3_ttf
    URL https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-3.2.2.tar.gz
)
FetchContent_Declare(SDL3_ttf_freetype
    URL https://github.com/libsdl-org/freetype/archive/refs/heads/VER-2-13-2-SDL.tar.gz
)

FetchContent_MakeAvailable(SDL3)

# SDL3_ttf 的 vendored FreeType 需要放在其 external/freetype 目录
FetchContent_GetProperties(SDL3_ttf)
if(NOT sdl3_ttf_POPULATED)
    FetchContent_Populate(SDL3_ttf)
    FetchContent_GetProperties(SDL3_ttf_freetype)
    if(NOT sdl3_ttf_freetype_POPULATED)
        FetchContent_Populate(SDL3_ttf_freetype)
    endif()
    file(MAKE_DIRECTORY "${sdl3_ttf_SOURCE_DIR}/external/freetype")
    file(COPY "${sdl3_ttf_freetype_SOURCE_DIR}/"
         DESTINATION "${sdl3_ttf_SOURCE_DIR}/external/freetype")
endif()
add_subdirectory("${sdl3_ttf_SOURCE_DIR}" "${sdl3_ttf_BINARY_DIR}")

# ============================================================
# Lua（仓库内源码构建）
# ============================================================
add_library(lua SHARED
	lib/lua-5.5.0/lapi.c lib/lua-5.5.0/lauxlib.c lib/lua-5.5.0/lbaselib.c lib/lua-5.5.0/lcode.c
	lib/lua-5.5.0/lcorolib.c lib/lua-5.5.0/lctype.c lib/lua-5.5.0/ldblib.c lib/lua-5.5.0/ldebug.c
	lib/lua-5.5.0/ldo.c lib/lua-5.5.0/ldump.c lib/lua-5.5.0/lfunc.c lib/lua-5.5.0/lgc.c
	lib/lua-5.5.0/linit.c lib/lua-5.5.0/liolib.c lib/lua-5.5.0/llex.c lib/lua-5.5.0/lmathlib.c
	lib/lua-5.5.0/lmem.c lib/lua-5.5.0/loadlib.c lib/lua-5.5.0/lobject.c lib/lua-5.5.0/lopcodes.c
	lib/lua-5.5.0/loslib.c lib/lua-5.5.0/lparser.c lib/lua-5.5.0/lstate.c lib/lua-5.5.0/lstring.c
	lib/lua-5.5.0/lstrlib.c lib/lua-5.5.0/ltable.c lib/lua-5.5.0/ltablib.c lib/lua-5.5.0/ltm.c
	lib/lua-5.5.0/lundump.c lib/lua-5.5.0/lutf8lib.c lib/lua-5.5.0/lvm.c lib/lua-5.5.0/lzio.c
)
target_include_directories(lua PRIVATE lib/lua-5.5.0)
set_target_properties(lua PROPERTIES PREFIX "")

# ============================================================
# 游戏本体
# ============================================================
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
	src/script/lua_state.cpp src/script/lua_bridge.cpp
	logo/sdl_logo.rc
)

target_include_directories(MotaSDL PRIVATE
	src src/render terminal src/script
	lib/lua-5.5.0
)
target_link_libraries(MotaSDL SDL3::SDL3-shared SDL3_ttf::SDL3_ttf-shared lua)
target_compile_definitions(MotaSDL PRIVATE SDL3_BUILD)

# ============================================================
# 产物复制 + 裁剪（strip）到输出目录
# ============================================================
add_custom_command(TARGET MotaSDL POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"$<TARGET_FILE:SDL3::SDL3-shared>"       "$<TARGET_FILE_DIR:MotaSDL>/SDL3.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"$<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>" "$<TARGET_FILE_DIR:MotaSDL>/SDL3_ttf.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"$<TARGET_FILE:lua>"                      "$<TARGET_FILE_DIR:MotaSDL>/lua.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${MINGW_BIN}/libstdc++-6.dll"            "$<TARGET_FILE_DIR:MotaSDL>/libstdc++-6.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${MINGW_BIN}/libgcc_s_seh-1.dll"         "$<TARGET_FILE_DIR:MotaSDL>/libgcc_s_seh-1.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${MINGW_BIN}/libwinpthread-1.dll"        "$<TARGET_FILE_DIR:MotaSDL>/libwinpthread-1.dll"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${CMAKE_SOURCE_DIR}/lib/simsun.ttc"      "$<TARGET_FILE_DIR:MotaSDL>/simsun.ttc"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${CMAKE_SOURCE_DIR}/logo/logo2.ico"      "$<TARGET_FILE_DIR:MotaSDL>/logo2.ico"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/scripts"             "$<TARGET_FILE_DIR:MotaSDL>/scripts"
	# 裁剪：剥离 DLL/EXE 的调试符号与符号表
	COMMAND ${CMAKE_STRIP} "$<TARGET_FILE_DIR:MotaSDL>/SDL3.dll"
	COMMAND ${CMAKE_STRIP} "$<TARGET_FILE_DIR:MotaSDL>/SDL3_ttf.dll"
	COMMAND ${CMAKE_STRIP} "$<TARGET_FILE_DIR:MotaSDL>/lua.dll"
	COMMAND ${CMAKE_STRIP} "$<TARGET_FILE_DIR:MotaSDL>/MotaSDL.exe"
)

# Lua 脚本变更时独立同步到输出目录
# （POST_BUILD 仅在重链接时执行，纯 Lua 改动不会触发，故单独处理）
file(GLOB MAP_LUA_FILES
	"${CMAKE_SOURCE_DIR}/scripts/*.lua"
	"${CMAKE_SOURCE_DIR}/scripts/map/*.lua")
add_custom_command(
	OUTPUT "${CMAKE_BINARY_DIR}/map_synced.stamp"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/scripts" "$<TARGET_FILE_DIR:MotaSDL>/scripts"
	COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/map_synced.stamp"
	DEPENDS ${MAP_LUA_FILES}
)
add_custom_target(map_sync ALL DEPENDS "${CMAKE_BINARY_DIR}/map_synced.stamp")
