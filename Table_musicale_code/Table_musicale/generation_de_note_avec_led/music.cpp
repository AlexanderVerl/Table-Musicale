#include "music.h"
#include <math.h>

const i2s_port_t i2sPort = I2S_NUM_0;
bool i2sInitialized = false;         //only true or false
float sineWaveTable[WAVETABLE_SIZE];
volatile bool isPlaying = false;
float currentFrequency = 0;
int currentDuration = 0;
int currentVolume = 0;

void initSineTable() {
  for (int i = 0; i < WAVETABLE_SIZE; i++) {     //512 symbals
    sineWaveTable[i] = sin(2 * M_PI * i / WAVETABLE_SIZE);   //calculating angle 0-2Pi 
  }
}

void setupCustomI2S(int bckPin, int lrckPin, int dataOutPin) {
  initSineTable();

  i2s_config_t i2sConfig = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),              //.pour initialisation de structure 
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 16,
    .dma_buf_len = 1024,
    .use_apll = true,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pinConfig = {
    .bck_io_num = bckPin,
    .ws_io_num = lrckPin,
    .data_out_num = dataOutPin,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(i2sPort, &i2sConfig, 0, NULL);
  i2s_set_pin(i2sPort, &pinConfig);
  i2sInitialized = true;
}

void playNote(float frequency, int duration, int volumePercent) {
  if (!i2sInitialized) return;
  currentFrequency = frequency;
  currentDuration = duration;
  currentVolume = volumePercent;
  isPlaying = true;
}

void handleSound() {
  static size_t phase = 0;
  static size_t samplesSent = 0;
  static size_t totalSamples = currentDuration * SAMPLE_RATE / 1000;
  static int16_t samples[256 * 2];

  if (!isPlaying) return;

  size_t bytesToSend = sizeof(samples);
  for (size_t i = 0; i < 256; i++) {
    float sample = sineWaveTable[phase] * (currentVolume / 100.0f) * 0.5f;
    samples[i * 2] = samples[i * 2 + 1] = (int16_t)(sample * INT16_MAX);
    phase = (phase + (size_t)(WAVETABLE_SIZE * currentFrequency / SAMPLE_RATE)) % WAVETABLE_SIZE;
  }

  size_t bytesWritten;
  i2s_write(i2sPort, samples, bytesToSend, &bytesWritten, portMAX_DELAY);
  samplesSent += bytesWritten / (sizeof(int16_t) * 2);

  if (samplesSent >= totalSamples) {
    isPlaying = false;
    samplesSent = 0;
  }
}