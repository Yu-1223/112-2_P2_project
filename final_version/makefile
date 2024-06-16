all: main

main: toml.o display_interface.o
	gcc display_interface.o toml.o main.c -o main -lncurses -lSDL2 -lSDL2_mixer
	gcc display_interface.o toml.o main_demo.c -o main_demo -lncurses -lSDL2 -lSDL2_mixer

toml.o: toml.c
	gcc -c toml.c -o toml.o

display_interface.o: display_interface.c
	gcc -c display_interface.c -o display_interface.o

clean:
	rm -f *.o main

