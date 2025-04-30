# ccosta - osta language compiler in c
.POSIX:

include config.mk

SRC = src/costa.c src/syntax/lex.c
OBJ = $(SRC:.c=.o)

all: src/costa

.c.o:
	$(CC) $(CFLAGS) $(LIBS) -c $< -o $@

# executables

src/costa.o: src/syntax/lex.h
src/lex.o: src/syntax/token.h src/syntax/lex.h src/utils/da.h

$(OBJ): config.mk

src/costa: $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

costa: src/costa

src/costa.a: $(filter-out src/costa.o, $(OBJ))
	ar rcs $@ $^

# tests

TESTS_SRC = $(filter-out test/test_runner.c, $(wildcard test/.c))
TESTS = $(TESTS_SRC:.c=)

test/test_runner: test/test_runner.c
	$(CC) -o $@ $(OBJ) $(CFLAGS)

$(TESTS): %: %.c src/costa.a
	$(CC) $(CFLAGS) $< -o $@

test: $(TESTS) test/test_runner
	test/test_runner

# misc

clean:
	rm -rf src/*.o src/syntax/*.o src/costa src/costa.a

install: src/osta
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f src/osta $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/costa

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/costa

.PHONY: osta all clean test install uninstall