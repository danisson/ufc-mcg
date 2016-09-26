# usando g++ como compilador
CC = g++

# Escolher OpenGL e GLFW dependendo do OS
LINKGL = -lglfw -lGL
ifeq "$(OS)" "Windows_NT"
LINKGL = -lglfw3 -lopengl32
endif

# compiler flags:
#  -g        adiciona informações de debug no executável
#  -Wall     ativa a maioria dos erros de compilação
#  -std=c+14 usa o standard mais recente
CFLAGS = -g -Wall -std=c++14 -L lib -L bin -I inc -fdiagnostics-color=always
LINK = -lm -l:imgui.a $(LINKGL)

default: bin/main

bin/main: src/main.cpp bin/imgui.a bin/octree.o bin/boundingbox.o bin/helper.o
	$(CC) $(CFLAGS) bin/octree.o bin/boundingbox.o bin/helper.o src/main.cpp -o bin/main $(LINK)

bin/imgui.a: lib/imgui/*.cpp
	$(CC) $(CFLAGS) -c lib/imgui/imgui.cpp -o bin/imgui.o
	$(CC) $(CFLAGS) -c lib/imgui/imgui_demo.cpp -o bin/imgui_demo.o
	$(CC) $(CFLAGS) -c lib/imgui/imgui_draw.cpp -o bin/imgui_draw.o
	$(CC) $(CFLAGS) -c lib/imgui/imgui_impl_glfw.cpp -o bin/imgui_impl_glfw.o
	ar rvs bin/imgui.a bin/imgui.o bin/imgui_demo.o bin/imgui_draw.o bin/imgui_impl_glfw.o
	rm bin/imgui.o bin/imgui_demo.o bin/imgui_draw.o bin/imgui_impl_glfw.o

bin/boundingbox.o: src/boundingbox.cpp src/model.h
	$(CC) $(CFLAGS) -c src/boundingbox.cpp -o bin/boundingbox.o

bin/octree.o: src/octree.cpp src/model.h
	$(CC) $(CFLAGS) -c src/octree.cpp -o bin/octree.o

bin/helper.o: src/helper.cpp src/helper.h
	$(CC) $(CFLAGS) -c src/helper.cpp -o bin/helper.o
clear:
	rm -f bin/main{,.exe} bin/imgui.a bin/*.o
