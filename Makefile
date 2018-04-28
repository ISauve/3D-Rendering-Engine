# Makefile for OpenGL Practice
# Created Feb 8, 2018

BUILD_DIR = obj
SRC_DIR = src
BIN_DIR = bin

CXX = g++ -std=c++14
CXXFLAGS = -Wall -O -g -MMD    # -MMD generates dependencies
LIBFLAGS = -Ibuild/include lib/glad.c -lGLFW -lAssimp -lsfml-graphics -framework OpenGL -framework CoreFoundation

# compile all .cpp files in source directory & it's subdirectories
SOURCES = $(wildcard $(SRC_DIR)/*.cpp  $(SRC_DIR)/*/*.cpp)

# .o files depend upon .cpp files with same names
OBJECTS = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SOURCES:.cpp=.o))

# .d file is list of dependencies for corresponding .cpp file
DEPS = ${OBJECTS:.o=.d}

TARGET = $(BIN_DIR)/opengl

### Set default make
.PHONY: default
default: $(TARGET)

### Link
# Note that the LIBFLAGS must come last
$(TARGET): $(OBJECTS)
	@[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LIBFLAGS)

### Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(CXX)  -c -o $@ $< $(CXXFLAGS)

-include ${DEPS}

### Clean target
clean:
	rm -rf $(BUILD_DIR)

### Run target
run:
	@$(MAKE) && ./$(TARGET)