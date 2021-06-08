CPP = $(shell find src -name *.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(CPP))
SHADERS = $(shell find src/shaders/*)
LIBS = `sdl2-config --libs` -lGLEW -lGLU -lGL
C_FLAGS = -std=c++17

all: x64
	cp -r src/shaders bin

run: all
	./bin/fractal-renderer-x64

$(OBJ): build/%.o : src/%.cpp
	mkdir -p $(dir $@) && \
	g++ -c -o $@ -Og -g $(patsubst build/%.o, src/%.cpp, $@) $(LIBS) $(C_FLAGS)

.PHONY: x64

x64: $(OBJ)
	mkdir -p bin && \
	g++ -o bin/fractal-renderer-x64 $^ $(LIBS)
	chmod a+x bin/fractal-renderer-x64

.PHONY: clean
clean:
	rm -rf build
	rm -rf bin
