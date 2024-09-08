#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "ultrasonic_sensor.h"

TFT_eSPI tft = TFT_eSPI();

long duration;
int distance;

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

void setup(void)
{
	pinMode(TRIG_PIN, OUTPUT);
	pinMode(ECHO_PIN, INPUT);
	Serial.begin(9600);

	tft.init();
	tft.setRotation(2);
	tft.fillScreen(TFT_BLACK);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.setTextSize(2);
	set_tft_brightness(80);
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
	Serial.print("Distance: ");
	Serial.println(distance);

	printRightAligned("Distance:", distance, 100, 100);
	delay(300);
}
