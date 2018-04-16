#define _GNU_SOURCE

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "battery.h"
#include "util.h"

int battery_charge_now(battery_t *batt) {
    return read_battery_data_int(&batt->charge_now, batt->name, "charge_now");
}

int battery_charge_full(battery_t *batt) {
    return read_battery_data_int(&batt->charge_full, batt->name, "charge_full");
}

int battery_charge_status(battery_t *batt) {
    char charge_status[13];
    read_battery_data_str(charge_status, batt->name, "status");

    if (strcmp(charge_status, "Charging") == 0) {
        batt->charge_status = CHARGING;
    } else if (strcmp(charge_status, "Discharging") == 0) {
        batt->charge_status = DISCHARGING;
    } else if (strcmp(charge_status, "Full") == 0) {
        batt->charge_status = FULL;
    } else {
        return -1;
    }

    return 0;
}

int battery_current_now(battery_t *batt) {
    return read_battery_data_int(&batt->current_now, batt->name, "current_now");
}

int battery_current_avg(battery_t *batt) {
    return read_battery_data_int(&batt->current_avg, batt->name, "current_avg");
}

int battery_charge_full_design(battery_t *batt) {
    return read_battery_data_int(&batt->charge_full_design, batt->name, "charge_full_design");
}

int battery_cycle_count(battery_t *batt) {
    return read_battery_data_int(&batt->cycle_count, batt->name, "cycle_count");
}

uint32_t percent(uint32_t a, uint32_t b) {
    return (a * 100) / b;
}

/* Exported functions begin here */

int initialize_battery(battery_t *battery, char *name) {
    int status = 0;
    battery->name = name;
    status += battery_charge_now(battery);
    status += battery_charge_full(battery);
    status += battery_charge_full_design(battery);
    status += battery_cycle_count(battery);
    status += battery_charge_status(battery);
    status += battery_current_now(battery);
    status += battery_current_avg(battery);
    return status;
}

char *battery_status_as_string(battery_status status) {
    switch(status) {
        case CHARGING:      return "Charging";
        case DISCHARGING:   return "Discharging";
        case FULL:          return "Full";
        default:            return "Unknown";
    }
}

int time_remaining(char **time_left_str, battery_t batt) {
    float time_left;
    switch (batt.charge_status) {
        case CHARGING:
            time_left = ((float) batt.charge_full - (float) batt.charge_now) / (float) batt.current_avg;
            break;
        case DISCHARGING:
            time_left = ((float) batt.charge_now / (float) batt.current_avg);
            break;
        default:
            time_left = 0;
            break;
    }
    uint8_t hours_left = (uint8_t) time_left;
    uint8_t mins_left = (uint8_t) ((time_left - hours_left) * 60);
    return asprintf(time_left_str, "%02d:%02d", hours_left, mins_left);
}

uint32_t battery_health(battery_t batt) {
    return percent(batt.charge_full, batt.charge_full_design);
}

uint32_t charge_percent(battery_t batt) {
    return percent(batt.charge_now, batt.charge_full);
}

uint32_t abs_charge_percent(battery_t batt) {
    return percent(batt.charge_now, batt.charge_full_design);
}
