CC = gcc
CXX = g++

INCLUDES = -I$(glfw_inc) -I$(glad_inc)
LIBRARIES = -L$(glfw_lib) ./libs/SOIL/libSoil.a -L./build

glfw = ./libs/glfw
glfw_inc = $(glfw)/include
glfw_lib = $(glfw)/lib-mingw-w64

glad = ./libs
glad_inc = $(glad)

CFLAGS = -Wall -ggdb -O3 $(INCLUDES)
CXXFLAGS = -Wall -ggdb -O3 $(INCLUDES)
LDFLAGS = $(LIBRARIES) -lglfw3 -lopengl32 -lglu32 -lgdi32

TARGET = main.exe
cpp_files = src/main.cpp
objects = $(cpp_files:.cpp=.o) src/glad.o
OBJDIR = obj
BUILDDIR = build

compile: clean
	mkdir obj
	mkdir build
	$(CC) $(CXXFLAGS)  -c libs/glad/glad.c -o obj/glad.o	
	$(CXX) $(CXXFLAGS)  -c src/game.cpp -g -o obj/game.o
	$(CXX) -shared -L./libs/glfw/lib-mingw-w64 ./libs/SOIL/libSoil.a -L./build -o build/game.dll obj/game.o obj/glad.o -Wl,--out-implib,libgame.a -lglfw3 -lopengl32 -lglu32 -lgdi32
	$(CXX) $(CXXLAGS) $(INCLUDES) -c src/main.cpp -g -o obj/main.o
	$(CXX) -o $(BUILDDIR)/$(TARGET) obj/main.o obj/glad.o $(LIBRARIES) $(LDFLAGS) build/game.dll

clean :
ifneq ($(wildcard $(OBJDIR)),)
	rm -r $(OBJDIR) 
endif
ifneq ($(wildcard $(BUILDDIR)),)
	rm -r $(BUILDDIR)
endif

all: $(TARGET)

$(TARGET): $(objects)
		$(CXX) -o $@ $^ $(LDFLAGS)

run:
	./build/main.exe

