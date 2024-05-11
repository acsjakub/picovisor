CC=gcc
CPPFLAGS=-Iinclude
CFLAGS = -Wall
BUILDDIR=build
SRCDIR=src

# all the source files
SRC = $(wildcard $(SRCDIR)/*.c)

# all the object files
OBJ = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRC))

.PHONY: test clean

visor: $(OBJ) | $(BUILDDIR)
	$(CC) $(LDFLAGS) $^ -o $@

# $< is the first prerequisite
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $@

clean:
	rm -r $(BUILDDIR) visor

