//generation_de_note_avec_led.ino
// Verlinskiy Alexander 6eme electronique 2024-25
//Projet: Table musical
//Hardware: esp32 + capteur infrorouge(hw-201) + MUX(pcf8575) + ampli calsse D(max98357)
//Date: 19/05/2025
//!!!!Code est écrit avec deepseek et analysé par moi!!!!

//Libreries
#include "music.h"
#include "PCF8575.h"
#include <Adafruit_NeoPixel.h>

//Constantes
#define NEOPIXELPIN 15
#define NUMPIXELS 48
#define NUMCAPTEUR 16

const float NOTES[] = {    //Les frequences des notes
  261.63, 293.66, 329.63,  // C4, D4, E4
  349.23, 392.00, 440.00,  // F4, G4, A4
  493.88, 523.25, 587.33,  // B4, C5, D5
  659.25, 698.46, 783.99,  // E5, F5, G5
  880.00, 987.77, 1046.50  // A5, B5, C6
};

PCF8575 PCF(0x20);
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  setupCustomI2S(1, 3, 19);  // CLK=1, LRC=3, DATA=19
  Wire.begin();
  PCF.begin();
  pixels.begin();
  pixels.setBrightness(100);
  Serial.println("System Ready");
}

void loop() {
  uint16_t capteurs = PCF.read16();

  // par chaque capteur on a la note+led
  for (int i = 0; i < NUMCAPTEUR; i++) {
    bool isActive = !(capteurs & (1 << i));
    uint32_t color = isActive ? pixels.Color(0, 150, 0) : pixels.Color(0, 0, 0);

    pixels.setPixelColor(i * 3, color);
    pixels.setPixelColor(i * 3 + 1, color);
    pixels.setPixelColor(i * 3 + 2, color);

    if (isActive && !isPlaying) {
      playNote(NOTES[i], 250, 150);  
    }
  }

  pixels.show();
  handleSound();
}