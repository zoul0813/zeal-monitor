    INCLUDE "zos_sys.asm"
    INCLUDE "include/strutils_h.asm"

    ORG 0x4000

    MACRO PUTCHAR CHAR
        ld a, CHAR
        call _putchar
    ENDM

_start:
    S_WRITE3(DEV_STDOUT, _msg_splash, _msg_splash_end - _msg_splash)

_main_loop:
    S_WRITE3(DEV_STDOUT, _msg_prompt, _msg_prompt_end - _msg_splash_end)
    S_READ3(DEV_STDIN, _buffer, 128)
    or a ; check for errors
    jr nz, _end

    ; null-terminate the buffer
    ld hl, _buffer
    add hl, bc
    ld (hl), 0
    ld hl, _buffer

    ld a, (_buffer)
    cp 'q'
    jp z, _quit
    cp 'h'
    jp z, _help
    cp 'w'
    jp z, _write
    cp 'r'
    jp z, _read
    cp 's'
    jp z, _save
    cp 'l'
    jp z, _load
    cp 'e'
    jp z, _exec
    jp c, _error    ; unknown command

    jp _main_loop

_end:
    ld h, a
    EXIT()

_quit:
    S_WRITE3(DEV_STDOUT, _msg_quit, _msg_quit_end - _msg_quit)
    jp _end

_help:
    S_WRITE3(DEV_STDOUT, _msg_help, _msg_help_end - _msg_help)
    jp _main_loop

_read:
    ld a, ' '
    call strchrnul
    or a
    jp z, _error

    push hl ; remember where we were
    call parse_hex
    ld (_addr), hl


    ld hl, 0xFF
    ld (_range), hl ; default value for range

    pop hl ; get the buffer addr back

    ld a, ' ' ; find next arg
    call strchrnul
    or a
    jp z, @end
    push hl ; remember where we were

    call parse_hex
    ld (_range), hl

    ld hl, (_addr)
    call _put_hl

    ld bc, (_range)
    inc bc ; we count down, BC++
    xor a
    ld (_counter), a

@loop:
    push hl
    push bc

    call _put_byte_hl
    PUTCHAR(' ')

    ld a, (_counter)
    inc a
    and 0x0F
    ld (_counter), a
    jr nz, @skip_addr
    PUTCHAR('\n');
    pop bc
    pop hl
    push hl
    push bc
    call _put_hl

@skip_addr:
    pop bc
    dec bc
    ld a, b
    or c
    pop hl
    jp z, @end
    inc hl
    jp @loop

@end:
    PUTCHAR('\n')
    jp _main_loop

_write:

@end:
    jp _main_loop

_save:

@end:
    jp _main_loop

_load:

@end:
    jp _main_loop

_exec:

@end:
    jp _main_loop

_error:
    S_WRITE3(DEV_STDOUT, _msg_error, _msg_error_end - _msg_error)
@end:
    jp _main_loop


; Print a single char
; Parameters:
;   A - The byte to print
; Alters:
;   A, HL, DE, BC
_putchar:
    ld (_putchar_buffer), a
    S_WRITE3(DEV_STDOUT, _putchar_buffer, 1)
    ret
_putchar_buffer: DEFB 0

; Print the value stored in _addr as hex to screen
; Alters:
;   A, HL, DE, BC
_put_addr:
    push hl
    ld hl, _addr
    call _put_hl
    pop hl
    ret

_put_hl:
    push hl
    ld a, h
    call _put_byte
    pop hl
    push hl
    ld a, l
    call _put_byte
    S_WRITE3(DEV_STDOUT, _msg_addr_suffix, _msg_addr_suffix_end - _msg_addr_suffix)
    pop hl
    ret

_put_byte_hl:
    ld a, (hl)
_put_byte:
    call byte_to_ascii
    push de
    ld a, e
    call _putchar
    pop de
    ld a, d
    call _putchar
    ret

_msg_splash: DEFM "Zeal Monitor v0.0.0\n"
             DEFM "by David Higgins 2025\n"
             DEFM "Type 'h' for help\n\n"
_msg_splash_end:

_msg_prompt: DEFM "> "
_msg_prompt_end:
_msg_addr_suffix: DEFM ": "
_msg_addr_suffix_end:
_msg_quit: DEFM "Quit\n"
_msg_quit_end:
_msg_error: DEFM "An error has occurred\n"
_msg_error_end:
_msg_help:
    DEFM "\n"
    DEFM "r[ead]  ADDR                 - dump memory starting at ADDR to ADDR+256\n"
    DEFM "r[ead]  ADDR [RANGE]         - dump memory starting at ADDR to ADDR+RANGE\n"
    DEFM "w[rite] ADDR [bytes]         - write [bytes] starting at ADDR\n"
    DEFM "s[ave]  ADDR [RANGE] [a.out] - write ADDR to ADDR+RANGE to [a.out]\n"
    DEFM "l[oad]  ADDR [a.out]         - load [a.out] into ADDR\n"
    DEFM "e[xec]  ADDR                 - execute starting at ADDR\n"
    DEFM "\n"
_msg_help_end:

_addr: DEFW 0
_range: DEFW 0
_size: DEFW 0
_counter: DEFB 0
_buffer: DEFS 128