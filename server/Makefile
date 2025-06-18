CC = gcc
CFLAGS = -Wall -Wextra -std=gnull -g
LDFLAGS = 

SRCS = main.c server.c arguments.c pi.c dtp.c handlers.c
OBJS = $(SRCS:.c=.o)
TARGET = miniftp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(FLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJS) $(TARGET)

