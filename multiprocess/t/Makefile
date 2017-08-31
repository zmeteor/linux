target = m

src = $(wildcard ./*.c)
obj = $(patsubst ./%.c ,./%.o,$(src))

CC = gcc
CPPFLAGS = -I
$(target):$(obj)
	$(CC) $(obj) -o $(target)
%.o:%.c
	$(CC) -o $< -o $@
./Phony:clean
clean:
	rm $(obj) $(target)
