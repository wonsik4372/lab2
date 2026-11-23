CC = gcc
CFLAGS = -Wall -g
LDLIBS = -lsystemd
TARGETS = notify_server notify_client

all: $(TARGETS)
%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)
clean:
	rm -f $(TARGETS)