#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/flash.h" // for the flash erasing and writing
#include "hardware/sync.h"  // for the interrupts

#define FLASH_TARGET_OFFSET (1800 * 1024) // choosing to start at 1.8MB into flash, so we don't overwrite the program

namespace persistent
{

    struct PersistentData
    {
        int8_t wifi_preferred_ssid_index; // index into the known SSIDs array
    };

    void save(PersistentData *myData)
    {
        if (!myData)
            return;

        uint8_t *myDataAsBytes = (uint8_t *)myData;
        size_t myDataSize = sizeof(*myData);

        printf("Programming flash target region...\n");

        uint32_t interrupts = save_and_disable_interrupts();
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_PAGE_SIZE);             // erase 1 page
        flash_range_program(FLASH_TARGET_OFFSET, myDataAsBytes, myDataSize); // write struct
        restore_interrupts(interrupts);

        printf("Done.\n");
    }

    PersistentData read()
    {
        PersistentData myData{
            .wifi_preferred_ssid_index = 0};
        const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
        memcpy(&myData, flash_target_contents, sizeof(myData));
        return myData;
    }

}
