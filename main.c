#define _GNU_SOURCE
#include <libnotify/notify.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "battery.h"

#define PERCENT(X,Y) (X) * 100 / (Y)

void usage() {
    printf("Usage: battery\n");
    printf("Get the current battery status formatted using Pango.\n");
    printf("\n");
    printf("Environment variables:\n");
    printf("   BLOCK_INSTANCE: The battery to report status for. If not\n");
    printf("      provided, BAT0 is assumed.\n");
}

char *formatted_pango(battery_t *batt) {
    // FontAwesome Symbols         CODE        SYMBOL
    char *plug                  = "\uF1E6"; // 
    char *check_icon            = "\uF00C"; // 
    char *full_battery          = "\uF240"; // 
    char *three_quarter_battery = "\uF241"; // 
    char *half_battery          = "\uF242"; // 
    char *quarter_battery       = "\uF243"; // 
    char *empty_battery         = "\uF244"; // 

    // Colors
    char *color_green  = "#859900";
    char *color_cyan   = "#2AA198";
    char *color_yellow = "#B58900";
    char *color_orange = "#CB4B16";
    char *color_red    = "#DC322F";

    uint32_t percent = PERCENT(batt->charge_now, batt->charge_full);

    // variables for the remainder of formatting
    char *battery_icon, *battery_color, *plug_color;
    if (percent <= 10) {
        battery_icon = empty_battery;
        battery_color = color_red;
    } else if (percent <= 35) {
        battery_icon = quarter_battery;
        battery_color = color_orange;
    } else if (percent <= 60) {
        battery_icon = half_battery;
        battery_color = color_yellow;
    } else if (percent <= 85) {
        battery_icon = three_quarter_battery;
        battery_color = color_cyan;
    } else {
        battery_icon = full_battery;
        battery_color = color_green;
    }

    // The Magsafe adapter turns green at around 95%
    plug_color = (percent < 95) ? color_orange : color_green;

    char *battery_string;
    switch (batt->charge_status) {
        case FULL:
            asprintf(&battery_string, "%s %s", check_icon, battery_icon);
            break;
        case CHARGING:
            asprintf(&battery_string, "%s", plug);
            break;
        case DISCHARGING:
            asprintf(&battery_string,"%s", battery_icon);
            break;
        default:
            return malloc(sizeof(char));
    }

    // If the battery is reported to be full, just say it's 100%
    if (batt->charge_status == FULL) percent = 100;

    char *formatted_string;
    asprintf(&formatted_string,
            "<span color=\"%s\" font_desc=\"Font Awesome 5 Regular\">%s </span>%d%%%%\n",
            (batt->charge_status == CHARGING) ? plug_color : battery_color,
            battery_string,
            percent);
    free(battery_string);
    return formatted_string;
}

void display_notification(char *time_left_str, char *helper_text) {
    notify_init ("Battery Status");
    NotifyNotification * battery_alert = notify_notification_new (helper_text, time_left_str, "dialog-information");
    notify_notification_show (battery_alert, NULL);
    g_object_unref(G_OBJECT(battery_alert));
    notify_uninit();
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

int main(int argc, char **argv) {
    if (argc > 1) fprintf(stderr, "All arguments are ignored.\n");

    battery_t *battery = calloc(1, sizeof(battery_t));

    battery->name = getenv("BLOCK_INSTANCE");
    if (!battery->name || strcmp(battery->name, "") == 0) {
        battery->name = "BAT0";
    }

    if (battery_charge_now(battery)) {
        fprintf(stderr, "Unable to get current charge stats.");
        free(battery);
        return 1;
    }
    if (battery_charge_full(battery)) {
        fprintf(stderr, "Unable to get full charge info.");
        free(battery);
        return 2;
    }
    if (battery_charge_status(battery)) {
        fprintf(stderr, "Unable to get charging status.");
        free(battery);
        return 3;
    }
    if (battery_current_now(battery)) {
        fprintf(stderr, "Unable to get current info.");
        free(battery);
        return 4;
    }
    if (battery_current_avg(battery)) {
        fprintf(stderr, "Unable to get avg current info.");
        free(battery);
        return 5;
    }

    char *button = getenv("BLOCK_BUTTON");
    if (button && button[0] - '0' == 1) {
        if (battery->charge_status != FULL) {
            char *time_left;
            time_remaining(&time_left, battery);
            char *helper_text = (battery->charge_status == CHARGING) ? "Until full" : "Until empty";
            display_notification(time_left, helper_text);
            free(time_left);
        } else {
            display_notification("", "Battery full");
        }
    }

    char *formatted_string = formatted_pango(battery);

    //For i3blocks to be really happy, print it twice
    printf(formatted_string);
    printf(formatted_string);

    // If current charge is <= 5%, status is urgent
    if (battery->charge_now <= 5) {
        free(battery);
        return 33;
    }

    free(formatted_string);
    free(battery);

    return 0;
}

