CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -g
LDFLAGS = 

SRCS = main.c server.c arguments.c pi.c dtp.c handlers.c utils.c session.c signals.c
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

