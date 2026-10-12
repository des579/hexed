TARGET = hexed

SRC = main.c array.c
OBJS = $(SRC:.c=.o)

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LIBS = -lncurses

# main stuff
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@



# test stuff
test: test.o array.o
	gcc test.o array.o -o test

test.o: test.c
	gcc -c test.c -o test.o

array.o: array.c
	gcc -c array.c -o array.o

# cleaning
clean:
	rm -f *.o $(TARGET) test

.PHONY: all clean test
