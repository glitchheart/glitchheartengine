CC = gcc
CXX = g++


LIBDIR = ./libs
INCLUDES = -I$(glfw_inc) -I$(glad_inc) -I$(al_inc) -I$(freetype_inc)
LIBRARIES = -L$(glfw_lib) -L$(al_lib) $(LIBDIR)/SOIL/libSoil.a $(LIBDIR)/FreeType/lib/freetype.a 

glfw = $(LIBDIR)/glfw
glfw_inc = $(glfw)/include
glfw_lib = $(glfw)/lib-mingw-w64

glad = $(LIBDIR)
glad_inc = $(glad)
glad_obj = build/glad.o

freetype_inc = $(LIBDIR)/FreeType/include

al = $(LIBDIR)/openal
al_inc = $(al)/include
al_lib = $(al)/libs/Win64/

suppress = -Wno-unused-function -Wno-char-subscripts

CFLAGS = -Wall $(suppress) -g -O3 $(INCLUDES)
CXXFLAGS = -Wall $(suppress) -g -O3 $(INCLUDES)
LDFLAGS = $(LIBRARIES) -lglfw3 -lopengl32 -lglu32 -lgdi32 -lopenal32 build/OpenAL32.dll

TARGET = main.exe
cpp_files = src/main.cpp
objects = $(cpp_files:.cpp=.o) src/glad.o
OBJDIR = obj
BUILDDIR = build
GAMEDLL = $(BUILDDIR)/game.dll
TEMPGAMEDLL = $(BUILDDIR)/game_temp.dll

compile: clean
	mkdir obj
ifeq ($(wildcard $(BUILDDIR)),)
	mkdir build
endif
	$(CXX) $(CXXFLAGS) -shared -o build/game.dll src/game.cpp -Wl,--out-implib,libgame.a $(LDFLAGS)
	$(CXX) $(CXXFLAGS)  -o $(BUILDDIR)/$(TARGET) $(glad_obj) src/main.cpp  $(LDFLAGS) 

clean :
ifneq ($(wildcard $(OBJDIR)),)
	rm -r $(OBJDIR) 
endif
ifneq ($(wildcard $(GAMEDLL)),)
	rm $(GAMEDLL)
endif
ifneq ($(wildcard $(TEMPGAMEDLL)),)
	rm $(TEMPGAMEDLL)
endif
ifneq ($(wildcard $(BUILDDIR)/$(TARGET)),)
	rm $(BUILDDIR)/$(TARGET)
endif

all: $(TARGET)

$(TARGET): $(objects)
		$(CXX) -o $@ $^ $(LDFLAGS)

run:
	./build/main.exe

