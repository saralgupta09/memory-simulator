CC = gcc
CFLAGS = -Wall -g

SRC = main.c \
      allocator/allocator.c \
      allocator/first_fit.c \
      allocator/best_fit.c \
      allocator/worst_fit.c \
      allocator/my_sbrk.c \
      observability/memory_dump.c


OUT = memsim.exe

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	del $(OUT)
