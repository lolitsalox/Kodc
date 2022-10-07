#include "include/io.h"
#include "include/error.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

char* io_read(const char* filename) {
    FILE* fp; 
    long fsize = 0;

    if (!(fp = fopen(filename, "rb")))
        error_open(filename);

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = (char*) malloc(fsize * sizeof(char) + 1);
    if (fread(buffer, fsize, 1, fp) != 1) {
        error_read(filename);
    }
    buffer[fsize] = 0;

    fclose(fp);
    return buffer;
}

void io_write(const char* filename, const char* src) {
    FILE* fp = NULL;

    if (!(fp = fopen(filename, "wb")))
        error_open(filename);

    if (fwrite(src, strlen(src), 1, fp) != 1) {
        error_write(filename);
    }

    fclose(fp);
}
