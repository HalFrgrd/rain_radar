#pragma once

#include "rain_radar_common.hpp"
#include <string>
#include "inky_frame_7.hpp"
#include "pico/types.h"

namespace data_fetching
{
    const uint32_t MAGIC_NUMBER = 0x425A5252; // "BZRR"
    struct ImageHeader
    {
        int64_t update_ts;
        uint32_t magic_number;
        int8_t next_wakeup_hours;
        int8_t next_wakeup_minutes;
    };

    ResultOr<ImageHeader> fetch_image(pimoroni::InkyFrame &inky_frame, int8_t connected_ssid_index);
    
}
