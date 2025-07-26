#include <stdint.h>
#include <stddef.h>
#include <zos_vfs.h>
#include <zos_sys.h>

char* strchr(const char* str, uint8_t c)
{
    while (*str) {
        if (*str == (char) c) {
            return (char*) str; // Return pointer to the found character
        }
        str++;
    }

    return NULL; // Return NULL if character not found
}

char* strtok(char* str, const char* delim)
{
    static char* next_token = NULL; // Stores the next position to continue tokenizing
    if (str) {
        next_token = str; // Start with new input string
    } else if (!next_token) {
        return NULL; // No more tokens left
    }

    // Skip leading delimiters
    while (*next_token && strchr(delim, *next_token)) {
        next_token++;
    }

    if (!*next_token)
        return NULL; // If the string is empty after skipping delimiters

    char* token_start = next_token; // Start of the token

    // Find the end of the token
    while (*next_token && !strchr(delim, *next_token)) {
        next_token++;
    }

    if (*next_token) {
        *next_token = '\0'; // Null-terminate the token
        next_token++;       // Move past the delimiter for the next call
    } else {
        next_token = NULL; // No more tokens left
    }

    return token_start;
}

int fflush_stdout(void); // from zeal8bitos.asm

uint16_t str_len(const char* str)
{
    uint16_t length = 0;
    while (str[length]) length++;
    return length;
}

void put_s(const char* str) {
    uint16_t size = str_len(str);
    zos_err_t err = write(DEV_STDOUT, str, &size);
    if(err != ERR_SUCCESS) exit(err);
}

void _put_hex(uint8_t value, char alpha) {
    char buffer[3];
    // Convert high nibble
    uint8_t high = (value >> 4) & 0x0F;
    buffer[0] = (high < 10) ? ('0' + high) : (alpha + high - 10);

    // Convert low nibble
    uint8_t low = value & 0x0F;
    buffer[1] = (low < 10) ? ('0' + low) : (alpha + low - 10);

    buffer[2] = 0; // Null terminator
    put_s(buffer);
}

void put_hex(uint8_t value) {
    _put_hex(value, 'A');
}

void put_hex16(uint16_t value) {
    uint8_t lo = value & 0xFF;
    uint8_t hi = (value >> 8) & 0xFF;
    put_hex(hi);
    put_hex(lo);
}

uint16_t parse_hex16(const char *str, char **endptr) {
    uint16_t result = 0;
    char c;
    const char *p = str;

    // Skip "0x" or "0X"
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }

    while ((c = *p)) {
        uint8_t digit;
        if (c >= '0' && c <= '9') {
            digit = (c - '0');
        } else if (c >= 'A' && c <= 'F') {
            digit = (c - 'A' + 10);
        } else if (c >= 'a' && c <= 'f') {
            digit = (c - 'a' + 10);
        } else break;
        result = (result << 4) | digit;
        p++;
    }

    if(endptr) {
        *endptr = (char *)p;
    }

    return result;
}

int isspace(char c) {
    return c == ' '  || c == '\t' ||
           c == '\n' || c == '\r';
}


uint8_t parse_hex(const char *arg, uint16_t* value) {
    char *endptr;
    uint16_t val = parse_hex16(arg, &endptr); // strtoul(arg, &endptr, 16);
    while(isspace((uint8_t)*endptr)) endptr++;
    if(*endptr != '\0') {
        put_s("Invalid Hex string: ");
        put_s(arg);
        put_s(" ");
        put_hex16(val);
        put_s("\n");
        return 1;
    }
    *value = val;
    return 0;
}