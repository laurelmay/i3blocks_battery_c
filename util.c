#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define POWER_SUPPLY_DIR "/sys/class/power_supply"

int battery_file_path(char **dest, char *path, char *battery_name, char *file) {
    return asprintf(dest, "%s/%s/%s", path, battery_name, file);
}

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

int read_battery_data_str(char *dest, char *batt_name, char* file) {
    char *full_path;
    int size = battery_file_path(&full_path, POWER_SUPPLY_DIR, batt_name,
            file);
    if (size == -1) return -1;

    string_from_file(dest, full_path);
    free(full_path);
    return 0;
}

int read_battery_data_int(uint32_t *dest, char *batt_name, char* file) {
    char *full_path;
    int size = battery_file_path(&full_path, POWER_SUPPLY_DIR, batt_name,
            file);
    if (size == -1) return -1;

    uint_from_file(dest, full_path);
    free(full_path);
    return 0;
}

