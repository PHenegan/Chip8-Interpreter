COMPILER = gcc
DEBUG_ARGS = -g3 -O0
NAME = chip8
FILES = chip8.c load.c

${NAME}: ${FILES}
	gcc -o ${NAME} ${FILES}

debug: ${NAME}
	gcc -g3 -O0 -o ${NAME}-dbg ${FILES}

clean:
	rm ${NAME} ${NAME}-dbg
