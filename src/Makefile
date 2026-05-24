VERSION = dev
EXE     = connect4-$(VERSION)

CFLAGS  = -std=c2x -Wall -Wextra -Werror -pedantic -pthread \
		  -DVERSION=\"$(VERSION)\"
LDFLAGS = -pthread -lm
CC      = clang

REQ = bitboards position search transposition util

all: CFLAGS += -O3 -ffast-math
all: CFLAGS += -march=native -m64 -mpopcnt
all: CFLAGS += -mbmi2 -DUSE_PEXT
all: connect4

debug: CFLAGS += -O0 -ggdb
debug: connect4

debug-memory: CFLAGS  += -fsanitize=address
debug-memory: LDFLAGS += -fsanitize=address
debug-memory: debug

debug-thread: CFLAGS  += -fsanitize=thread
debug-thread: LDFLAGS += -fsanitize=thread
debug-thread: debug

connect4.o: connect4.c connect4.h $(REQ:=.h)

.c.o:
	$(CC) -o $@ -c $(CFLAGS) $<

connect4: connect4.o $(REQ:=.o)
	$(CC) -o $(EXE) $(REQ:=.o) connect4.o $(LDFLAGS)

clean:
	rm -f $(EXE) connect4.o $(REQ:=.o)
