#include <stdio.h>
#include <stdint.h>

int uint_from_file(uint32_t *a, char *path) {
    FILE *file = fopen(path, "r");
    if (!file) return -1;
    int matches = fscanf(file, "%u", a);

    fclose(file);

    return matches;
}

int string_from_file(char *string, char *path) {
    FILE *file = fopen(path, "r");
    if (!file) return -1;
    int matches = fscanf(file, "%s", string);

    fclose(file);

    return matches;
}
