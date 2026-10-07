CC = gcc
CFLAGS = -Wall -g
LIBS = -lncurses -lm
TARGET = hexed

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
