# ccosta - osta language compiler in c
.POSIX:

include config.mk

SRC = src/costa.c src/syntax/lex.c
OBJ = $(SRC:.c=.o)

all: src/osta

.c.o:
	$(CC) $(CFLAGS) $(LIBS) -c $< -o $@

src/costa.o: src/syntax/lex.h
src/lex.o: src/syntax/token.h src/syntax/lex.h

$(OBJ): config.mk

osta: src/osta

src/osta: $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

clean:
	rm -fr src/*.o src/syntax/*.o src/osta

install: src/osta
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f src/osta $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/osta

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/osta

.PHONY: osta all clean install uninstall