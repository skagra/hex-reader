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

void setup()
{
   Serial.begin(9600);

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

void loop()
{
   for (byte value = 0; value < 256; value++)
   {
      valueLeftDisplay->showValue(value);
      valueRightDisplay->showValue(255 - value);

      delay(255);
   }
}
