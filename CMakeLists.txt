cmake_minimum_required(VERSION 3.25)

project(
	Engine
	VERSION 0.0.1
	LANGUAGES CXX
)

# -------------------------
# Global configuration
# -------------------------
include(FetchContent)

set(FETCHCONTENT_QUIET ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Detect Emscripten
set(IS_EMSCRIPTEN OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
	set(IS_EMSCRIPTEN ON)
endif()

# -------------------------
# Options
# -------------------------
add_library(Engine_options INTERFACE)
add_library(Engine::options ALIAS Engine_options)

target_compile_features(
	Engine_options
	INTERFACE
		cxx_std_23
)

# -------------------------
# Safety
# -------------------------
add_library(Engine_safety INTERFACE)
add_library(Engine::safety ALIAS Engine_safety)

target_compile_options(
	Engine_safety
	INTERFACE
		-fstack-protector-strong
		-D_GLIBCXX_ASSERTIONS

		$<$<CONFIG:Debug>:
			-O0
			-g3
			-gdwarf-4
			-fno-inline
			-fno-omit-frame-pointer

			$<$<AND:$<PLATFORM_ID:Linux>,$<NOT:$<BOOL:${IS_EMSCRIPTEN}>>>:
                -fsanitize=address
                -fsanitize=undefined
            >
		>

		$<$<CONFIG:Release>:
            -O3

            $<$<AND:$<PLATFORM_ID:Linux>,$<NOT:$<BOOL:${IS_EMSCRIPTEN}>>>:
                -fsanitize-trap=undefined
            >
        >
)

target_link_options(
	Engine_safety
	INTERFACE
		$<$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Linux>,$<NOT:$<BOOL:${IS_EMSCRIPTEN}>>>:
			-fsanitize=address
			-fsanitize=undefined
		>

		$<$<AND:$<CONFIG:Release>,$<PLATFORM_ID:Linux>,$<NOT:$<BOOL:${IS_EMSCRIPTEN}>>>:
		>
)

find_package(OpenGL REQUIRED)

# -------------------------
# Platform
# -------------------------
add_library(Engine_platform INTERFACE)
add_library(Engine::platform ALIAS Engine_platform)

target_link_libraries(
	Engine_platform
	INTERFACE
		$<$<PLATFORM_ID:Windows>:
			opengl32
			gdi32
			winmm
			ws2_32
		>
)

target_link_options(
	Engine_platform
	INTERFACE
		$<$<PLATFORM_ID:Windows>:
			-static
		>
)

target_link_libraries(
	Engine_platform
	INTERFACE
		$<$<AND:$<PLATFORM_ID:Linux>,$<NOT:$<BOOL:${IS_EMSCRIPTEN}>>>:
			m
			pthread
			dl
			rt
			X11
		>
)

target_link_libraries(
    Engine_platform
    INTERFACE
        OpenGL::GL
)

target_compile_options(
    Engine_platform
    INTERFACE
        $<$<BOOL:${IS_EMSCRIPTEN}>:
            -s USE_GLFW=3
            -s FULL_ES3=1
        >
)

target_link_options(
    Engine_platform
    INTERFACE
        $<$<BOOL:${IS_EMSCRIPTEN}>:
            -s USE_GLFW=3
            -s FULL_ES3=1
            -s ASYNCIFY
            -s PROXY_TO_PTHREAD
        >
)

# -------------------------
# raylib
# -------------------------
include(FetchContent)

FetchContent_Declare(
	raylib
	GIT_REPOSITORY https://github.com/raysan5/raylib.git
	GIT_TAG 5.5
	GIT_SHALLOW TRUE
	EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(raylib)

# -------------------------
# Imported libraries
# -------------------------
add_library(rlImGui STATIC IMPORTED)

add_library(luajit STATIC IMPORTED)

set_target_properties(rlImGui PROPERTIES
IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/rlImGui/librlImGui-Linux.a
)

set_target_properties(luajit PROPERTIES
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/Lua/libluajit-Linux.a
)

if (WIN32)

	set_target_properties(rlImGui PROPERTIES
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/rlImGui/librlImGui.a
	)

	set_target_properties(luajit PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/Lua/libluajit.a
	)

elseif (${IS_EMSCRIPTEN})

	set_target_properties(rlImGui PROPERTIES
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/rlImGui/librlImGui-Web.a
	)

	set_target_properties(luajit PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/Lua/libluajit-Web.a
	)

endif()

# -------------------------
# Dependencies
# -------------------------

if(EXISTS "/home/sam/Documents/Projects/Networking")
	message(STATUS "Using local Networking repo")

	FetchContent_Declare(
		networking
		SOURCE_DIR /home/sam/Documents/Projects/Networking/
	)

else()
	message(STATUS "Using online Networking repo")

	FetchContent_Declare(
		networking
		GIT_REPOSITORY https://github.com/R2Sam/Networking.git
		GIT_TAG main
		GIT_SHALLOW TRUE
		EXCLUDE_FROM_ALL
	)
endif()

FetchContent_MakeAvailable(networking)

# -------------------------
# Engine static library
# -------------------------
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.cpp)
list(FILTER SRC EXCLUDE REGEX ".*/main\\.cpp$")

add_library(engine STATIC ${SRC})

target_include_directories(
	engine
	PUBLIC
		include
		src
)

target_compile_options(
    engine
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fmax-errors=5

        -Wno-missing-field-initializers
        -Wno-narrowing
        -Wno-enum-compare
        -Wno-reorder
        -Wno-shadow
        -Wno-deprecated-declarations
)

target_link_libraries(
	engine
	PUBLIC
		Engine::options
		Engine::safety
		Engine::platform

		raylib
		rlImGui
		luajit
		networking
)