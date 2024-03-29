# CFLAGS=-g -Wall -Wshadow -Wpointer-arith -Wstrict-prototypes
CFLAGS=-Werror -Wall -Wshadow -Wpointer-arith
TARGETS= \
 	color-control-compile \
	color-control-simulate

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

color-control.wasm: src/wasm.c src/libs/color-control.c
	EMCC_ONLY_FORCED_STDLIBS=1 emcc -Os $< -o $@ -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s EMIT_EMSCRIPTEN_METADATA -Wl,--export-table
	wasm2wat $@ -o color-control.wat

src/mpc/mpc.c:
	git submodule update --init

color-control-%: src/%.c src/libs/reader.c src/libs/reader.h src/libs/stdin.c src/libs/stdin.h src/libs/color-control.c src/libs/color-control.h src/mpc/mpc.c
	$(CC) $(CFLAGS) $< -o $@

test: test-simulator test-frame test-upload

test-simulator: $(TARGETS)
	./color-control-compile < scripts/short.script | ./color-control-simulate

test-frame: $(TARGETS)
	./color-control-compile < scripts/short.script | hexdump -C

test-upload: $(TARGETS)
	./color-control-compile < scripts/short.script > /dev/ttyACM0
