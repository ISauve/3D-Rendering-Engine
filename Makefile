# Makefile for OpenGL Practice
# Created Feb 8, 2018

BUILD_DIR = obj
SRC_DIR = src
BIN_DIR = bin

CXX = g++ -std=c++14
CXXFLAGS = -Wall -O -g
LIBFLAGS = -Ibuild/include src/glad.c -lGLFW -framework OpenGL

# compile all .cpp files in source directory & it's subdirectories
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# .o files depend upon .cpp files with same names
OBJECTS = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SOURCES:.cpp=.o))

TARGET = $(BIN_DIR)/opengl

### Set default make
.PHONY: default
default: $(TARGET)

### Link
$(TARGET): $(OBJECTS)
	@[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LIBFLAGS)

### Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(CXX)  -c -o $@ $< $(CXXFLAGS)

### Clean target
clean:
	rm -rf $(BUILD_DIR)

### Run target
run:
	@$(MAKE) && ./$(TARGET)