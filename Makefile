CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = cshell
SRCDIR = src
CMDDIR = $(SRCDIR)/commands
BUILDDIR = build

# Source files
MAIN_SRC = $(SRCDIR)/main.c
CORE_SRC = $(SRCDIR)/shell.c $(SRCDIR)/parser.c $(SRCDIR)/executor.c
CMD_SRC = $(wildcard $(CMDDIR)/*.c)
ALL_SRC = $(MAIN_SRC) $(CORE_SRC) $(CMD_SRC)

# Object files
OBJ = $(ALL_SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

.PHONY: all clean install debug

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/cshell

debug: CFLAGS += -DDEBUG
debug: $(TARGET)

rebuild: clean all
