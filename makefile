# Compiler options
CC=gcc
CFLAGS=-g
CPPFLAGS=-Wall -Wextra -pedantic

# Build directories
SRC=./src
BIN=./bin
OBJ=./obj

.PHONY: clean

all: $(BIN) $(OBJ) $(BIN)/calculator

$(BIN): 
	mkdir $(BIN)

$(OBJ):
	mkdir $(OBJ)

$(BIN)/calculator: $(OBJ)/calculator.o $(OBJ)/formula.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(BIN)/calculator $(OBJ)/calculator.o $(OBJ)/formula.o -lm -lncurses

$(OBJ)/calculator.o: $(SRC)/calculator.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $(OBJ)/calculator.o $(SRC)/calculator.c

$(OBJ)/formula.o: $(SRC)/formula.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $(OBJ)/formula.o $(SRC)/formula.c
clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)