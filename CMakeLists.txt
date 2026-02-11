cmake_minimum_required(VERSION 3.25)

project(Engine
  VERSION 1.0.0
  LANGUAGES CXX
)

# ------------------------------------------------------------------------------
# C++ standard & global settings
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# ------------------------------------------------------------------------------
# Platform detection
# ------------------------------------------------------------------------------
if (WIN32)
  set(SYSTEM_LIBS
    opengl32
    gdi32
    winmm
    ws2_32
    gomp
    -static
  )

  set(RLIMGUI_LIB ${LIB_DIR}/rlImGui/librlImGui.a)
  set(LUAJIT_LIB  ${LIB_DIR}/Lua/libluajit.a)

  set(RELEASE_FLAGS        -O3)
  set(DEBUG_FLAGS          -Og -gdwarf-4)
  set(RELEASE_LINKER_FLAGS -s -mwindows)

elseif (UNIX AND NOT APPLE AND NOT CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
  set(SYSTEM_LIBS
    GL
    m
    pthread
    dl
    rt
    X11
    gomp
  )

  set(RLIMGUI_LIB ${LIB_DIR}/rlImGui/librlImGui-Linux.a)
  set(LUAJIT_LIB  ${LIB_DIR}/Lua/libluajit-Linux.a)

  set(RELEASE_FLAGS        -O3)
  set(DEBUG_FLAGS          -Og -gdwarf-4 -fno-omit-frame-pointer)
  set(RELEASE_LINKER_FLAGS -s)

elseif (UNIX AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
  message(FATAL_ERROR "Linux ARM is not supported")

else()
  message(FATAL_ERROR "Unsupported platform")
endif()

# ------------------------------------------------------------------------------
# Common compiler warnings & flags
# ------------------------------------------------------------------------------
set(COMMON_FLAGS
  -Wall
  -Wextra
  -pedantic
  -fopenmp
  -fmax-errors=5
  -Wno-missing-field-initializers
  -Wno-narrowing
  -Wno-enum-compare
  -Wno-reorder
  -Wno-shadow
  -Wno-deprecated-declarations
)

# ------------------------------------------------------------------------------
# Imported third-party libraries
# ------------------------------------------------------------------------------

add_library(rlImGui STATIC IMPORTED)
set_target_properties(rlImGui PROPERTIES
  IMPORTED_LOCATION ${RLIMGUI_LIB}
)

add_library(luajit STATIC IMPORTED)
set_target_properties(luajit PROPERTIES
  IMPORTED_LOCATION ${LUAJIT_LIB}
)

# ------------------------------------------------------------------------------
# FetchContent dependencies
# ------------------------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET OFF)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../Networking/.git")
  set(NETWORKING_REPO file://${CMAKE_CURRENT_SOURCE_DIR}/../Networking/.git)
  message(STATUS "Using local Networking repo")
elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Networking/.git")
  set(NETWORKING_REPO file://${CMAKE_CURRENT_SOURCE_DIR}/../../../../Networking/.git)
  message(STATUS "Using local Networking repo")
else()
  set(NETWORKING_REPO https://github.com/R2Sam/Networking.git)
  message(STATUS "Using remote Networking repo")
endif()

FetchContent_Declare(networking
  GIT_REPOSITORY ${NETWORKING_REPO}
  GIT_TAG        lib
	GIT_SHALLOW TRUE
  EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(networking)

FetchContent_Declare(raylib
  GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
  GIT_TAG 5.5
  GIT_SHALLOW TRUE
  EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(raylib)

# ------------------------------
# Formatting
# ------------------------------
file(GLOB_RECURSE FILES CONFIGURE_DEPENDS
  src/*.cpp
  src/*.h
)

add_custom_target(
    clang-format
    COMMAND clang-format --dry-run --Werror ${FILES} || {echo "ERROR: Some files in src/ are not properly formatted."; echo "Differences can be seen with 'clang-format <file> | diff -u <file> -'."; echo "Run 'clang-format -i <file>' to fix errors"}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Checking code style with clang-format..."
)

add_custom_target(
    clang-tidy
    COMMAND run-clang-tidy -quiet -p build ${FILES} || {echo "ERROR: Some files in src/ are not properly formatted or have errors."; echo "Run 'clang-tidy -fix --format-style=file <file>' to fix errors"}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Checking code style with clang-tidy..."
)

# ------------------------------------------------------------------------------
# Engine static library
# ------------------------------------------------------------------------------
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
  src/*.cpp
)

list(FILTER SRC EXCLUDE REGEX ".*/main\\.cpp$")

add_library(engine STATIC ${SRC})

target_include_directories(engine
  PUBLIC
    include
    src
)

add_dependencies(engine clang-format clang-tidy)

target_compile_options(engine PRIVATE
  ${COMMON_FLAGS}
  $<$<CONFIG:Debug>:${DEBUG_FLAGS}>
  $<$<CONFIG:Release>:${RELEASE_FLAGS}>
)

target_link_libraries(engine
  PUBLIC
    raylib
    rlImGui
    luajit
    networking
    ${SYSTEM_LIBS}
)

target_link_options(engine PRIVATE
  $<$<CONFIG:Release>:${RELEASE_LINKER_FLAGS}>
)

if (EMSCRIPTEN)
  target_compile_options(engine PRIVATE
    -s USE_PTHREADS
  )

  target_link_options(engine PRIVATE
    -s ASYNCIFY
    -s PROXY_TO_PTHREAD
  )
endif()