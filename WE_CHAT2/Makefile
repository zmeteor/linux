CC := gcc
LIB_INC = -I /usr/include/mysql/include -L /usr/lib/mysql/lib -lpthread -lmysqlclient

server := $(wildcard ./server/source/*.c)
client := $(wildcard ./client/source/*.c)

serverObj := $(patsubst %.c,%.o,$(server))
clientObj := $(patsubst %.c,%.o,$(client))

.PHONY : all

all: ser cli

ser : $(serverObj)
	$(CC) $^ -o $@ $(LIB_INC)

cli : $(clientObj)
	$(CC) $^ -o $@ $(LIB_INC)

%.o:%.c
	$(CC) -c $^ -o $@ $(LIB_INC)

.PHONY:clean

clean:
	rm -f ./server/source/*.o ./client/source/*.o ./ser ./cli

