#define _GNU_SOURCE
#include <libnotify/notify.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "battery.h"

void usage() {
    printf("Usage: battery\n");
    printf("Get the current battery status formatted using Pango.\n");
    printf("\n");
    printf("Environment variables:\n");
    printf("   BLOCK_INSTANCE: The battery to report status for. If not\n");
    printf("      provided, BAT0 is assumed.\n");
}

int formatted_pango(battery_t *batt, char **string) {
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

    uint32_t percent = charge_percent(batt);

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
    int battery_str_len;
    switch (batt->charge_status) {
        case FULL:
            battery_str_len = asprintf(&battery_string, "%s %s", check_icon, battery_icon);
            break;
        case CHARGING:
            battery_str_len = asprintf(&battery_string, "%s", plug);
            break;
        case DISCHARGING:
            battery_str_len = asprintf(&battery_string,"%s", battery_icon);
            break;
        default:
            return -1;
    }
    if (battery_str_len == -1) return -1;

    // If the battery is reported to be full, just say it's 100%
    if (batt->charge_status == FULL) {
        percent = 100;
        battery_color = color_green;
    }

    int size = asprintf(string,
            "<span color=\"%s\" font_desc=\"Font Awesome\">%s </span>%d%%\n",
            (batt->charge_status == CHARGING) ? plug_color : battery_color,
            battery_string, percent);
    free(battery_string);
    return size;
}

void display_notification(char *time_left_str, char *helper_text) {
    notify_init ("Battery Status");
    NotifyNotification * battery_alert = notify_notification_new (helper_text, time_left_str, "dialog-information");
    notify_notification_show (battery_alert, NULL);
    g_object_unref(G_OBJECT(battery_alert));
    notify_uninit();
}

void display_batt_info_dialog(battery_t *batt, char *time_left) {
    GtkWidget* dialog = gtk_message_dialog_new(
            NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
            "Battery information - %s", batt->name);
    char *status_string = battery_status_as_string(batt->charge_status);
    gtk_message_dialog_format_secondary_text(
            GTK_MESSAGE_DIALOG(dialog),
            "Battery name:\t\t%s\n"
            "Battery charge:\t\t%d\n"
            "Charge when full:\t\t%d\n"
            "Design full:\t\t\t%d\n"
            "Cycle count:\t\t\t%d\n"
            "Charging status:\t\t%s\n"
            "Current now:\t\t\t%d\n"
            "Current avg:\t\t\t%d\n"
            "%% Charged:\t\t\t%d%%\n"
            "Time remaining:\t\t%s\n"
            "Battery health:\t\t%d%%\n",
            batt->name, batt->charge_now, batt->charge_full,
            batt->charge_full_design, batt->cycle_count, status_string,
            batt->current_now, batt->current_avg,
            charge_percent(batt), time_left, battery_health(batt));
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char **argv) {
    if (argc > 1) fprintf(stderr, "All arguments are ignored.\n");

    battery_t *battery = calloc(1, sizeof(battery_t));

    char *name = getenv("BLOCK_INSTANCE");
    if (!name || strcmp(name, "") == 0) {
        name = "BAT0";
    }

    if (initialize_battery(battery, name) != 0) {
        fprintf(stderr, "Unable to initialize battery");
        free(battery);
        return 1;
    }

    char *formatted_string;
    formatted_pango(battery, &formatted_string);

    int pid = fork();
    char *button = getenv("BLOCK_BUTTON");
    if (pid == 0 && button) {
        char *time_left;
        time_remaining(&time_left, battery);
        char *helper_text = (battery->charge_status == CHARGING) ? "Until full" : "Until empty";
        switch(button[0] - '0') {
            case 1: // Left click
                display_notification(time_left, helper_text);
                break;
            case 3: // Right click
                gtk_init(&argc, &argv);
                display_batt_info_dialog(battery, time_left);
                break;
        }
        free(time_left);
    }

    //For i3blocks to be really happy, print it twice
    if (pid != 0) {
        printf(formatted_string);
        printf(formatted_string);
    }

    // If current charge is <= 5%, status is urgent
    if (battery->charge_now <= 5) {
        free(battery);
        return 33;
    }

    free(formatted_string);
    free(battery);

    return 0;
}

