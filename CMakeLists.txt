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
  set(DEBUG_FLAGS          -O0 -g3 -gdwarf-4 -fno-inline -fno-omit-frame-pointer)
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
  set(DEBUG_FLAGS          -Og -g3 -gdwarf-4 -fno-omit-frame-pointer)
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

set(FETCHCONTENT_QUIET ON)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../Networking")
    message(STATUS "Using local Networking repo")
    FetchContent_Declare(
        networking
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Networking/
    )
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../Networking")
  message(STATUS "Using local Networking repo")
    FetchContent_Declare(
        networking
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../Networking/
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

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../Utils")
    message(STATUS "Using local Utils repo")
    FetchContent_Declare(
        utils
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Utils/
    )
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../Utils")
  message(STATUS "Using local Utils repo")
    FetchContent_Declare(
        utils
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../Utils/
    )
else()
    message(STATUS "Using online Utils repo")
    FetchContent_Declare(
        utils
        GIT_REPOSITORY https://github.com/R2Sam/Utils.git
        GIT_TAG main
        GIT_SHALLOW TRUE
        EXCLUDE_FROM_ALL
    )
endif()

FetchContent_MakeAvailable(utils)

FetchContent_Declare(raylib
  GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
  GIT_TAG 5.5
  GIT_SHALLOW TRUE
  EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(raylib)

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

target_compile_options(engine PRIVATE
  ${COMMON_FLAGS}
  $<$<CONFIG:Debug>:${DEBUG_FLAGS}>
  $<$<CONFIG:Release>:${RELEASE_FLAGS}>
  $<$<CONFIG:Debug>:
        $<$<PLATFORM_ID:Linux>:
        -fsanitize=address
        -fsanitize=undefined>
    >
)

target_link_libraries(engine
  PUBLIC
    raylib
    rlImGui
    luajit
    networking
    utils
    ${SYSTEM_LIBS}
)

target_link_options(engine PRIVATE
  $<$<CONFIG:Release>:${RELEASE_LINKER_FLAGS}>
  $<$<CONFIG:Debug>:
        $<$<PLATFORM_ID:Linux>:
        -fsanitize=address
        -fsanitize=undefined>
    >
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