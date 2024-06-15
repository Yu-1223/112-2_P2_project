all: toml.o display_interface.o
	gcc display_interface.o toml.o main.c -o main -lncurses  -lSDL2 -lSDL2_mixer
	gcc display_interface.o toml.o main_demo.c -o main_demo -lncurses  -lSDL2 -lSDL2_mixer
	gcc display_interface.o toml.o main_w_audio_demo.c -o main_w_audio_demo -lncurses  -lSDL2 -lSDL2_mixer
toml:
	gcc toml.c -o toml.o
display:
	gcc display_interface.c -o display_interface.o
