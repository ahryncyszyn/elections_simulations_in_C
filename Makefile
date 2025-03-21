CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11

SRC = src/main.c src/csv_parsing.c src/mandates_calculations.c
TARGET = elections_simulation

all: $(TARGET)

$(TARGET) : $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

