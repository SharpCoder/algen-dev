all: examples/hello_world.cpp
	g++ -Wall examples/hello_world.cpp -o hello_world

profile:
	g++ -pg examples/hello_world.cpp -o hello_world.profile
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./hello_world.profile


clean:
	rm -rf algen.a algen.o hello_world hello_world.profile

test: ./hello_world
	./hello_world