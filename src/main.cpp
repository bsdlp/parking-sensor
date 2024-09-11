#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <LittleFS.h>
#include <limits>
#include "ultrasonic_sensor.h"

// LittleFSConfig fsCfg;
TFT_eSPI tft = TFT_eSPI();
Config config;

long duration;
int distance;
bool displayOn;

// uses PWM to set brightness between 0 and 100%
void set_tft_brightness(uint8_t Value)
{
	if (Value < 0 || Value > 100)
	{
		printf("TFT_SET_BL Error \r\n");
	}
	else
	{
		analogWrite(TFT_BL, Value * 2.55);
	}
}

// Function to print a right-aligned string with space padding
void printRightAligned(const char *label, int number, int x, int y)
{
	char buffer[16];								// Buffer for formatted text
	sprintf(buffer, "%5d", number); // Format number with 5 width, space-padded if needed

	tft.setCursor(x, y); // Set cursor to the desired position
	tft.print(label);		 // Print the label

	// Calculate the position to print the number right-aligned
	int labelWidth = tft.textWidth(label);	 // Get the width of the label
	int numberWidth = tft.textWidth(buffer); // Get the width of the number
	int totalWidth = labelWidth + numberWidth;

	// Calculate the x position for right alignment within a fixed width
	int availableWidth = tft.width() - x;			 // Total available width from x to screen edge
	int offsetX = availableWidth - totalWidth; // Calculate offset to fit label + number

	tft.setCursor(x + offsetX + labelWidth, y); // Move cursor to the right-aligned position
	tft.print(buffer);													// Print the number
}

void setupFs(void)
{
	/*
	fsCfg.setAutoFormat(true);
	LittleFS.setConfig(fsCfg);
	*/
	if (!LittleFS.begin())
	{
		Serial.println("An Error has occurred while mounting LittleFS");
		return;
	}

	/*
	 if (!SPIFFS.begin())
	 {
		 Serial.println("formatting file system");

		 SPIFFS.format();
		 if (!SPIFFS.begin())
		 {
			 Serial.println("error formatting file system");
		 }
	 }
	*/
}

void readConfigOrSetDefaults()
{
	Serial.println("Reading config file");
	File cfgFile;
	cfgFile = LittleFS.open(CONFIG_FILE, "r");
	if (!cfgFile)
	{
		cfgFile = LittleFS.open(CONFIG_FILE, "w");
		cfgFile.println("bl=100");
		cfgFile.println("dt=20");
		cfgFile.println("dst=9999");
		cfgFile.flush();
		cfgFile.close();
		cfgFile = LittleFS.open(CONFIG_FILE, "r");
	}

	String key;
	String value;
	while (cfgFile.available())
	{
		Serial.println("reading config file line");
		key = cfgFile.readStringUntil('=');
		value = cfgFile.readStringUntil('\n');
		if (key == "bl")
		{
			config.tft_brightness_limit = value.toInt();
		}
		else if (key == "dt")
		{
			config.distance_threshold = value.toInt();
		}
		else if (key == "dst")
		{
			config.display_sleep_time = value.toInt();
		}
		Serial.printf("config key: %s, value: %s\n", key.c_str(), value.c_str());
	}
	cfgFile.close();
}

void setupDisplay(void)
{
	tft.init();
	tft.setRotation(2);
	tft.fillScreen(TFT_BLACK);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.setTextSize(2);
	set_tft_brightness(config.tft_brightness_limit);
	displayOn = true;
}

void setup(void)
{
	Serial.begin(9600);
	delay(10000);

	setupFs();
	readConfigOrSetDefaults();

	pinMode(BUTTON_LED, OUTPUT);
	pinMode(BUTTON_PIN, INPUT_PULLDOWN);
	pinMode(TRIG_PIN, OUTPUT);
	pinMode(ECHO_PIN, INPUT);
	analogWrite(BUTTON_LED, 255);

	setupDisplay();
}

// on button press, we set the current distance as the new threshold and save it to fs
void handleButtonPress()
{
	if (digitalRead(BUTTON_PIN))
	{
		if (displayOn)
		{
			analogWrite(TFT_BL, 0);
			displayOn = false;
		}
		else
		{
			analogWrite(TFT_BL, config.tft_brightness_limit * 2.55);
			displayOn = true;
		}
	}
}

void loop()
{
	// clear trigger pin on sonar
	digitalWrite(TRIG_PIN, LOW);
	delayMicroseconds(2);

	// send a 10 microsecond pulse to trigger pin
	digitalWrite(TRIG_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIG_PIN, LOW);

	duration = pulseIn(ECHO_PIN, HIGH);
	distance = (duration * .0343) / 2;

	// if the distance has changed, then wake up the display

	// if the distance hasn't changed since display_sleep_time, then turn off the display

	Serial.print("Distance: ");
	Serial.println(distance);

	handleButtonPress();

	Serial.printf("config: bl=%d, dt=%d, dst=%d\n", config.tft_brightness_limit, config.distance_threshold, config.display_sleep_time);

	printRightAligned("Distance:", distance, 100, 100);
	delay(300);
}
