#include "display.h"

void Display::use(Adafruit_GFX *t) {
  tft = t;
  tft->setCursor(0, 0);
  tft->setTextSize(2);
  tft->setTextColor(0xffff, 0x0000);
}
void Display::pixel(uint16_t x, uint16_t y, uint16_t color) {
  tft->drawPixel(x, y, color);
}
void Display::line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
						 uint16_t color) {
  tft->drawLine(x0, y0, x1, y1, color);
}
void Display::fastVLine(uint16_t x0, uint16_t y0, uint16_t length,
								uint16_t color) {
  tft->drawFastVLine(x0, y0, length, color);
}
void Display::fastHLine(uint8_t x0, uint8_t y0, uint8_t width, uint16_t color) {
  tft->drawFastHLine(x0, y0, width, color);
}
void Display::rectLine(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
							  uint16_t color) {
  tft->drawRect(x0, y0, w, h, color);
}
void Display::rect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
						 uint16_t color) {
  tft->fillRect(x0, y0, w, h, color);
}
void Display::circleLine(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
  tft->drawCircle(x0, y0, r, color);
}
void Display::circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
  tft->fillCircle(x0, y0, r, color);
}
void Display::roundRectLine(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
									 uint16_t radius, uint16_t color) {
  tft->drawRoundRect(x0, y0, w, h, radius, color);
}
void Display::roundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
								uint16_t radius, uint16_t color) {
  tft->fillRoundRect(x0, y0, w, h, radius, color);
}
void Display::triangleLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
									uint16_t x2, uint16_t y2, uint16_t color) {
  tft->drawTriangle(x0, y0, x1, y1, x2, y2, color);
}
void Display::triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
							  uint16_t x2, uint16_t y2, uint16_t color) {
  tft->fillTriangle(x0, y0, x1, y1, x2, y2, color);
}
void Display::fill(uint16_t color) { tft->fillScreen(color); }
void Display::fatLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
							 uint16_t color) {
  tft->drawLine(x0, y0, x1, y1, color);
  tft->drawLine(x0 - 1, y0 - 1, x1 - 1, y1 - 1, color);
  tft->drawLine(x0 + 1, y0 + 1, x1 + 1, y1 + 1, color);
  tft->drawLine(x0, y0, x1, y1, color);
}

void Display::fatHLine(uint16_t x0, uint16_t y0, uint16_t width,
							  uint16_t color) {
  tft->drawFastHLine(x0, y0, width, color);
  tft->drawFastHLine(x0, y0 - 1, width, color);
  tft->drawFastHLine(x0, y0 + 1, width, color);
}

void Display::print(char *s) {
  tft->setCursor(0, 0);
  tft->print(s);
}
