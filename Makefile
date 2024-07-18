CC=gcc
CFLAGS=-Wall -Wextra -std=c17 -pedantic -ggdb

# Define the output directory for binaries and the executable name
EXE=emulator
SRCDIR=src
BINDIR=bin
OBJS=$(BINDIR)/main.o $(BINDIR)/math.o $(BINDIR)/helper.o $(BINDIR)/debug.o $(BINDIR)/core.o

all: $(EXE)

# Generic rule for building objects from corresponding C files in the src directory
$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Clean the build directory
clean:
	rm -f $(OBJS) $(EXE)

.PHONY: all clean
