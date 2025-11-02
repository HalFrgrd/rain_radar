
#include <chrono>
#include <cstdio>
#include <iomanip>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <time.h>


#include "battery.hpp"
#include "data_fetching.hpp"
#include "drivers/inky73/inky73.hpp"
#include "drivers/pcf85063a/pcf85063a.hpp"
#include "drivers/psram_display/psram_display.hpp"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/uart.h"
#include "hardware/watchdog.h"
#include "inky_frame_7.hpp"
#include "persistent_data.hpp"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "pico/types.h"
#include "pimoroni_common.hpp"
#include "rain_radar_common.hpp"
#include "secrets.h"
#include "wifi_setup.hpp"

using namespace pimoroni;

InkyFrame inky_frame;
void draw_error(InkyFrame &graphics, const std::string_view &msg)
{

    graphics.set_pen(Inky73::RED);
    graphics.rectangle(Rect(graphics.width / 3, graphics.height * 2 / 3, graphics.width / 3, graphics.height / 4));
    graphics.set_pen(Inky73::WHITE);
    graphics.text(msg, Point(graphics.width / 3 + 5, graphics.height * 2 / 3 + 5), graphics.width / 3 - 5, 2);
}

// void draw_lower_left_text(InkyFrame &graphics, const std::string_view &msg)
// {
//     // graphics.set_pen(Inky73::BLACK);
//     // graphics.rectangle(Rect(0, graphics.height - 25, graphics.width, 25));
//     graphics.set_pen(Inky73::WHITE);
//     graphics.text(msg, Point(5, graphics.height - 17), graphics.width / 2, 2);
// }

void draw_battery_status(InkyFrame &graphics, const char *status)
{
    graphics.set_pen(Inky73::WHITE);

    // Measure text width to right-align it
    int text_width = graphics.measure_text(status, 1);
    int x_position = graphics.width - text_width - 5; // 5 pixels from right edge

    graphics.text(status, Point(x_position, 5), graphics.width, 1); // Small text at top
}

datetime_t dt = {
    .year = 0,
    .month = 0,
    .day = 0,
    .dotw = 0,
    .hour = 0,
    .min = 0,
    .sec = 0,
};

int next_wakeup_min = 10;
int next_wakeup_hour = -1;


void draw_next_wakeup(InkyFrame &graphics, int hour, int minute)
{
    std::ostringstream oss;
    if (hour >= 0) {
        oss << "Next update at " << std::setfill('0') << std::setw(2) << hour << ":"
            << std::setfill('0') << std::setw(2) << minute;
    } else {
        int mins_to_wakeup;
        if (minute < dt.min) {
            mins_to_wakeup = (minute + 60) - dt.min;
        } else {
            mins_to_wakeup = minute - dt.min;
        }
        oss << "Next update in " << mins_to_wakeup << " min";
    }

    int text_width = graphics.measure_text(oss.str(), 1);

    graphics.set_pen(Inky73::WHITE);
    graphics.text(oss.str(), Point(graphics.width-60 - text_width, 5), graphics.width, 1);
}



std::pair<Err, std::string> run_app()
{

    persistent::PersistentData payload = persistent::read();

    ResultOr<int8_t> new_preferred_ssid_index = wifi_setup::wifi_connect(inky_frame, payload.wifi_preferred_ssid_index);
    if (!new_preferred_ssid_index.ok())
    {
        return {new_preferred_ssid_index.err, "WiFi connect failed"};
    }
    int8_t connected_ssid_index = new_preferred_ssid_index.unwrap();
    if (connected_ssid_index != payload.wifi_preferred_ssid_index)
    {
        payload.wifi_preferred_ssid_index = connected_ssid_index;
        printf("New preferred SSID index: %d\n", payload.wifi_preferred_ssid_index);
        persistent::save(&payload);
    }

    inky_frame.set_pen(Inky73::RED);
    inky_frame.clear();

    // fetching the image will write to the PSRAM display directly
    ResultOr<data_fetching::ImageHeader> const res = data_fetching::fetch_image(inky_frame, connected_ssid_index);
    if (!res.ok())
    {
        return {res.err, "Image fetch failed"};
    } else {
        data_fetching::ImageHeader image_header = res.unwrap();
        time_t update_ts = image_header.update_ts;
        struct tm *tm_info = gmtime(&update_ts);

        // Convert struct tm to datetime_t
        dt.year  = tm_info->tm_year + 1900; // tm_year is years since 1900
        dt.month = tm_info->tm_mon + 1;     // tm_mon is 0–11
        dt.day   = tm_info->tm_mday;        // 1–31
        dt.dotw  = tm_info->tm_wday;        // 0 = Sunday
        dt.hour  = tm_info->tm_hour;
        dt.min   = tm_info->tm_min;
        dt.sec   = tm_info->tm_sec;
        printf("Image timestamp: %04d-%02d-%02d %02d:%02d:%02d UTC\n",
            dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        inky_frame.rtc.set_datetime(&dt);
        next_wakeup_hour = image_header.next_wakeup_hours;
        next_wakeup_min = image_header.next_wakeup_minutes;
    }

    // points of interest
    for (const auto &poi : secrets::POINTS_OF_INTEREST_XY)
    {
        inky_frame.set_pen(Inky73::WHITE);
        inky_frame.circle(Point(poi[0], poi[1]), 3);
        inky_frame.set_pen(Inky73::RED);
        inky_frame.circle(Point(poi[0], poi[1]), 2);
    }

    // Initialize battery monitoring
    // MUST BE INITIALIZED AFTER WIFI SETUP ON PICO W
    // for some reason it needs cyw43_arch_init() to have been called first
    Battery battery;
    battery.init();
    const char *status = battery.get_status_string();
    printf("Battery status: %s\n", status);
    printf("%s", battery.is_usb_powered() ? "USB powered\n" : "Battery powered\n");
    draw_battery_status(inky_frame, status);

    return {Err::OK, ""};

}

int main()
{
    inky_frame.init();
    inky_frame.rtc.unset_alarm();
    inky_frame.rtc.clear_alarm_flag();
    inky_frame.rtc.unset_timer();
    inky_frame.rtc.clear_timer_flag();

    // get the rtc ticking
    inky_frame.rtc.set_datetime(&dt);

    stdio_init_all();
    sleep_ms(100);

    // Reducing system clocked resulted in wifi connection issues
    // I think the pico couldn't keep up with the data rate
    // Reduce CPU clock to 96 MHz to lower power consumption.
    // set_sys_clock_khz takes kHz and returns true on success.
    // const uint32_t target_khz = 96000;
    // if (!set_sys_clock_khz(target_khz, true)) {
    //     printf("Warning: failed to set system clock to %u kHz\n", target_khz);
    // } else {
    //     printf("System clock set to %u kHz\n", target_khz);
    // }

    InkyFrame::WakeUpEvent event = inky_frame.get_wake_up_event();
    printf("Wakup event: %d\n", event);

    auto [app_err, app_msg] = run_app();

    if (app_err != Err::OK) {
        std::string error_msg = std::string(app_msg) + " (" + std::string(errToString(app_err)) + ")";
        printf("Error: %s\n", error_msg.c_str());
        draw_error(inky_frame, error_msg);
    } else {
        next_wakeup_hour = -1;
        next_wakeup_min = (dt.min+1 + 10) / 10 * 10;
        if (next_wakeup_min >= 60)
        {
            next_wakeup_min -= 60;
        }
    }

    draw_next_wakeup(inky_frame, next_wakeup_hour, next_wakeup_min);

    if (wifi_setup::is_connected()) {
        wifi_setup::network_deinit(inky_frame);
    }
    
    inky_frame.update(true);

    printf("done!\n");

    inky_frame.sleep_until(-1, next_wakeup_min, next_wakeup_hour, -1);

    return 0;
}
