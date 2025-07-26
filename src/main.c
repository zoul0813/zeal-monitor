// #include <stdio.h>
#include <stdint.h>
#include <stddef.h>
// #include <string.h>
// #include <stdlib.h>
#include <ctype.h>
#include <zos_errors.h>
#include <zos_vfs.h>
#include <zos_sys.h>

#include "utils.h"

#define CMD_BUFFER_SIZE 128

zos_err_t err;
char cmd[CMD_BUFFER_SIZE + 1];
char *cmd_tok;
char *cmd_sep = " ";

uint16_t size;
uint16_t range;
uint16_t addr;

zos_err_t parse_arg(uint16_t* val) {
    cmd_tok = strtok(NULL, cmd_sep);
    if(cmd_tok == NULL) {
        return ERR_NO_MORE_ENTRIES;
    }
    err = parse_hex(cmd_tok, val);
    return err;
}

zos_err_t parse_addr(void) {
    return parse_arg(&addr);
}

zos_err_t parse_range(void) {
    range = 256;
    err = parse_arg(&range);
    if(err == ERR_NO_MORE_ENTRIES) return ERR_SUCCESS;
    if(err != ERR_SUCCESS) return err;
    range++;
    return ERR_SUCCESS;
}

void dump(uint16_t addr, uint16_t len) {
    if(len == 0) return;
    for(uint16_t i = 0; i < len; i++) {
        uint8_t value = *((uint8_t *)(uintptr_t)(addr + i));
        if(i % 16 == 0) {
            // printf("\n%04X: ", addr + i);
            put_s("\n");
            put_hex16(addr + i);
            put_s(": ");
        }
        // printf("%02X ", value);
        put_hex(value);
        put_s(" ");
    }
    put_s("\n\n");
}

void poke(uint16_t addr, uint16_t len, uint8_t* buffer) {
    if(len == 0) return;
    for(uint16_t i = 0; i < len; i++) {
        *((uint8_t *)(uintptr_t)(addr + i)) = buffer[i];
    }
}

zos_err_t save(uint16_t addr, uint16_t len, char* fname) {
    if(len == 0) return ERR_INVALID_OFFSET;
    zos_dev_t dev = open(fname, O_CREAT | O_WRONLY);
    if(dev < 0) return -dev;

    size = len;
    uint8_t* ptr = ((uint8_t *)(uintptr_t)(addr));
    write(dev, ptr, &size);
    return close(dev);
}

zos_err_t load(uint16_t addr, char* fname) {
    zos_dev_t dev = open(fname, O_RDONLY);
    if(dev < 0) return -dev;

    size = 16 * 255;
    uint8_t* ptr = ((uint8_t *)(uintptr_t)(addr));
    read(dev, ptr, &size);
    return close(dev);
}

void run(uint16_t addr) {
    void (*func)(void) = (void (*)(void))addr;
    func();  // Jump to and execute code at addr
}

int main(void) {
    put_s("Zeal Monitor v0.0.0\nby David Higgins 2025\nType 'h' for help\n\n");

    while(1) {
        put_s("> "); fflush_stdout();
        size = CMD_BUFFER_SIZE;
        err = read(DEV_STDIN, cmd, &size);
        if(err != ERR_SUCCESS) {
            // printf("%d: error reading user input\n", err);
            put_hex(err);
            put_s(": error reading user input\n");
            break;
        }
        cmd[size] = '\0';

        cmd_tok = strtok(cmd, cmd_sep);
        while(cmd_tok != NULL) {
            switch(cmd_tok[0]) {
                case 'r': { // memory
                    err = parse_addr();
                    if(err != ERR_SUCCESS) goto end_loop;

                    err = parse_range();
                    if(err != ERR_SUCCESS) goto end_loop;

                    dump(addr, range);
                } break;
                case 'w': { // poke
                    err = parse_addr();
                    if(err != ERR_SUCCESS) goto end_loop;

                    uint8_t buffer[CMD_BUFFER_SIZE];
                    size = 0;
                    while(err == ERR_SUCCESS && size < CMD_BUFFER_SIZE) {
                        parse_arg(&buffer[size]);
                        size++;
                    }
                    poke(addr, size, buffer);
                } break;
                case 's': { // save
                    err = parse_addr();
                    if(err != ERR_SUCCESS) goto end_loop;

                    err = parse_range();
                    if(err != ERR_SUCCESS) goto end_loop;

                    err = save(addr, range, "a.out");
                    if(err != ERR_SUCCESS) {
                        // printf("error saving file: %d [%02X]\n", err, err);
                        put_s("error saving file: ");
                        put_hex(err);
                        put_s("\n"); //%d [%02X]\n", err, err);
                    }
                } break;
                case 'l': { // load
                    err = parse_addr();
                    if(err != ERR_SUCCESS) goto end_loop;

                    err = load(addr, "a.out");
                    if(err != ERR_SUCCESS) {
                        // printf("error saving file: %d [%02X]\n", err, err);
                        put_s("error saving file: ");
                        put_hex(err); // %d [%02X]\n", err, err);
                        put_s("\n");
                    }
                } break;
                case 'e': { //exec
                    err = parse_addr();
                    if(err != ERR_SUCCESS) goto end_loop;
                    run(addr);
                } break;
                case 'd': { // disassemble
                } break;
                case 'h': { // help
                    put_s("\n");
                    put_s("r[ead]  ADDR                 - dump memory starting at ADDR to ADDR+256\n");
                    put_s("r[ead]  ADDR [RANGE]         - dump memory starting at ADDR to ADDR+RANGE\n");
                    put_s("w[rite] ADDR [bytes]         - write [bytes] starting at ADDR\n");
                    put_s("s[ave]  ADDR [RANGE] [a.out] - write ADDR to ADDR+RANGE to [a.out]\n");
                    put_s("l[oad]  ADDR [a.out]         - load [a.out] into ADDR\n");
                    put_s("e[xec]  ADDR                 - execute starting at ADDR\n");
                    put_s("\n");
                } break;
                case 'q': { // quit
                    put_s("Quit\n");
                    goto do_exit;
                }
            }
            if(cmd_tok != NULL) cmd_tok = strtok(NULL, cmd_sep);
end_loop:
        }
    }

do_exit:
    return 0;
}