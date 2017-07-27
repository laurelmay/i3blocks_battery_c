#ifndef BATTERY_H_
#define BATTERY_H_

#include <stdint.h>

typedef enum battery_status {
    CHARGING, DISCHARGING, FULL
} battery_status;

typedef struct {
    char *name;
    uint32_t charge_now;
    uint32_t charge_full;
    uint32_t charge_full_design;
    uint32_t cycle_count;
    battery_status charge_status;
    uint32_t current_now;
    uint32_t current_avg;
} battery_t;

int initialize_battery (battery_t *battery, char *name);
char *battery_status_as_string (battery_status status);
int time_remaining (char **time_left_str, battery_t battery);
uint32_t battery_health (battery_t battery);
uint32_t charge_percent (battery_t battery);
uint32_t abs_charge_percent (battery_t battery);

#endif //BATTERY_H_
