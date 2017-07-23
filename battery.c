#define _POSIX_C_SOURCE 200809L
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
    char battery_current_avg_file_path[50] = {0};
    snprintf(battery_current_avg_file_path, 49,
            "/sys/class/power_supply/%s/current_avg", batt->name);
    FILE *battery_current_avg_file = fopen(battery_current_avg_file_path, "r");

    if (!battery_current_avg_file) return 1;

    uint32_t current_avg;
    fscanf(battery_current_avg_file, "%u", &current_avg);
    batt->current_avg = current_avg;
    return 0;
}

