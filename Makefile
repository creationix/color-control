CFLAGS=-m64 -std=c99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual \
        -Wstrict-prototypes -Wmissing-prototypes

test-compile: test compile
	cat sample.script | ./compile

test-exec: test compile exec
	cat test | ./compile | ./exec

compile: compile.c stdin.c stdin.h mpc.c mpc.h color-control.h
	$(CC) $(CFLAGS) $< -o $@

exec: exec.c stdin.c stdin.h color-control.c color-control.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f compile exec
