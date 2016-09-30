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

bin/main: src/main.cpp bin/imgui.a bin/tree.o bin/octree.o bin/boundingbox.o bin/shapes.o bin/helper.o bin/interface.o
	$(CC) $(CFLAGS) bin/interface.o bin/tree.o bin/octree.o bin/boundingbox.o bin/shapes.o bin/helper.o src/main.cpp -o bin/main $(LINK)

bin/imgui.a: lib/imgui/*.cpp
	$(CC) $(CFLAGS) -c lib/imgui/imgui.cpp -o bin/imgui.o
	$(CC) $(CFLAGS) -c lib/imgui/imgui_demo.cpp -o bin/imgui_demo.o
	$(CC) $(CFLAGS) -c lib/imgui/imgui_draw.cpp -o bin/imgui_draw.o
	$(CC) $(CFLAGS) -c lib/imgui/imgui_impl_glfw.cpp -o bin/imgui_impl_glfw.o
	ar rvs bin/imgui.a bin/imgui.o bin/imgui_demo.o bin/imgui_draw.o bin/imgui_impl_glfw.o
	rm bin/imgui.o bin/imgui_demo.o bin/imgui_draw.o bin/imgui_impl_glfw.o

bin/boundingbox.o: src/boundingbox.cpp src/model.h
	$(CC) $(CFLAGS) -c src/boundingbox.cpp -o bin/boundingbox.o

bin/tree.o: src/tree.cpp src/octree_internal.h
	$(CC) $(CFLAGS) -c src/tree.cpp -o bin/tree.o

bin/octree.o: src/octree.cpp src/model.h
	$(CC) $(CFLAGS) -c src/octree.cpp -o bin/octree.o

bin/shapes.o: src/shapes.cpp src/octree_internal.h
		$(CC) $(CFLAGS) -c src/shapes.cpp -o bin/shapes.o

bin/helper.o: src/helper.cpp src/helper.h src/octree_internal.h
	$(CC) $(CFLAGS) -c src/helper.cpp -o bin/helper.o

bin/interface.o: src/interface.cpp src/interface.h
	$(CC) $(CFLAGS) -c src/interface.cpp -o bin/interface.o
clear:
	rm -f bin/main{,.exe} bin/imgui.a bin/*.o
