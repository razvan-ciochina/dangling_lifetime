cmake_minimum_required (VERSION 3.20)
project (dangling_lifetime)
set(CMAKE_CXX_STANDARD 17)

if (NOT BACKTRACES)
	set(INFO_MESSAGE "To enable backtraces please use -DBACKTRACES=ON, and set build type to Debug")
	execute_process(COMMAND 
				${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1
				${CMAKE_COMMAND} -E cmake_echo_color --green --bold ${INFO_MESSAGE})
endif()

if (BACKTRACES)
	include(FetchContent)
	FetchContent_Declare(backward
			GIT_REPOSITORY https://github.com/bombela/backward-cpp)
	FetchContent_MakeAvailable(backward)
	if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(INFO_MESSAGE "IMPORTANT: You really should build the debug version for nice backtraces (cmake -DCMAKE_BUILD_TYPE=Debug ..)")
		execute_process(COMMAND 
			${CAMKE_COMMAND} -E env CLICOLOR_FORCE=1
			${CMAKE_COMMAND} -E cmake_echo_color --red --bold "${INFO_MESSAGE}")
	endif()
	file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
	add_executable(dangling_lifetime dangling_lifetime.cpp ${BACKWARD_ENABLE})
	target_compile_options(dangling_lifetime PUBLIC -DBACKTRACES)
	add_backward(dangling_lifetime)
else()
	add_executable(dangling_lifetime dangling_lifetime.cpp)
endif()


