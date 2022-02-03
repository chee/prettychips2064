// TODO move header into header

#include "screen.h"
#include "display.h"
#include "pretty_envelope.h"

AdsrScreen::AdsrScreen(PrettyEnvelope *env[8]) {
  for (int i = 0; i < 8; i++) {
	 envelopes[i] = env[i];
  }
}

void AdsrScreen::update_envelope() {
  for (auto envelope : envelopes) {
	 envelope->attack(adsr_x_map[attack]);
	 envelope->decay(adsr_x_map[decay]);
	 envelope->release(adsr_x_map[release]);
	 envelope->sustain(1 - sustain / y_max);
  }
}

void AdsrScreen::handle_lxy(int light, int xy) {
  // TODO figure out how to make this more general
  if (light == 0) {
	 if (attack > 1 && xy & LEFT) {
		attack -= 1;
	 }
	 if (attack < x_max && xy & RIGHT) {
		attack += 1;
	 }
  } else if (light == 1) {
	 if (decay > 1 && xy & LEFT) {
		decay -= 1;
	 }
	 if (decay < x_max && xy & RIGHT) {
		decay += 1;
	 }
	 if (sustain < y_max && xy & DOWN) {
		sustain += 1;
	 }
	 if (sustain > 0 && xy & UP) {
		sustain -= 1;
	 }
  } else if (light == 2) {
	 // NOTE: this one's flipped, because release comes in from the right
	 if (release > 1 && xy & RIGHT) {
		release -= 1;
	 }
	 if (release < x_max && xy & LEFT) {
		release += 1;
	 }
	 if (sustain < y_max && xy & DOWN) {
		sustain += 1;
	 }
	 if (sustain > 0 && xy & UP) {
		sustain -= 1;
	 }
  }
  if (xy != 0) {
	 is_dirty = true;
	 update_envelope();
  }
}

void AdsrScreen::draw(Display *display) {
  uint8_t sustain_width = SCREEN_WIDTH - attack - decay - release;
  display->fill(0xffff);
  uint16_t line_color = 0x0000;
  // attack line
  display->fatLine(0, SCREEN_HEIGHT, attack, 0, line_color);

  // decay line
  display->fatLine(attack, 0, attack + decay, sustain, line_color);

  // sustain line
  display->fatHLine(attack + decay, sustain, sustain_width, line_color);

  // release line
  display->fatLine(attack + decay + sustain_width, sustain, SCREEN_WIDTH - 1,
						 SCREEN_HEIGHT - 1, line_color);

  int circle_radius = 4;
  display->circle(attack, 0, circle_radius, RED_LIGHT);

  display->circle(attack + decay, sustain, circle_radius, YELLOW_LIGHT);
  display->circle(attack + decay + sustain_width, sustain, circle_radius,
						BLUE_LIGHT);

  is_dirty = false;
}

WaveScreen::WaveScreen(WaveformVoice *w[8]) {
  for (int i = 0; i < 8; i++) {
	 waves[i] = w[i];
  }
}

const int forms[6]{WAVEFORM_SINE,        WAVEFORM_SQUARE,
						 WAVEFORM_SAWTOOTH,    WAVEFORM_TRIANGLE,
						 WAVEFORM_SAMPLE_HOLD, WAVEFORM_SAWTOOTH_REVERSE};

void WaveScreen::handle_lxy(int light, int xy) {
  if (!xy) {
	 return;
  }
  if (light == 0) {
	 is_dirty = true;
	 int next = WAVEFORM_SINE;

	 if (xy & UP && xy & LEFT) {
		 next = WAVEFORM_SINE;
	 } else if (xy & UP && xy & RIGHT) {
		 next = WAVEFORM_SQUARE;
	 } else if (xy & DOWN && xy & LEFT) {
		 next = WAVEFORM_SAWTOOTH;
	 } else if (xy & DOWN && xy & RIGHT) {
		 next = WAVEFORM_TRIANGLE;
	 } else if (xy & LEFT) {
		 next = WAVEFORM_SAMPLE_HOLD;
	 }else if (xy & RIGHT) {
		 next = WAVEFORM_SAWTOOTH_REVERSE;
	 } else if (xy & UP) {
		 next = WAVEFORM_SQUARE;
	 } else if (xy & DOWN) {
		 next = WAVEFORM_PULSE;
	 }
	 for (auto wave : waves) {
		 wave->set_wave(next);
	 }
  }
}

void WaveScreen::draw(Display *d) {
  d->fill(0x0000);
  switch (waves[0]->wave_type) {
  case WAVEFORM_SAWTOOTH: {
	 d->fatLine(0, SCREEN_HEIGHT, 40, 0, RED_LIGHT);
	 d->fatLine(40, 0, 40, SCREEN_HEIGHT, RED_LIGHT);
	 d->fatLine(40, SCREEN_HEIGHT, 80, 0, RED_LIGHT);
	 d->fatLine(80, 0, 80, SCREEN_HEIGHT, RED_LIGHT);
	 d->fatLine(80, SCREEN_HEIGHT, 120, 0, RED_LIGHT);
	 d->fatLine(120, 0, 120, SCREEN_HEIGHT, RED_LIGHT);
	 d->fatLine(120, SCREEN_HEIGHT, 160, 0, RED_LIGHT);
	 d->fatLine(160, 0, 160, SCREEN_HEIGHT, RED_LIGHT);
	 break;
  }
  case WAVEFORM_SQUARE: {
	 d->fatLine(0, 0, 80, 0, RED_LIGHT);
	 d->fatLine(80, 0, 80, SCREEN_HEIGHT, RED_LIGHT);
	 d->fatLine(80, SCREEN_HEIGHT, 160, SCREEN_HEIGHT, RED_LIGHT);
	 break;
  }
  case WAVEFORM_TRIANGLE: {
	 d->triangleLine(40, 40, 60, 60, 40, 80, RED_LIGHT);
	 break;
  }
  case WAVEFORM_SINE: {
	 d->circleLine(40, 40, 40, RED_LIGHT);
	 break;
  }
  case WAVEFORM_SAMPLE_HOLD: {
	 d->roundRectLine(20, 20, 40, 40, 20, RED_LIGHT);
	 break;
  }
  case WAVEFORM_SAWTOOTH_REVERSE: {
	 d->fatLine(0, SCREEN_HEIGHT, 40, 0, RED_LIGHT);
	 d->fatLine(40, 0, 40, SCREEN_HEIGHT, RED_LIGHT);
	 d->fatLine(40, SCREEN_HEIGHT, 80, 0, RED_LIGHT);
	 d->fatLine(120, 0, 120, SCREEN_HEIGHT, RED_LIGHT);
	 d->fatLine(120, SCREEN_HEIGHT, 160, 0, RED_LIGHT);
	 d->fatLine(160, 0, 160, SCREEN_HEIGHT, RED_LIGHT);
	 break;
  }
  default: {
	 // TODO text
	 d->fatLine(20, 20, 40, 80, RED_LIGHT);
	 break;
  }
  }
  is_dirty = false;
}
