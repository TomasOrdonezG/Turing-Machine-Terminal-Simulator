DATA = data/2.tm

CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, bin/%.o, $(SRC))
EXEC = bin/program

all: $(EXEC)

run: $(EXEC)
	./$(EXEC) $(DATA)

valgrind: $(EXEC)
	valgrind ./$(EXEC) $(DATA)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

bin/%.o: src/%.c | bin
	$(CC) $(CFLAGS) -c $< -o $@

bin:
	mkdir -p bin

clean:
	rm -rf bin