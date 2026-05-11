cmake_minimum_required(VERSION 3.28.1)

project(MagicalTower)

aux_source_directory(./src SRC_LIST)
aux_source_directory(./src/event SRC_LIST)

include_directories(./inc)
link_directories(./lib)

add_executable(${PROJECT_NAME} ${SRC_LIST} ./logo/logo.rc)

target_link_libraries(${PROJECT_NAME} ncursesw)

# 构建后将 DLL 复制到可执行文件目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${CMAKE_SOURCE_DIR}/lib/ncursesw.dll"
	"$<TARGET_FILE_DIR:${PROJECT_NAME}>/ncursesw.dll"
)

add_definitions(-D_XOPEN_SOURCE_EXTENDED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=GBK")
