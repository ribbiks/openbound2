# compilers and flags
CXX := g++
EMXX := emcc
CXXFLAGS := -std=c++11 -lSDL2 -lSDL2_image -lfmt -Wall -Wextra
EMXXFLAGS := -std=c++11 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -lfmt --preload-file assets --preload-file maps

# include and library directories
SRC_DIR := src
OBJ_DIR := obj

CXXFLAGS += -Ithird-party -Llib -I/opt/local/include -L/opt/local/lib
EMXXFLAGS += -Ithird-party -Llib_wasm

# sources and objects
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
	$(CXX) $(CXXFLAGS) $^ -o $@

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
