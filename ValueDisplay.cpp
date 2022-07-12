#include "ValueDisplay.h"

#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Fonts/FreeSans9pt7b.h"

void ValueDisplay::init()
{
   showValue(0);
}

char *hexString(byte value, char *buffer)
{
   sprintf(buffer, "0x%02X", value);
   buffer[4] = (byte)0;
   return buffer;
}

char *decString(byte value, char *buffer)
{
   sprintf(buffer, "%03i", value);
   buffer[3] = (byte)0;
   return buffer;
}

void ValueDisplay::drawHex(byte value)
{
   int16_t x1, y1;
   uint16_t w, h;

   char *hex = hexString(_oldValue, _stringBuffer);

   _display->setCursor(_xLeft, _hexY);
   _display->getTextBounds(_stringBuffer, _xLeft, _hexY, &x1, &y1, &w, &h);
   _display->fillRect(x1, y1, w, h, SSD1306_BLACK);

   hex = hexString(value, _stringBuffer);
   _display->print(hex);
}

void ValueDisplay::drawDec(byte value)
{
   int16_t x1, y1;
   uint16_t w, h;

   char *dec = decString(_oldValue, _stringBuffer);
   _display->getTextBounds(dec, _xLeft, _decY, &x1, &y1, &w, &h);
   _display->fillRect(x1, y1, w, h, SSD1306_BLACK);

   dec = decString(value, _stringBuffer);
   _display->setCursor(_xLeft, _decY);
   _display->print(dec);
}

void ValueDisplay::drawBin(byte value)
{
   _binaryDisplay->showValue(value);
}

ValueDisplay::ValueDisplay(int xLeft, int yTop, Adafruit_SSD1306 *display)
{
   _display = display;

   _xLeft = xLeft;
   _yTop = yTop;

   _display->setTextWrap(false);

   _display->setTextColor(SSD1306_WHITE);
   _display->setFont(&FreeSans9pt7b);
   _fontAdvance = FreeSans9pt7b.yAdvance;

   _hexY = _fontAdvance - 6;
   _decY = _fontAdvance * 2 - 6;

   _binaryDisplay = new BinaryDisplay(xLeft, _decY + 10, display);

   init();
}

void ValueDisplay::showValue(byte value)
{
   if (value != _oldValue)
   {
      drawHex(value);
      drawDec(value);
      drawBin(value);

      _display->display();

      _oldValue = value;
   }
}
