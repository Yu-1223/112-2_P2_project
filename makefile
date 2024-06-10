all: toml.o test.c display_interface.o
	gcc display_interface.o toml.o main.c -o main -lncurses
	gcc display_interface.o toml.o main_demo.c -o main_demo -lncurses
toml:
	gcc toml.c -o toml.o
display:
	gcc display_interface.c -o display_interface.o