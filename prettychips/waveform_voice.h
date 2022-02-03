#pragma once

#include <Audio.h>
#include "pretty_voice.h"
#include "pretty_envelope.h"

class WaveformVoice {
	AudioConnection *wave_env_patch;
public:
	AudioSynthWaveform *wave;
	PrettyEnvelope *envelope;
	int wave_type = WAVEFORM_SAWTOOTH;
	boolean used = false;
	byte pitch = -1;
	int age = 0;
	WaveformVoice();
	~WaveformVoice();
	void play(byte p, byte velocity);
	void stop();
	double trans(int note);
	void set_wave(int wt);
	void set_wave(const int16_t *data, float maxFreq);
	int state();
};
