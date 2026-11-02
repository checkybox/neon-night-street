CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++11 -Iinclude
LDFLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl

TARGET = neon_night_street

all:
	@echo "Repository initialized with C++11 and OpenGL 3.3 Core profile."
	@echo "Core executable targets will be linked in upcoming milestone."

clean:
	rm -f $(TARGET) *.o src/*.o

.PHONY: all clean
