# Makefile for C++ project with native and WebAssembly compilation

# Compiler and flags
CXX := g++
EMXX := emcc
CXXFLAGS := -std=c++11 -lSDL2 -lSDL2_image -lfmt -Wall -Wextra
EMXXFLAGS := -std=c++11 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -lfmt --preload-file assets --preload-file maps

# Include and library directories
SRC_DIR := src
INCLUDE_DIR := third-party
LIB_DIR := lib
LIB_DIR_WASM := lib_wasm
OBJ_DIR := obj

# Update flags to include subdirectories
CXXFLAGS += -I$(INCLUDE_DIR) -L$(LIB_DIR) -I/opt/local/include -L/opt/local/lib
EMXXFLAGS += -I$(INCLUDE_DIR) -L$(LIB_DIR_WASM)

# Source files and objects
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# output
NATIVE_TARGET := openbound
WASM_TARGET := openbound.html

# Default target
all: native

# Native compilation
native: $(NATIVE_TARGET)

$(NATIVE_TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# WebAssembly compilation
wasm: $(WASM_TARGET)

$(WASM_TARGET): $(SRCS)
	$(EMXX) $(EMXXFLAGS) $^ -o $@

# Object file compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR)
	rm -f $(NATIVE_TARGET) $(WASM_TARGET) *.wasm *.html

# Include dependency files
-include $(DEPS)

# Phony targets
.PHONY: all native wasm clean
