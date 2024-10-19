# detect platform (for native)
ifeq ($(OS),Windows_NT)
    PLATFORM := windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM := linux
    endif
    ifeq ($(UNAME_S),Darwin)
        PLATFORM := mac
    endif
endif
LIB_PATH := lib/$(PLATFORM)

# compilers and flags
CXX := g++
EMXX := emcc
CXXFLAGS := -g -std=c++11 -O2 -Wall -Wextra $(shell sdl2-config --cflags) $(shell pkg-config --cflags SDL2_image) -Ithird-party
EMXXFLAGS := -std=c++11 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' --preload-file assets --preload-file maps -Ithird-party -lfmt -Llib/wasm
LDFLAGS := $(shell sdl2-config --libs) $(shell pkg-config --libs SDL2_image) -lfmt -L$(LIB_PATH)

# sources and objects
SRC_DIR := src
OBJ_DIR := obj
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# output
NATIVE_TARGET := openbound
WASM_TARGET := openbound.html

# default target
all: native

# native compilation
native: $(NATIVE_TARGET)

$(NATIVE_TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# webassembly compilation
wasm: $(WASM_TARGET)

$(WASM_TARGET): $(SRCS)
	$(EMXX) $(EMXXFLAGS) $^ -o $@

# object file compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# cleanup
clean:
	rm -rf $(OBJ_DIR)
	rm -f $(NATIVE_TARGET) $(WASM_TARGET) openbound.data openbound.js openbound.wasm

-include $(DEPS)

.PHONY: all native wasm clean
