
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "ValueDisplay.h"
#include "Button.h"

#define CLOCK_INHIBIT_PIN 9   // CE pin 15
#define SERIAL_DATA_IN_PIN 10 // Q9 pin 9
#define CLOCK_PIN 11          // CP pin 2
#define LOAD_INHIBIT_PIN 12   // PL pin 1

#define RUN_PIN 3
#define SAMPLE_PIN 2

#define PIN_DELAY_MICROS 5

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

#define PULSE_SIZE 5

#define LOOP_DELAY_MILLIS 100
#define SAMPLE_DELAY_MILLIS 100

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
ValueDisplay *topLeftDisplay;
ValueDisplay *topRightDisplay;
ValueDisplay *bottomLeftDisplay;
ValueDisplay *bottomRightDisplay;

Button *runButton;
Button *sampleButton;

bool running = true;

void runCallback(void *clientData)
{
   running = !running;
}

void sampleCallback(void *clientData)
{
   sample();
}

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

   topLeftDisplay = new ValueDisplay(0, 0, &display);
   topRightDisplay = new ValueDisplay(SCREEN_WIDTH / 2, 0, &display);
   bottomLeftDisplay = new ValueDisplay(0, SCREEN_HEIGHT / 2, &display);
   bottomRightDisplay = new ValueDisplay(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, &display);

   runButton = new Button(RUN_PIN, runCallback, (void *)0);
   sampleButton = new Button(SAMPLE_PIN, sampleCallback, (void *)0);
}

byte getHighNibble(byte value)
{
   return (value >> 4) & 0xF;
}

byte getLowNibble(byte value)
{
   return value & 0xF;
}

byte makeByte(byte highNibble, byte lowNibble)
{
   return highNibble << 4 | (lowNibble & 0xF);
}

byte reverseNibbleBits(byte nibble)
{
   return (nibble & 0x1) << 3 |
          (nibble & 0x2) << 1 |
          (nibble & 0x4) >> 1 |
          (nibble & 0x8) >> 3;
}

void latchValue()
{
   digitalWrite(LOAD_INHIBIT_PIN, LOW);
   delayMicroseconds(PIN_DELAY_MICROS);
   digitalWrite(LOAD_INHIBIT_PIN, HIGH);
   delayMicroseconds(PIN_DELAY_MICROS);
}

void readValue(byte result[])
{
   // Ensure clock is high before getting data - else we'll miss the first bit
   digitalWrite(CLOCK_INHIBIT_PIN, HIGH);
   digitalWrite(CLOCK_PIN, HIGH);

   // Grab current values byte
   digitalWrite(CLOCK_INHIBIT_PIN, LOW);
   result[0] = shiftIn(SERIAL_DATA_IN_PIN, CLOCK_PIN, LSBFIRST);
   result[1] = shiftIn(SERIAL_DATA_IN_PIN, CLOCK_PIN, LSBFIRST);
   result[2] = shiftIn(SERIAL_DATA_IN_PIN, CLOCK_PIN, LSBFIRST);
   result[3] = shiftIn(SERIAL_DATA_IN_PIN, CLOCK_PIN, LSBFIRST);
   digitalWrite(CLOCK_INHIBIT_PIN, HIGH);
}

void drawPulse()
{
   display.fillRect(SCREEN_WIDTH - PULSE_SIZE - 2, SCREEN_HEIGHT - PULSE_SIZE - 2, PULSE_SIZE, PULSE_SIZE, SSD1306_WHITE);
   display.display();
}

void unDrawPulse()
{
   display.fillRect(SCREEN_WIDTH - PULSE_SIZE - 2, SCREEN_HEIGHT - PULSE_SIZE - 2, PULSE_SIZE, PULSE_SIZE, SSD1306_BLACK);
   display.display();
}

void sample()
{
   byte value[4];

   drawPulse();

   latchValue();
   readValue(value);

   topLeftDisplay->showValue(makeByte(getLowNibble(value[3]), reverseNibbleBits(getHighNibble(value[3]))));
   topRightDisplay->showValue(makeByte(getLowNibble(value[2]), reverseNibbleBits(getHighNibble(value[2]))));
   bottomLeftDisplay->showValue(makeByte(getLowNibble(value[1]), reverseNibbleBits(getHighNibble(value[1]))));
   bottomRightDisplay->showValue(makeByte(getLowNibble(value[0]), reverseNibbleBits(getHighNibble(value[0]))));

   delay(SAMPLE_DELAY_MILLIS);

   unDrawPulse();

   delay(SAMPLE_DELAY_MILLIS);
}

void loop()
{
   if (running)
   {
      sample();
   }
   else
   {
      sampleButton->tick();
   }

   runButton->tick();

   delay(LOOP_DELAY_MILLIS);
}
