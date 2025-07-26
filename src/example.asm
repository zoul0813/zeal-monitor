    INCLUDE "zos_sys.asm"

    ORG 0x8000

_start:
    S_WRITE3(DEV_STDOUT, _message, _message_end - _message)
    ret

_message: DEFM "Hello, World!\n"
_message_end: