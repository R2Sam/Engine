cmake_minimum_required(VERSION 3.25)
project(MyProject CXX)

# -------------------------
# C++ standard & output
# -------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# -------------------------
# Platform detection
# -------------------------
if (WIN32)
    message(STATUS "Building on Windows")

    set(COMMON_FLAGS "-Wall -Wextra -pedantic -fopenmp -fmax-errors=5 -Wno-missing-field-initializers -Wno-narrowing -Wno-enum-compare -Wno-reorder -Wno-shadow -Wno-deprecated-declarations -O3")
    set(LINK_FLAGS "-s -mwindows")

    set(SYSTEM_LIBS opengl32 gdi32 winmm ws2_32 gomp -static)

    set(RAYLIB_LIB ${LIB_DIR}/Raylib/libraylib.a)
    set(RLIMGUI_LIB ${LIB_DIR}/rlImGui/librlImGui.a)
    set(LUAJIT_LIB ${LIB_DIR}/Lua/libluajit.a)

elseif(UNIX AND NOT APPLE AND NOT CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
    message(STATUS "Building on Linux")

    set(COMMON_FLAGS "-Wall -Wextra -pedantic -fopenmp -fmax-errors=5 -Wno-missing-field-initializers -Wno-narrowing -Wno-enum-compare -Wno-reorder -Wno-shadow -Wno-deprecated-declarations -O3")
    set(LINK_FLAGS "-s")

    set(SYSTEM_LIBS GL m pthread dl rt X11 gomp)

    set(RAYLIB_LIB ${LIB_DIR}/Raylib/libraylib-Linux.a)
    set(RLIMGUI_LIB ${LIB_DIR}/rlImGui/librlImGui-Linux.a)
    set(LUAJIT_LIB ${LIB_DIR}/Lua/libluajit-Linux.a)

elseif(UNIX AND NOT APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
    message(FATAL_ERROR "Building on Linux arm not supported")

else()
    message(FATAL_ERROR "Unsupported OS")
endif()

# -------------------------
# Compiler flags
# -------------------------
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${LINK_FLAGS}")

# -------------------------
# Imported libraries (release only)
# -------------------------
add_library(Engine::raylib STATIC IMPORTED)
set_target_properties(Engine::raylib PROPERTIES IMPORTED_LOCATION ${RAYLIB_LIB})

add_library(Engine::rlImGui STATIC IMPORTED)
set_target_properties(Engine::rlImGui PROPERTIES IMPORTED_LOCATION ${RLIMGUI_LIB})

add_library(Engine::luajit STATIC IMPORTED)
set_target_properties(Engine::luajit PROPERTIES IMPORTED_LOCATION ${LUAJIT_LIB})

# -------------------------
# Fetched libraries
# -------------------------
include(FetchContent)

FetchContent_Declare(networking GIT_REPOSITORY https://github.com/R2Sam/Networking.git GIT_TAG lib)
FetchContent_MakeAvailable(networking)

# -------------------------
# Object library for sources
# -------------------------
file(GLOB_RECURSE SOURCES src/**/*.cpp)
list(FILTER SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

add_library(EngineObjects OBJECT ${SOURCES})
target_include_directories(EngineObjects PUBLIC include src)

# -------------------------
# Static library for engine
# -------------------------
add_library(engine STATIC $<TARGET_OBJECTS:EngineObjects>)
target_include_directories(engine PUBLIC include src)
target_link_libraries(engine PUBLIC Engine::raylib Engine::rlImGui Engine::luajit networking ${SYSTEM_LIBS})