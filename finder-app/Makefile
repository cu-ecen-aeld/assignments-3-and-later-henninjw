# Compiler
# CROSS_COMPILE = aarch64-none-linux-gnu-
ifdef CROSS_COMPILE
	CC = $(CROSS_COMPILE)gcc
else
	CC = gcc
endif
CFLAGS = -Wall -Wextra

# Executable name
TARGET = writer

# Source files
SRCS = writer.c

# Object files (derived from source files)
OBJS = $(SRCS:.c=.o)

# Default target
all: writer.c
	gcc writer.c -o writer
	aarch64-none-linux-gnu-gcc writer.c -o writer

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)