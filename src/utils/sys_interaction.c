#include "sys_interaction.h"
inline char* readFile(const char* filename, size_t* size) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);
    char* buffer = malloc(*size);
    fread(buffer, 1, *size, file);
    fclose(file);
    return buffer;
}