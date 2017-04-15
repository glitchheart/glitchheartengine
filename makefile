CC = gcc
CXX = g++

INCLUDES = -I$(glfw_inc) -I$(glad_inc)  -I$(al_inc)
LIBRARIES = -L$(glfw_lib) -L$(al_lib) ./libs/SOIL/libSoil.a -L./build

glfw = ./libs/glfw
glfw_inc = $(glfw)/include
glfw_lib = $(glfw)/lib-mingw-w64

glad = ./libs
glad_inc = $(glad)

al = ./libs/openal
al_inc = $(al)/include
al_lib = $(al)/libs/Win64/

CFLAGS = -Wall -ggdb -O3 $(INCLUDES)
CXXFLAGS = -Wall -ggdb -O3 $(INCLUDES)
LDFLAGS = $(LIBRARIES) -lglfw3 -lopengl32 -lglu32 -lgdi32 -lopenal32 build/OpenAL32.dll

TARGET = main.exe
cpp_files = src/main.cpp
objects = $(cpp_files:.cpp=.o) src/glad.o
OBJDIR = obj
BUILDDIR = build

compile: clean
	mkdir obj
ifeq ($(wildcard $(BUILDDIR)),)
	mkdir build
endif
	$(CC) $(CXXFLAGS)  -c libs/glad/glad.c -o obj/glad.o	
	$(CXX) $(CXXFLAGS)  -c src/game.cpp -g -o obj/game.o
	$(CXX) -shared -o build/game.dll obj/game.o obj/glad.o -Wl,--out-implib,libgame.a $(LDFLAGS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c src/main.cpp -g -o obj/main.o
	$(CXX) -o $(BUILDDIR)/$(TARGET) obj/main.o obj/glad.o $(LIBRARIES) $(LDFLAGS) build/game.dll 

clean :
ifneq ($(wildcard $(OBJDIR)),)
	rm -r $(OBJDIR) 
endif

all: $(TARGET)

$(TARGET): $(objects)
		$(CXX) -o $@ $^ $(LDFLAGS)

run:
	./build/main.exe

