-include .env

PLATFORM ?= pc
TARGET = game

SRC = wolf_engine/src/Camera.cpp \
      wolf_engine/src/Engine.cpp \
      wolf_engine/src/Input.cpp \
      wolf_engine/src/Map.cpp \
      wolf_engine/src/Rasterizer.cpp \
      wolf_engine/src/Raycaster.cpp \
      wolf_engine/src/Palette.cpp \
      wolf_engine/src/AssetMgr.cpp \
      src/main.cpp \
      src/Player.cpp

ifeq ($(PLATFORM), rg35xx)
    # RG35XX Plus Setup
    CXX = aarch64-linux-gnu-g++
    CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -ffast-math -flto -mcpu=cortex-a53 -mtune=cortex-a53 -fopenmp
else
    # PC Setup (Default)
    CXX = g++
    CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -ffast-math -flto -fopenmp
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
	OMP_WAIT_POLICY=PASSIVE OMP_NUM_THREADS=4 OMP_STACKSIZE=512k ./$(TARGET)

deploy: $(TARGET)
ifndef DEVICE_IP
	$(error DEVICE_IP is not set. Please copy .env.example to .env and set your device IP)
endif
	ssh root@$(DEVICE_IP) "mkdir -p /userdata/roms/ports/wolf_game/assets"
	scp game root@$(DEVICE_IP):/userdata/roms/ports/wolf_game/game
	scp -r assets/* root@$(DEVICE_IP):/userdata/roms/ports/wolf_game/assets/

.PHONY: all clean run deploy
