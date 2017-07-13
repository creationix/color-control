CFLAGS=-g -m64 -std=c99 -Wall -Wshadow -Wpointer-arith -Wstrict-prototypes

pipe: sample.script compile exec 
	cat $< | ./compile | ./exec

run: sample.ast exec
	cat $< | ./exec

sample.ast: sample.script test compile script.grammar
	cat sample.script | ./compile > $@

compile: compile.c stdin.c stdin.h mpc.c mpc.h color-control.h
	$(CC) $(CFLAGS) $< -o $@

exec: exec.c stdin.c stdin.h color-control.c color-control.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f compile exec sample.ast
