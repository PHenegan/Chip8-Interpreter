COMPILER = gcc
DEBUG_ARGS = -g3 -O0
NAME = chip8

${NAME}: ${NAME}.c
	gcc -o ${NAME} ${NAME}.c

debug: ${NAME}
	gcc -g3 -O0 -o ${NAME}-dbg ${NAME}.c

clean:
	rm ${NAME} ${NAME}-dbg
