# Makefile for Emergency Ambulance Route Finder
# MVC architecture: Model + View + Controller
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2
LDFLAGS = -lm

SRC = main.c \
      controller/controller.c \
      model/graph.c model/dijkstra.c model/priority_queue.c \
      model/ambulance.c model/hospital.c model/emergency.c \
      model/file_handler.c \
      utils/search_sort.c \
      view/ui.c

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
