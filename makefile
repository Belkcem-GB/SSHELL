CC = gcc
CFLAGS = -Wall -Wextra
EXECUTABLE = sshell

all: $(EXECUTABLE)

$(EXECUTABLE): sshell.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

