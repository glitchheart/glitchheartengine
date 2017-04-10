CC = gcc
CXX = g++

INCLUDES = -I$(glfw_inc) -I$(glad_inc)
LIBRARIES = -L$(glfw_lib) ./libs/SOIL/libSoil.a

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
headers =

all: $(TARGET)

$(TARGET): $(objects)
		$(CXX) -o $@ $^ $(LDFLAGS)

clean :
		del "$(TARGET)"