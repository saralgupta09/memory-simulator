CC = gcc
CFLAGS = -Wall -g

SRC = \
	main.c \
	allocator/my_malloc.c \
	allocator/my_sbrk.c \
	observability/memory_dump.c

OUT = memsim

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)
