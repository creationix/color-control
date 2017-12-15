CFLAGS=-g -Wall -Wshadow -Wpointer-arith -Wstrict-prototypes

pipe: sample.script compile exec
	cat $< | ./compile | ./exec

run-shell: sample.ast exec-shell
	cat $< | ./exec-shell

jflash: daplie.bin flash.jlink
	JLinkExe -device STM32F042F4 -if swd -speed 4000 flash.jlink

run: sample.script compile exec
	cat $< | ./compile | ./exec

sample.ast: sample.script compile script.grammar
	cat sample.script | ./compile > $@

compile: compile.c stdin.c stdin.h mpc.c mpc.h color-control.h
	$(CC) $(CFLAGS) $< -o $@

exec-shell: exec.c stdin.c stdin.h color-control.c color-control.h
	$(CC) $(CFLAGS) $< -o $@

exec: exec-serial.c stdin.c stdin.h color-control.c color-control.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f compile exec exec-shell sample.ast
