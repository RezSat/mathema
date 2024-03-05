CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# List of source files
SRCS = main.c lexer.c helper.c

# List of object files
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = lexer

# Build executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJS) $(EXEC)

# Phony target to avoid conflicts with files named clean
.PHONY: clean
