#include <Adafruit_Arcada.h>
#include <USB-MIDI.h>

#include <Adafruit_NeoPixel.h>

#include "pretty_envelope.h"
#include "pretty_voice.h"
#include "waveform_voice.h"

#include <Audio.h>
#include <AudioControl.h>
#include <AudioStream.h>

Adafruit_Arcada arcada;

#define NEOPIXEL_PIN 8
#define TOTAL_NUMBER_OF_LIGHTS 5

class Light {
public:
  int red;
  int green;
  int blue;
  Light(int r, int g, int b) {
	 red = r;
	 green = g;
	 blue = b;
  }
};

Light lights[5] = {Light(0xff, 0x00, 0x64), Light(0xff, 0xdc, 0x00),
						 Light(0x00, 0x96, 0xff), Light(0x28, 0xff, 0x00),
						 Light(0x64, 0x00, 0xff)};

USBMIDI_CREATE_DEFAULT_INSTANCE();

Adafruit_NeoPixel pixels(TOTAL_NUMBER_OF_LIGHTS, NEOPIXEL_PIN, NEO_GRB);

// Audio mixers can take 4 inputs
// Let's have 8 inputs in total for an 8-finger polyphonic synth
AudioMixer4 left_hand;
AudioMixer4 right_hand;

// mixers can take mixers as inputs, so let's create another to be sent to both
// speakers. pc2064v1 will be a polyphonic synth with monophonic output
AudioMixer4 mixer;
AudioConnection patch_left_hand(left_hand, 0, mixer, 0);
AudioConnection patch_right_hand(right_hand, 0, mixer, 1);

// The output is an audio stream too, it takes two inputs
AudioOutputAnalogStereo audioOut;
AudioConnection patch_left_ear(mixer, 0, audioOut, 0);
AudioConnection patch_right_ear(mixer, 0, audioOut, 1);

#include "display.h"
#include "screen.h"

enum class SlotChoice : uint8_t { wave, env };

enum class WaveChoice : uint8_t { sine, saw, string, something };

enum class EnvChoice : uint8_t { ar, adsr, ahdsr, dahdsr };

class PrettyMachine {
  // TODO move lights to the machine
  SlotChoice slot = SlotChoice::env;
  WaveChoice wave_choice = WaveChoice::sine;
  EnvChoice env_choice = EnvChoice::adsr;
  int current_light = 0;
  int active_lights = 5;
  int max_lights = 5;
  boolean _lights_changed = false;
  Screen *screen = nullptr;
  Display *display = nullptr;
  // 8 fingers
  WaveformVoice *voices[8];
  PrettyEnvelope *envelopes[8];
  // 8 voice->mixers
  AudioConnection *patches[8];
  // TODO implement this better by moving used voices to the end of the list
  int voice_counter = 0;

public:
  PrettyMachine() {}

  void begin() {
	  for (auto i : {0, 1, 2, 3}) {
		  mixer.gain(i, 0.8);
	  }
	 for (int i = 0; i < 8; i++) {
		voices[i] = new WaveformVoice();
		envelopes[i] = voices[i]->envelope;
		if (i < 4) {
		  patches[i] = new AudioConnection(*voices[i]->envelope, 0, left_hand, i);
		} else {
		  patches[i] =
				new AudioConnection(*voices[i]->envelope, 0, right_hand, i - 4);
		}
	 }
  }

  void set_waveform_type(int wt) {
	 for (auto voice : voices) {
			  voice->set_wave(wt);
	 }
  }

  void set_display(Display *d) { display = d; }

	// TODO move this voice selection into a voice container and be like `voice++`
  void noteOn(byte pitch, byte velocity) {
	 voice_counter += 1;
	 // get a free voice
	 for (auto voice : voices) {
		if (voice->state() == PRETTY_VOICE_STATE_FREE) {
		  voice->play(pitch, velocity);
		  voice->age = voice_counter;
		  return;
		}
	 }

	 // or a releasing voice
	 for (auto voice : voices) {
		if (voice->state() == PRETTY_VOICE_STATE_STEALABLE) {
		  voice->play(pitch, velocity);
		  voice->age = voice_counter;
		  return;
		}
	 }

	 // or the oldest voice
	 int oldest = infinity();
	 int choice = 7;
	 for (int i = 0; i < 8; i++) {
		if (voices[i]->age < oldest) {
		  oldest = voices[i]->age;
		  choice = i;
		}
	 }
	 voices[choice]->play(pitch, velocity);
	 voices[choice]->age = voice_counter;
  }

  void noteOff(byte pitch, byte velocity) {
	 for (auto wave : voices) {
		if (wave->pitch == pitch) {
		  wave->stop();
		}
	 }
  }

  void show(SlotChoice s) {
	  switch (s) {
	  case SlotChoice::env: {
		  show(env_choice);
		  return;
	  }
	  case SlotChoice::wave: {
		  show(wave_choice);
	  }
	  default:
		  show(wave_choice);
		  return;
	  }
  }

  void show(WaveChoice w) {
	  delete screen;
	  screen = new WaveScreen(voices);
	  active_lights = screen->lights_needed;
	  screen->draw(display);
  }

	void show(EnvChoice e) {
		delete screen;
		screen = new AdsrScreen(envelopes);
		active_lights = screen->lights_needed;
		screen->draw(display);
  }

  boolean screen_changed() { return screen->is_dirty; }

  boolean lights_changed() { return _lights_changed; }

  void handle_buttons(uint8_t buttons) {
	 if (buttons & ARCADA_BUTTONMASK_SELECT) {
		if (current_light == 0) {
		  current_light = active_lights - 1;
		} else {
		  current_light -= 1;
		}
		_lights_changed = true;
	 }

	 if (buttons & ARCADA_BUTTONMASK_START) {
		current_light += 1;
		if (current_light == active_lights) {
		  current_light = 0;
		}
		_lights_changed = true;
	 }

	 if (buttons & ARCADA_BUTTONMASK_B) {

		const char *choices[3] = {"wave", "adsr";
		uint8_t choice = arcada.menu(choices, 3, 0x0000, 0xffff);
		switch (choice) {
		case 0: {
			show(SlotChoice::wave);
			break;
		}
		case 1: {
			show(SlotChoice::env);
			break;
		}
		}
	 }

	 if (buttons & ARCADA_BUTTONMASK_A) {
		const char *choices[8] = {"sine", "sawtooth", "square",   "triangle",
										  "?",    "pulse",    "htootwas", "s&h"};
		uint8_t choice = arcada.menu(choices, 8, 0x0000, 0xffff);
		set_waveform_type(choice);
		screen->draw(display);
	 }
  }

  void handle_joy(int16_t x, int16_t y) {
	 int xy = 0;
	 if (x < -200) {
		xy |= LEFT;
	 } else if (x > 200) {
		xy |= RIGHT;
	 }
	 if (y < -200) {
		xy |= UP;
	 } else if (y > 200) {
		xy |= DOWN;
	 }
	 if (xy != 0) {
		screen->handle_lxy(current_light, xy);
	 }
  }

  void update_light_display();

  void update() {
	 if (lights_changed()) {
		update_light_display();
	 }
	 if (screen_changed()) {
		if (display != nullptr) {
		  screen->draw(display);
		}
	 }
  }

  void change(uint8_t buttons, int16_t x, int16_t y) {
	 handle_buttons(buttons);
	 handle_joy(x, y);
	 update();
  }
};

void PrettyMachine::update_light_display() {
  for (int i = 0; i < TOTAL_NUMBER_OF_LIGHTS; i++) {
	 Light light = lights[i];
	 int divisor = (i != current_light) + 1;
	 if (i >= active_lights) {
		pixels.setPixelColor(i, 0x0);
	 } else {
		uint32_t rgbcolor = pixels.gamma32(pixels.Color(
			 light.red / divisor, light.green / divisor, light.blue / divisor));
		pixels.setPixelColor(i, rgbcolor);
	 }

	 pixels.show();
  }
  _lights_changed = false;
}

Display display;
PrettyMachine machine;

void noteOn(byte channel, byte pitch, byte velocity) {
  machine.noteOn(pitch, velocity);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  machine.noteOff(pitch, velocity);
}

void setup() {
  arcada.arcadaBegin();
  arcada.filesysBeginMSD();
  arcada.displayBegin();
  arcada.setBacklight(255);
  arcada.drawBMP((char *)"icon.bmp", 0, 0);
  display.use(arcada.display);
  machine.set_display(&display);
  // TODO understand the number 18
  AudioMemory(18);
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();
  pixels.begin();
  pixels.setBrightness(180);
  machine.begin();
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleNoteOn(noteOn);
  MIDI.setHandleNoteOff(noteOff);
  machine.update_light_display();
  machine.show(SlotChoice::wave);
}

void loop() {
  arcada.readButtons();
  uint8_t buttons = arcada.justReleasedButtons();
  int16_t joyx = arcada.readJoystickX();
  int16_t joyy = arcada.readJoystickY();
  machine.change(buttons, joyx, joyy);
  MIDI.read();
}
