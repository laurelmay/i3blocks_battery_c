#include <stdint.h>

typedef enum battery_status {
    CHARGING, DISCHARGING, FULL
} battery_status;

typedef struct {
    char *name;
    uint32_t charge_now;
    uint32_t charge_full;
    battery_status charge_status;
    uint32_t current_now;
    uint32_t current_avg;
} battery_t;

int battery_charge_now   (battery_t *battery);
int battery_charge_full  (battery_t *battery);
int battery_charge_status(battery_t *battery);
int battery_current_now  (battery_t *battery);
int battery_current_avg  (battery_t *battery);

