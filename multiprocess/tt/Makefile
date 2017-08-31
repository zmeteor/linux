DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

SRC = $(wildcard ${DIR_SRC}/%.c)
OBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TABGET = main
BIN_TABGET = ${DIR_BIN}/${TABGET}

CC = gcc

CFLAGS = -g -Wall -I${DIR_INC}
${BIN_TABGET}:${OBJ}
	$(CC) $(OBJ) -o $@
${DIR_OBJ}/%.o:${DIR_SRC}/%.c
	$CC $(CFLAGS) -c $< -o $@
./PHONY:clean
clean:
	find ${DIR_OBJ} -name *.o -exec rm -rf {}

