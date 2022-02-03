#pragma once
#define RED_LIGHT 0xf80e
#define YELLOW_LIGHT 0xffe0
#define BLUE_LIGHT 0x04BF
#define GREEN_LIGHT 0x8fe0
#define VIOLET_LIGHT 0x801f
#include "display.h"
#include "pretty_envelope.h"
#include "waveform_voice.h"

class Screen {
public:
  int lights_needed = 5;
  bool is_dirty = false;
  virtual void handle_lxy(int light, int xy);
  virtual void draw(Display *display);

protected:
  int width;
  int height;
};

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128

#define LEFT 1
#define RIGHT 2
#define UP 4
#define DOWN 8

const float adsr_x_map[50] = {0.0,
										0.41649312786339027,
										1.665972511453561,
										3.7484381507705122,
										6.663890045814244,
										10.412328196584756,
										14.993752603082049,
										20.408163265306122,
										26.655560183256977,
										33.73594335693461,
										41.649312786339024,
										50.39566847147022,
										59.975010412328196,
										70.38733860891296,
										81.63265306122449,
										93.7109537692628,
										106.62224073302791,
										120.36651395251978,
										134.94377342773845,
										150.35401915868388,
										166.5972511453561,
										183.67346938775512,
										201.5826738858809,
										220.32486463973345,
										239.90004164931278,
										260.3082049146189,
										281.5493544356518,
										303.62349021241147,
										326.53061224489795,
										350.2707205331112,
										374.8438150770512,
										400.249895876718,
										426.48896293211163,
										453.56101624323196,
										481.4660558100791,
										510.2040816326531,
										539.7750937109538,
										570.1790920449813,
										601.4160766347355,
										633.4860474802165,
										666.3890045814244,
										700.124947938359,
										734.6938775510205,
										770.0957934194086,
										806.3306955435236,
										843.3985839233653,
										881.2994585589338,
										920.033319450229,
										959.6001665972511,
										1000.0};

class AdsrScreen : public Screen {
public:
  PrettyEnvelope *envelopes[8];
  // ~⅓ of the screen for attack, decay and release
  uint8_t x_max = 49;
  uint8_t attack = 20;
  uint8_t decay = 20;
  uint8_t release = 20;
  int lights_needed = 3;

  uint8_t sustain = 64;
  float y_max = SCREEN_HEIGHT - 1;

  AdsrScreen(PrettyEnvelope *env[8]);
  void update_envelope();
  void handle_lxy(int light, int xy);
  void draw(Display *display);
};

class WaveScreen : public Screen {
public:
  WaveformVoice *waves[8];
  int lights_needed = 1;
  WaveScreen(WaveformVoice *w[8]);
  void handle_lxy(int light, int xy);
  void draw(Display *display);
};
