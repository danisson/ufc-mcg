# usando g++ como compilador
CC = g++

# Escolher OpenGL e GLFW dependendo do OS
LINKGL = -lgflw -lGL
ifeq "$(OS)" "Windows_NT"
LINKGL = -lgflw3 -lopengl32
endif

# compiler flags:
#  -g        adiciona informações de debug no executável
#  -Wall     ativa a maioria dos erros de compilação
#  -std=c+11 usa o standard mais recente
CFLAGS = -g -Wall -std=c++11 -L lib -L bin -I inc
LINK = -lm -l:imgui.a $(LINKGL)

default: bin/main

bin/main: src/main.cpp bin/imgui.a
	$(CC) $(CFLAGS) src/main.cpp -o bin/main $(LINK)

bin/imgui.a: lib/imgui/*.cpp
	$(CC) $(CFLAGS) -c lib/imgui/imgui.cpp -o bin/imgui.o
	$(CC) $(CFLAGS) -c lib/imgui/imgui_demo.cpp -o bin/imgui_demo.o
	$(CC) $(CFLAGS) -c lib/imgui/imgui_draw.cpp -o bin/imgui_draw.o
	$(CC) $(CFLAGS) -c lib/imgui/imgui_impl_glfw.cpp -o bin/imgui_impl_glfw.o
	ar rvs bin/imgui.a bin/imgui.o bin/imgui_demo.o bin/imgui_draw.o bin/imgui_impl_glfw.o
	rm bin/imgui.o bin/imgui_demo.o bin/imgui_draw.o bin/imgui_impl_glfw.o

clear:
	rm bin/main{,.exe} bin/imgui.a
