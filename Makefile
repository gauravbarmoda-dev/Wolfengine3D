PLATFORM ?= pc
TARGET = game

SRC = wolf_engine/src/Camera.cpp \
      wolf_engine/src/Engine.cpp \
      wolf_engine/src/Input.cpp \
      wolf_engine/src/Map.cpp \
      wolf_engine/src/Rasterizer.cpp \
      wolf_engine/src/Raycaster.cpp

ifeq ($(PLATFORM), rg35xx)
    # RG35XX Plus Setup
    CXX = aarch64-linux-gnu-g++
    CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -ffast-math -flto -mcpu=cortex-a53 -mtune=cortex-a53 -fopenmp
    SRC += rg35xx_demo/src/main.cpp
else
    # PC Setup (Default)
    CXX = g++
    CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -ffast-math -flto -fopenmp
    SRC += pc_demo/src/main.cpp
endif

LDFLAGS = -lSDL2 -fopenmp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	OMP_WAIT_POLICY=PASSIVE OMP_NUM_THREADS=4 ./$(TARGET)

.PHONY: all clean run
