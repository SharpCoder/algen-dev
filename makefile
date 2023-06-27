hello_world: examples/hello_world.cpp
	g++ examples/hello_world.cpp -o hello_world

clean:
	rm -rf algen.a algen.o hello_world

test: ./hello_world
	./hello_world