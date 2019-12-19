const { promises: { readFile } } = require('fs');

async function main() {
    const wasm = await readFile('color-control.wasm');
    let heap;
    function toString(offset) {
        let str = "";
        while (heap[offset]) {
            str += String.fromCharCode(heap[offset++]);
        }

    }
    let first = true;
    const { instance: { exports } } = await WebAssembly.instantiate(wasm, {
        env: {
            puts(offset) {
                console.log(toString(offset));
            },
            //    ◢■■◣
            //  ◢■◤  ◥■◣
            // ■■      ■■
            //  ◥■◣  ◢■◤
            //    ◥■■◤
            on_update(ptr) {
                const pixels = heap.subarray(ptr);
                let str = "";
                if (first) {
                    first = false;
                }
                else {
                    str = "\x1b[5A";
                }
                str +=
                    `   \x1b[38;2;${pixels[0]};${pixels[1]};${pixels[2]}m◢■■◣\x1b[0m\n` +
                    ` \x1b[38;2;${pixels[21]};${pixels[22]};${pixels[23]}m◢■◤\x1b[0m  \x1b[38;2;${pixels[3]};${pixels[4]};${pixels[5]}m◥■◣\x1b[0m\n` +
                    `\x1b[38;2;${pixels[18]};${pixels[19]};${pixels[20]}m■■\x1b[0m      \x1b[38;2;${pixels[6]};${pixels[7]};${pixels[8]}m■■\x1b[0m\n` +
                    ` \x1b[38;2;${pixels[15]};${pixels[16]};${pixels[17]}m◥■◣\x1b[0m  \x1b[38;2;${pixels[9]};${pixels[10]};${pixels[11]}m◢■◤\x1b[0m\n` +
                    `   \x1b[38;2;${pixels[12]};${pixels[13]};${pixels[14]}m◥■■◤\x1b[0m`
                console.log(str);
            },
            on_pin(bank, pin, state) {
                console.log("on_pin", { bank, pin, state });
            },
            on_error(code, msg) {
                const err = new Error(toString(msg));
                err.code = code;
                throw err;
            },
            on_delay(ms) {
                const end = Date.now() + ms;
                while (Date.now() < end);
            }
        }
    });
    heap = new Uint8Array(exports.memory.buffer);

    const code = await readFile("power-rainbow.code");
    new Uint8Array(exports.memory.buffer, exports.get_code(), 1024).set(code.subarray(4, code.length - 2));
    const result = exports.color_control(code.length);
    console.log({ result });
}
main();
