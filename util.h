#ifndef UTIL_H_
#define UTIL_H_

int read_battery_data_int(uint32_t *dest, char *batt_name, char* file);
int read_battery_data_str(char *dest, char *batt_name, char* file);
int battery_file_path(char **dest, char *path, char *battery_name, char *file);
int uint_from_file(uint32_t *a, char *path);
int string_from_file(char *string, char *path);

#endif //UTIL_H_
