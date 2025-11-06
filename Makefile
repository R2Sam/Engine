UNAME := $(shell uname -s)
ARCH := $(shell uname -m)

# Variables
NAME = main
SRC_DIR = src
OBJ_DIR = src
BIN_DIR = bin
DATA_DIR = data
INCLUDE_DIR = include
LIB_DIR = lib

# Source and Object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Common compiler flags
COMMON_FLAGS = -std=c++23 -fmax-errors=5 -Wall -Wextra -pedantic -fopenmp -MMD -MP -I$(INCLUDE_DIR) -I$(SRC_DIR)
# COMMON_FLAGS += -Werror
COMMON_FLAGS += -Wno-missing-field-initializers -Wno-narrowing -Wno-enum-compare -Wno-reorder -Wno-shadow -Wno-deprecated-declarations

# Debug and Release flags
DEBUG_FLAGS = -Og -gdwarf-4

ifeq ($(OS),Windows_NT)
	SHELL := cmd
	CXX := g++
	RELEASE_FLAGS = -O3 -s -mwindows
	LIBS := 
	LIBS += $(LIB_DIR)/Raylib/libraylib.a -lopengl32 -lgdi32 -lwinmm  $(LIB_DIR)/Lua/libluajit.a -lgomp $(LIB_DIR)/rlImGui/librlImGui.a -lws2_32 -lwinmm
else ifeq ($(ARCH),aarch64)
	$(error Pi build not complete)
	SHELL := /bin/bash
	CXX := g++
	RELEASE_FLAGS := -O3 -s
	LIBS := -lpthread
else ifeq ($(UNAME),Linux)
	SHELL := /bin/bash
	CXX := g++
	RELEASE_FLAGS := -O3 -s
	LIBS := 
	LIBS += $(LIB_DIR)/Raylib/libraylib-Linux.a  -lGL -lm -lpthread -ldl -lrt -lX11 $(LIB_DIR)/Lua/libluajit-Linux.a -lgomp $(LIB_DIR)/rlImGui/librlImGui-Linux.a
else ifeq ($(UNAME),Darwin)
	$(error Mac not supported)
else
    $(error Unknown operating system: $(OS))
endif

# Rules

debug: FLAGS = $(DEBUG_FLAGS)
debug: $(OBJS)
	$(CXX) $(OBJS) -o $(BIN_DIR)/main $(LIBS)

release: FLAGS = $(RELEASE_FLAGS)
release: $(OBJS)
	$(CXX) $(OBJS) -o $(BIN_DIR)/$(NAME) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(COMMON_FLAGS) $(FLAGS) -c $< -o $@

-include $(OBJS:.o=.d)

ifeq ($(UNAME),Linux)
run:
	kitty --hold bash -c "cd  $(BIN_DIR) && ./main"

gdb:
	kitty --hold bash -c "cd  $(BIN_DIR) && gdb main"

clean:
	find "$(OBJ_DIR)" -name "*.o" -type f -delete && find "$(OBJ_DIR)" -name "*.d" -type f -delete

all: clean debug

else
run:
	cmd /c start cmd /k "cd $(BIN_DIR) && main.exe"

gdb:
	cmd /c start cmd /k "cd $(BIN_DIR) && gdb main.exe"

clean:
	del /S /Q "$(OBJ_DIR)\*.o" && del /S /Q "$(OBJ_DIR)\*.d"

Cmd:
	cmd /c start cmd

all: clean | debug

endif