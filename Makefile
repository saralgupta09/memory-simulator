CC = gcc

CFLAGS = -Wall -g -DENABLE_CACHE

SRC = \
main.c \
simulator/cli.c \
allocator/allocator.c \
allocator/first_fit.c \
allocator/best_fit.c \
allocator/worst_fit.c \
allocator/my_sbrk.c \
observability/memory_dump.c \
stats/stats.c \
cache/cache.c

memsim.exe:
	$(CC) $(CFLAGS) $(SRC) -o memsim.exe
