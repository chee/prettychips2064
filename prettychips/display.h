#pragma once
#include <Adafruit_GFX.h>

class Display {
  Adafruit_GFX *tft;
public:
	 int handle_radius = 4;
  void use(Adafruit_GFX *t);
  void pixel(uint16_t x, uint16_t y, uint16_t color);
  void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
				uint16_t color);
  void fastVLine(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color);
  void fastHLine(uint8_t x0, uint8_t y0, uint8_t width, uint16_t color);
  void rectLine(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
					 uint16_t color);
  void rect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
  void circleLine(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
  void circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
  void roundRectLine(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
							uint16_t radius, uint16_t color);
  void roundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
					  uint16_t radius, uint16_t color);
  void triangleLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
						  uint16_t x2, uint16_t y2, uint16_t color);
  void triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2,
					 uint16_t y2, uint16_t color);
  void fill(uint16_t color);
  void fatLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
					uint16_t color);
  void fatHLine(uint16_t x0, uint16_t y0, uint16_t width, uint16_t color);

  void print(char *s);
};
