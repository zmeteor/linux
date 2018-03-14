objects = config.o register.o login.o list.o chat.o server.o
LIB_INC = -I /usr/include/mysql/include -L /usr/lib/mysql/lib -lmysqlclient -pthread
#objects = config.c register.c login.c list.c chat.c server.c config.h

server : $(objects)
	gcc -g  -o server $(objects) $(LIB_INC)

$(objects) : config.h

server.o : server.c
	gcc -c  server.c $(LIB_INC)

chat.o : chat.c
	gcc -c  chat.c $(LIB_INC)

register.o : register.c
	gcc -c  register.c $(LIB_INC)

login.o : login.c
	gcc -c  login.c $(LIB_INC)

list.o : list.c
	gcc -c  list.c $(LIB_INC)

config.o : config.c
	gcc -c  config.c $(LIB_INC)

.PHONY : clean
clean:
	rm -f $(objects)
	
	

