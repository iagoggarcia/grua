CXX = g++
CC = gcc

CXXFLAGS = -Wall -std=c++11 -Iinclude
CFLAGS = -Wall -Iinclude

LDFLAGS = -lglfw -lGL -ldl -lm

SRC_DIR = src
OBJ_DIR = obj

TARGET = grua

CPP_SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/cubo.cpp $(SRC_DIR)/grua.cpp $(SRC_DIR)/esfera.cpp
C_SOURCES = $(SRC_DIR)/glad.c

CPP_OBJECTS = $(CPP_SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
C_OBJECTS = $(C_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

OBJECTS = $(CPP_OBJECTS) $(C_OBJECTS)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)