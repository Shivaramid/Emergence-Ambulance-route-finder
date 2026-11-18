# Makefile for Emergency Ambulance Route Finder
# Standard C / GCC Compatible

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2
LDFLAGS = -lm

SRC = main.c graph.c dijkstra.c priority_queue.c ambulance.c hospital.c emergency.c file_handler.c search_sort.c ui.c
OBJ = $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
    TARGET = ambulance_system.exe
    RM = del /Q /F
else
    TARGET = ambulance_system
    RM = rm -f
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	$(RM) $(TARGET) $(OBJ) 2>nul || true

run: $(TARGET)
ifeq ($(OS),Windows_NT)
	.\$(TARGET)
else
	./$(TARGET)
endif

.PHONY: all clean run
