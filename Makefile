CFLAGS=-Wall -Werror -std=c99

test-compile: test compile
	cat test | ./compile

test-exec: test compile exec
	cat test | ./compile | ./exec

compile: compile.c stdin.c mpc.c mpc.h color-control.h
	$(CC) $(CFLAGS) $< -o $@

exec: exec.c stdin.c color-control.c color-control.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f compile exec
