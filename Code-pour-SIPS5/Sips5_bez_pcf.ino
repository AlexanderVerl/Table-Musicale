#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <driver/i2s.h>
#include <math.h>

// Pin definitions
#define SENSOR_PIN 34  // Pin for the HW-201 sensor output (object detection)
#define LED_PIN 15     // Pin for WS2812B LED strip data input
#define NUM_LEDS 8     // Number of LEDs in the strip
#define I2S_DOUT 19    // Pin for MAX98357 audio amplifier DIN
#define I2S_BCLK 1     // Pin for I2S Bit Clock
#define I2S_LRC 3      // Pin for I2S Left-Right Clock

#define SAMPLE_RATE 44100   // Audio sample rate (44.1 kHz)
#define AMPLITUDE 1000      // Audio amplitude for sine wave
#define FREQUENCY 440       // Frequency of the sine wave (440 Hz, which is A4 note)
#define AUDIO_BUFFER_SIZE 256 // Size of audio buffer

// Create LED strip object
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);

  // Initialize I2S audio output
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),   // Master mode, transmit data
    .sample_rate = SAMPLE_RATE,                             // Set sample rate to 44.1 kHz
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,           // 16-bit audio sample
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,           // Stereo audio (left and right channels)
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,     // Standard I2S format
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,               // Interrupt priority level
    .dma_buf_count = 8,                                     // Number of DMA buffers
    .dma_buf_len = 64,                                      // Length of each DMA buffer
    .use_apll = false                                       // Use PLL for audio clock (not used here)
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,  // Bit Clock Pin
    .ws_io_num = I2S_LRC,    // Left-Right Clock Pin
    .data_out_num = I2S_DOUT,// Data Out Pin (sending audio data)
    .data_in_num = I2S_PIN_NO_CHANGE // No input pin (not used here)
  };

  // Install and configure I2S driver
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config); // Set the pin configuration

  Serial.println("Setup completed.");

  pinMode(SENSOR_PIN, INPUT);
  pinMode(I2S_DOUT, OUTPUT);

  strip.begin();
  strip.show();  // Initialize all pixels to off
}

void loop() {
  int sensorValue = digitalRead(SENSOR_PIN);  // Read the sensor to detect if an object is present
  static int16_t audio_buffer[AUDIO_BUFFER_SIZE];  // Buffer for audio data
  static float phase = 0.0;  // Phase for sine wave generation

  // Generate a sine wave (440 Hz, 16-bit audio, 100ms buffer)
  for (int i = 0; i < AUDIO_BUFFER_SIZE; i++) {
    audio_buffer[i] = (int16_t)(AMPLITUDE * sin(phase));  // Calculate sine wave value
    phase += 2 * PI * FREQUENCY / SAMPLE_RATE;            // Increment phase for next sample
    if (phase >= 2 * PI) {                                 // Wrap phase value to stay within the sine wave cycle
      phase -= 2 * PI;
    }
  }

  if (sensorValue == LOW) {  // Assuming LOW means object detected
    Serial.println("Object detected!");
    playAudio(audio_buffer);  
    animateLEDs(); 
  } else {
    // If no object detected, stop sound and turn off LEDs
    strip.clear(); 
    strip.show();
    digitalWrite(I2S_DOUT, LOW);  // No audio output
  }
  
  delay(100);  // Small delay to debounce sensor input
}

void playAudio(int16_t* audio_buffer) {
  Serial.println("Playing audio...");
  size_t bytes_written;
  i2s_write(I2S_NUM_0, audio_buffer, sizeof(audio_buffer[0]) * AUDIO_BUFFER_SIZE, &bytes_written, portMAX_DELAY);
}

void animateLEDs() {
  Serial.println("Animating LEDs...");
  // Animate LEDs to show object detection
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 100, 20));  // Set LEDs to red color
    strip.show(); 
    delay(100);  
  }
  
  // Clear LEDs after animation
  strip.clear();
  strip.show();
}