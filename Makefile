# CFLAGS=-g -Wall -Wshadow -Wpointer-arith -Wstrict-prototypes
CFLAGS=-Werror -Wall -Wshadow -Wpointer-arith
TARGETS= \
 	color-control-compile \
	color-control-simulate \
	color-control-upload

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

mpc:
	git submodule update --init

color-control-%: src/%.c src/libs/stdin.c src/libs/stdin.h src/libs/color-control.c src/libs/color-control.h src/mpc
	$(CC) $(CFLAGS) $< -o $@

test: test-simulator test-frame test-upload

test-simulator: $(TARGETS)
	./color-control-compile < scripts/power-rainbow.script | ./color-control-simulate

test-frame: $(TARGETS)
	./color-control-compile < scripts/power-rainbow.script | ./color-control-upload /dev/stdout | hexdump -C

test-upload: $(TARGETS)
	./color-control-compile < scripts/power-rainbow.script | ./color-control-upload /dev/ttyACM0
