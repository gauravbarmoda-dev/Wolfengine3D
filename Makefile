CXX = aarch64-linux-gnu-g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lSDL2

SRC = wolf_engine/src/Camera.cpp \
      wolf_engine/src/Engine.cpp \
      wolf_engine/src/Input.cpp \
      wolf_engine/src/Map.cpp \
      wolf_engine/src/Rasterizer.cpp \
      wolf_engine/src/Raycaster.cpp \
      rg35xx_demo/src/main.cpp

OBJ = $(SRC:.cpp=.o)
TARGET = rg35xx_game

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
