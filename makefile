hello_world: examples/hello_world.cpp
	g++ -c src/algen.cpp
	ar rvs algen.a algen.o
	g++ examples/hello_world.cpp algen.a -o hello_world

clean:
	rm -rf algen.a algen.o hello_world

test: ./hello_world
	./hello_world