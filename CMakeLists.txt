cmake_minimum_required(VERSION 3.16)

project(AConn)
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
message(STATUS "Platform : ${CMAKE_SYSTEM_NAME}")

SET(CMAKE_BUILD_TYPE "Release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
include_directories(include)

# build and link portaudio (static)
add_subdirectory(thirdparty/portaudio)
link_libraries(portaudio_static)

if(WIN32)
	link_libraries(ws2_32)
endif()

# add executable files for every file in /src
aux_source_directory(src SOURCES)
foreach(file IN LISTS SOURCES)
	string(REGEX MATCH "[^/]*\\." program ${file})
	# Why "\\." instead of "\." works? (for literal '.')
	string(REGEX REPLACE "\\." "" program ${program})
	add_executable(${program} ${file})
endforeach()