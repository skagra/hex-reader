
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "ValueDisplay.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
ValueDisplay *valueLeftDisplay;
ValueDisplay *valueRightDisplay;

#define CLOCK_INHIBIT_PIN 8  // CE pin 15
#define SERIAL_DATA_IN_PIN 9 // Q9 pin 9
#define CLOCK_PIN 10         // CP pin 2
#define LOAD_INHIBIT_PIN 11  // PL pin 1

#define PIN_DELAY_MICROS 5

void setup()
{
   Serial.begin(9600);

   // Setup 74HC165 pins
   pinMode(LOAD_INHIBIT_PIN, OUTPUT);
   pinMode(CLOCK_INHIBIT_PIN, OUTPUT);
   pinMode(CLOCK_PIN, OUTPUT);
   pinMode(SERIAL_DATA_IN_PIN, INPUT);

   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
   if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
   {
      Serial.println(F("SSD1306 allocation failed"));
      for (;;)
         ;
   }

   // Set up display
   display.clearDisplay();
   valueLeftDisplay = new ValueDisplay(0, 0, &display);
   valueRightDisplay = new ValueDisplay(64, 0, &display);
}

void latchValue()
{
   digitalWrite(LOAD_INHIBIT_PIN, LOW);
   delayMicroseconds(PIN_DELAY_MICROS);
   digitalWrite(LOAD_INHIBIT_PIN, HIGH);
   delayMicroseconds(PIN_DELAY_MICROS);
}

int16_t readByte()
{
   int16_t result = 0;

   // Ensure clock is high before getting data - else we'll miss the first bit
   digitalWrite(CLOCK_INHIBIT_PIN, HIGH);
   digitalWrite(CLOCK_PIN, HIGH);

   // Grab a byte
   digitalWrite(CLOCK_INHIBIT_PIN, LOW);
   result = shiftIn(SERIAL_DATA_IN_PIN, CLOCK_PIN, LSBFIRST) << 8;
   result |= shiftIn(SERIAL_DATA_IN_PIN, CLOCK_PIN, LSBFIRST);
   digitalWrite(CLOCK_INHIBIT_PIN, HIGH);

   return result;
}

void loop()
{
   latchValue();
   int16_t incoming = readByte();

   valueLeftDisplay->showValue((byte)(incoming >> 8));
   valueRightDisplay->showValue((byte)incoming);

   delay(500);
}
