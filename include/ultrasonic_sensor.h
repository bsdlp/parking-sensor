#ifndef CONFIG_H
#define CONFIG_H

#define TRIG_PIN 4
#define ECHO_PIN 5
#define BUTTON_LED 0
#define BUTTON_PIN 1
#define SLEEP_DISTANCE_THRESHOLD 5
#define CONFIG_FILE "/config"

struct Config
{
	// 0-100 % brightness
	int tft_brightness_limit; // bl

	// threshold at which we consider car is parked
	int distance_threshold; // dt

	int display_sleep_time; // dst
};

#endif
