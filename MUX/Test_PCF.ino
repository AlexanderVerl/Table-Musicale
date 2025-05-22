#include "PCF8575.h"

PCF8575 PCF(0x20);

int Capteurs;

void setup() {
  Serial.begin(9600);

  Wire.begin();  //Initialisation du bus I2C

  PCF.begin();
}


void loop() {
  Capteurs = PCF.read16();
  Serial.print("Capteurs =  ");
  Serial.println(Capteurs, BIN);
  delay(1000);
}
