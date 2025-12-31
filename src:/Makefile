CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu11
TARGET = order_management
SRC = src/main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) < examples/input_example.txt

clean:
	rm -f $(TARGET)

.PHONY: all run clean
