objects = config.o register.o login.o chat.o checkpasswd.o client.o interface.o

server : $(objects)
	gcc -o client $(objects) -pthread


$(objects) : config.h

client.o : client.c
chat.o : chat.c
register.o : register.c
login.o : login.c
interface.o : interface.c
checkpasswd.o :checkpasswd.c
config.o : config.c

.PHONY : clean
clean:
	rm -f  $(objects)
	
	

