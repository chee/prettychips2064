#include "waveform_voice.h"
#include "pretty_envelope.h"
#include "pretty_voice.h"
#include <Audio.h>

WaveformVoice::WaveformVoice() {
  wave = new AudioSynthWaveform;
  wave->pulseWidth(0.75);
  envelope = new PrettyEnvelope;
  wave_env_patch = new AudioConnection(*wave, *envelope);
}

WaveformVoice::~WaveformVoice() {
  delete wave;
  delete envelope;
  delete wave_env_patch;
}

double WaveformVoice::trans(int note) {
  return 440.0 * pow(2.0, (note - 69.0) / 12.0);
}

void WaveformVoice::play(byte p, byte velocity) {
  used = true;
  pitch = p;
  double freq = trans(pitch);
  AudioNoInterrupts();
  wave->begin(1.0, freq, wave_type);
  envelope->noteOn();
  AudioInterrupts();
}

void WaveformVoice::stop() {
  pitch = -1;
  used = false;
  envelope->noteOff();
}

int WaveformVoice::state() {
  if (envelope->state == PRETTY_ENVELOPE_STATE_IDLE) {
	 return PRETTY_VOICE_STATE_FREE;
  } else if (used == false) {
	 return PRETTY_VOICE_STATE_STEALABLE;
  } else {
	 return PRETTY_VOICE_STATE_USED;
  }
}

void WaveformVoice::set_wave(int wt) {
	wave_type = wt;
}

void WaveformVoice::set_wave(const int16_t *data, float maxFreq) {
	wave->arbitraryWaveform(data, maxFreq);
	wave_type = WAVEFORM_ARBITRARY;
}
