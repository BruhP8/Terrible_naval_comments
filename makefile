SRC=camp.c camp_allocator.c darray.c game_logic.c player.c main.c
OBJ=$(SRC:.c=.o)
CC=gcc
FLAGS=-Wall
HEADER=master_head.h
TARGET=exec.out

$(TARGET): $(OBJ) $(HEADER)
	$(CC) -o $(TARGET) $(OBJ) -lncurses

%.o: %.c $(HEADER)
	$(CC) -c $< $(FLAGS)

clean:
	rm *.o

cleanall:
	rm *.o $(TARGET)