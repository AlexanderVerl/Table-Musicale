//Nom Prènom: Verlinskiy Alexander
//Nom de project: Table Musical
//Hardware: Esp 32, Max98357, PCF8575, HW-201
//Date:

// Librairies
#include "playnote.h"
#include "PCF8575.h"
#include <Adafruit_NeoPixel.h>

// Constantes
#define NEOPIXELPIN 15
#define NUMPIXELS 48
#define NUMCAPTEUR 16

// Objets
PCF8575 PCF(0x20);
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

//Variables globales
int Capteurs;

//////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  // while(!Serial); // ATTENTION BLOQUANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //delay(1500);
  Serial.println("Init project");
  // while(!Serial); // ATTENTION BLOQUANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!

  setupCustomI2S(1, 3, 19);  // pin CLK, LRC, data

  Wire.begin();  //Initialisation du bus I2C
  PCF.begin();
  pixels.begin();
}
//while(1);
//}

void loop() {
  //playNote(0, 250, 100);  // Jouer une note

  // playNote(500, 250, 50);  // Jouer une autre note
  //playNote(1000, 550, 50);  // frequency, duration ms, volume Percent)

  // playNote(500, 250, 50);

  // playNote(500, 1000, 100);

  /*  playNote(500, 250, 50);
  delay(100);
  playNote(0, 250, 100);
    delay(100);*/


  for (int i = 0; i < NUMCAPTEUR; i++) {
    Capteurs = PCF.read16();
    if ((Capteurs & (0b0000000000000001 << i)) == 0) {
      pixels.setPixelColor(i * 3, pixels.Color(0, 150, 0));
      pixels.setPixelColor((i * 3) + 1, pixels.Color(0, 150, 0));
      pixels.setPixelColor((i * 3) + 2, pixels.Color(0, 150, 0));
      pixels.show();
      playNote(500, 250, 50);
    } else {
      pixels.setPixelColor(i * 3, pixels.Color(0, 0, 0));
      pixels.setPixelColor((i * 3) + 1, pixels.Color(0, 0, 0));
      pixels.setPixelColor((i * 3) + 2, pixels.Color(0, 0, 0));
      pixels.show();
      playNote(0, 250, 100);
    }
  }


  /*
  Capteurs = PCF.read16();
  Serial.print("Capteurs =  ");
  Serial.println(Capteurs, BIN);
  delay(1000);
  */
}
