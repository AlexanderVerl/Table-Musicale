#ifndef PLAYNOTEI2S_H
#define PLAYNOTEI2S_H

#include <Arduino.h>
#include <driver/i2s.h>

#define SAMPLE_RATE     (44100)
#define CHANNEL_NUM     (2)
#define BITS_PER_SAMPLE (16)
#define  baseFrequency  261.63 // Fréquence de la note Do (C)

void setupCustomI2S(int bckPin, int lrckPin, int dataOutPin);
void playNote(float frequency, int duration, int volumePercent);

#endif
