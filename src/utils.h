
int fflush_stdout(void); // from zeal8bitos.asm
char* strchr(const char* str, uint8_t c);
char* strtok(char* str, const char* delim);
uint16_t str_len(const char* str);
void put_s(const char* str);
void put_hex(uint8_t value);
void put_hex16(uint16_t value);
uint8_t parse_hex(const char *arg, uint16_t* value);