CXX ?= g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++11 -Iinclude

UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)

ifeq ($(UNAME_S), Linux)
    LDFLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
    TARGET = neon_night_street
else ifeq ($(UNAME_S), Darwin)
    LDFLAGS = -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    TARGET = neon_night_street
else
    # Windows (MinGW / MSYS2 / Git Bash)
    LDFLAGS = -Llib -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -lpthread
    TARGET = neon_night_street.exe
endif

SRCS = glad.c \
       src/main.cpp \
       src/Shader.cpp \
       src/Mesh.cpp \
       src/Scene.cpp \
       src/Camera.cpp \
       src/Texture.cpp \
       src/Model.cpp \
       src/CityBuilder.cpp \
       src/AlleyBuilder.cpp \
       src/ShopBuilder.cpp \
       src/HotelBuilder.cpp \
       src/StreetProps.cpp \
       src/DecalBuilder.cpp \
       src/Audio.cpp \
       src/Rain.cpp \
       src/Cinematic.cpp

OBJS = $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SRCS)))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) neon_night_street neon_night_street.exe *.o src/*.o

.PHONY: all clean
