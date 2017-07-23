#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdint.h>

int uint_from_file(uint32_t *a, char *path) {
    FILE *file = fopen(path, "r");
    if (!file) return -1;
    fscanf(file, "%u", a);

    fclose(file);

    return 0;
}

int string_from_file(char *string, char *path) {
    FILE *file = fopen(path, "r");
    if (!file) return -1;
    fscanf(file, "%s", string);

    fclose(file);

    return 0;
}
