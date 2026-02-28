/**
 * Battery monitoring utility for Raspberry Pi Pico W
 * Provides voltage reading, battery percentage calculation, and USB power detection
 */

#ifndef BATTERY_HPP
#define BATTERY_HPP

#include <cstdint>


#ifndef PICO_VSYS_PIN
#define PICO_VSYS_PIN 29
#endif

class Battery {
public:
    /**
     * Initialize the battery monitoring system
     * Must be called before using other methods
     * Note: For Pico W, cyw43_arch_init() must be called before this
     */
    void init();

    /**
     * Get the current system voltage
     * @return Voltage in volts, or -1.0f if unable to read
     */
    float get_voltage();

    /**
     * Get the battery percentage (0-100)
     * Only meaningful when running on battery power
     * @return Battery percentage 0-100, or -1 if unable to determine
     */
    int get_battery_percentage();

    /**
     * Check if the device is powered by USB (true) or battery (false)
     * @return true if USB powered, false if battery powered, or false if unable to determine
     */
    bool is_usb_powered();

    /**
     * Get a formatted string showing voltage and power source
     * Example: "4.2V USB" or "3.8V BAT"
     * @return Formatted string with voltage and power source
     */
    const char* get_status_string();

private:
    static constexpr float MIN_BATTERY_VOLTAGE = 3.6f;
    static constexpr float MAX_BATTERY_VOLTAGE = 4.2f;
    static constexpr int SAMPLE_COUNT = 3;
    static constexpr int PICO_FIRST_ADC_PIN = 26;
    
    char status_buffer[16];  // Buffer for status string
    
    /**
     * Internal method to read VSYS voltage using ADC
     * @return Raw voltage reading or -1.0f on error
     */
    float read_vsys_voltage();
};

#endif // BATTERY_HPP