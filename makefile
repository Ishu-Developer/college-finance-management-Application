# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -I./include `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0` -lsqlite3

# Source files
SOURCES = src/main.c src/database/db_init.c

# Object files
OBJECTS = build/main.o build/db_init.o

# Targets
all: bin/college_finance

bin/college_finance: $(OBJECTS)
	$(CC) $(OBJECTS) -o bin/college_finance $(LDFLAGS)

build/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

build/db_init.o: src/database/db_init.c
	$(CC) $(CFLAGS) -c src/database/db_init.c -o build/db_init.o

clean:
	rm -rf build bin

.PHONY: all clean
