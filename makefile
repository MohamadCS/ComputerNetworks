CXX= g++
CFLAGS= -g -std=c++23
INCLUDE_DIR=./include
SRC_DIR = ./src
BUILD_DIR = ./build
TARGET= prog

CPP_SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
SOURCES = $(CPP_SOURCES)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_SOURCES))
LINK = -I$(INCLUDE_DIR)

all: clean $(TARGET)

leaks: $(TARGET) 
	leaks -atExit -- $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) $(LINK) $(OBJECTS)  -o $@ $

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(LINK) -c $< -o $@ 

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/$(TARGET)


