#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include "battery.h"

int battery_charge_now(battery_t *batt) {
    char battery_charge_now_file_path[50] = {0};
    snprintf(battery_charge_now_file_path, 49,
            "/sys/class/power_supply/%s/charge_now", batt->name);
    FILE *battery_charge_now_file = fopen(battery_charge_now_file_path, "r");

    if (!battery_charge_now_file) return 1;

    uint32_t current_charge;
    fscanf(battery_charge_now_file, "%u", &current_charge);
    batt->charge_now = current_charge;
    return 0;
}

int battery_charge_full(battery_t *batt) {
    char battery_charge_full_file_path[50] = {0};
    snprintf(battery_charge_full_file_path, 49,
            "/sys/class/power_supply/%s/charge_full", batt->name);
    FILE *battery_charge_full_file = fopen(battery_charge_full_file_path, "r");

    if (!battery_charge_full_file) return 1;

    uint32_t charge_full;
    fscanf(battery_charge_full_file, "%u", &charge_full);
    batt->charge_full = charge_full;
    return 0;
}

int battery_charge_status(battery_t *batt) {
    char battery_status_file_path[50] = {0};
    snprintf(battery_status_file_path, 49,
            "/sys/class/power_supply/%s/status", batt->name);
    FILE *battery_status_file = fopen(battery_status_file_path, "r");

    if (!battery_status_file) return 1;

    char charge_status[10];
    fscanf(battery_status_file, "%s", charge_status);
    batt->charge_status = strcmp(charge_status, "Charging") == 0 ? CHARGING : DISCHARGING;
    if (strcmp(charge_status, "Charging") == 0) {
        batt->charge_status = CHARGING;
    } else if (strcmp(charge_status, "Discharging") == 0) {
        batt->charge_status = DISCHARGING;
    } else if (strcmp(charge_status, "Full") == 0) {
        batt->charge_status = FULL;
    } else {
        return 0;
    }

    return 0;
}

int battery_current_now(battery_t *batt) {
    char battery_current_now_file_path[50] = {0};
    snprintf(battery_current_now_file_path, 49,
            "/sys/class/power_supply/%s/current_now", batt->name);
    FILE *battery_current_now_file = fopen(battery_current_now_file_path, "r");

    if (!battery_current_now_file) return 1;

    uint32_t current_now;
    fscanf(battery_current_now_file, "%u", &current_now);
    batt->current_now = current_now;
    return 0;
}

int battery_current_avg(battery_t *batt) {
    char battery_current_avg_file_path[60] = {0};
    snprintf(battery_current_avg_file_path, 59,
            "/sys/class/power_supply/%s/current_avg", batt->name);
    FILE *battery_current_avg_file = fopen(battery_current_avg_file_path, "r");

    if (!battery_current_avg_file) return 1;

    uint32_t current_avg;
    fscanf(battery_current_avg_file, "%u", &current_avg);
    batt->current_avg = current_avg;
    return 0;
}

int battery_charge_full_design(battery_t *batt) {
    char battery_charge_full_design_file_path[60] = {0};
    snprintf(battery_charge_full_design_file_path, 59,
            "/sys/class/power_supply/%s/charge_full_design", batt->name);
    FILE *battery_charge_full_design_file = fopen(battery_charge_full_design_file_path, "r");

    if (!battery_charge_full_design_file) return 1;

    uint32_t charge_full_design;
    fscanf(battery_charge_full_design_file, "%u", &charge_full_design);
    batt->charge_full_design = charge_full_design;
    return 0;
}

int battery_cycle_count(battery_t *batt) {
    char battery_cycle_count_file_path[60] = {0};
    snprintf(battery_cycle_count_file_path, 59,
            "/sys/class/power_supply/%s/cycle_count", batt->name);
    FILE *battery_cycle_count_file = fopen(battery_cycle_count_file_path, "r");

    if (!battery_cycle_count_file) return 1;

    uint32_t cycle_count;
    fscanf(battery_cycle_count_file, "%u", &cycle_count);
    batt->cycle_count = cycle_count;
    return 0;
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
        case CHARGING:
            return "Charging";
        case DISCHARGING:
            return "Discharging";
        case FULL:
            return "Full";
        default:
            return "Unknown";
    }
}

int time_remaining(char **time_left_str, battery_t *batt) {
    // TODO: Find a better way to handle the FULL state. Maybe as "CHARGING"?
    float time_left;
    switch (batt->charge_status) {
        case CHARGING:
            time_left = ((float) batt->charge_full - (float) batt->charge_now) / (float) batt->current_avg;
            break;
        case DISCHARGING:
            time_left = ((float) batt->charge_now / (float) batt->current_avg);
            break;
        default:
            time_left = 0;
            break;
    }
    uint8_t hours_left = (uint8_t) time_left;
    uint8_t mins_left = (uint8_t) ((time_left - hours_left) * 60);
    return asprintf(time_left_str, "%02d:%02d", hours_left, mins_left);
}

uint32_t battery_health(battery_t *batt) {
    return batt->charge_full * 100 / batt->charge_full_design;
}

uint32_t charge_percent(battery_t *batt) {
    return batt->charge_now * 100 / batt->charge_full;
}

uint32_t abs_charge_percent(battery_t *batt) {
    return batt->charge_now * 100 / batt->charge_full_design;
}
