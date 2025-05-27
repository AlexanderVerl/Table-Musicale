#ifndef PLAYNOTEI2S_H
#define PLAYNOTEI2S_H

#include <Arduino.h>
#include <driver/i2s.h>

extern const i2s_port_t i2sPort;

#define SAMPLE_RATE (44100)
#define CHANNEL_NUM (2)
#define BITS_PER_SAMPLE (16)
#define WAVETABLE_SIZE 512

extern float sineWaveTable[WAVETABLE_SIZE];
extern volatile bool isPlaying;
extern float currentFrequency;
extern int currentDuration;
extern int currentVolume;

void setupCustomI2S(int bckPin, int lrckPin, int dataOutPin);
void playNote(float frequency, int duration, int volumePercent);
void handleSound();

#endif