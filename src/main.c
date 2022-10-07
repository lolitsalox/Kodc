#include <stdio.h>
#include <stdlib.h>

#include "include/kod.h"
#include "include/error.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        error_usage();
    }

    kod_compile_file(argv[1]);
    
    return 0;
}