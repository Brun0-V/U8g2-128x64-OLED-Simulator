CFLAGS = -g -Wall -Icsrc `sdl2-config --cflags`
LDLIBS = `sdl2-config --libs`

LIB_SRC = $(wildcard csrc/*.c) $(wildcard sdl/*.c)
LIB_OBJ = $(LIB_SRC:.c=.o)

sim: $(LIB_OBJ) main.o
	$(CC) $(CFLAGS) $(LIB_OBJ) main.o $(LDLIBS) -o sim

run: sim
	./sim

demo: $(LIB_OBJ) displaykit_demo.o
	$(CC) $(CFLAGS) $(LIB_OBJ) displaykit_demo.o $(LDLIBS) -o demo

run-demo: demo
	./demo

clean:
	rm -f $(LIB_OBJ) main.o displaykit_demo.o sim demo
