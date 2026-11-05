CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++11 -Iinclude
LDFLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl

TARGET = neon_night_street
SRCS = glad.c src/main.cpp src/Shader.cpp src/Mesh.cpp src/Scene.cpp src/Camera.cpp
OBJS = $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SRCS)))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o src/*.o

.PHONY: all clean
