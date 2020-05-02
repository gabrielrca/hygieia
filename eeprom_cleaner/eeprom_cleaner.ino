/*
 *Universidade Federal do Rio de Janeiro - UFRJ
 *Author: Gabriel Rodrigues Caldas de Aquino (gabriel@labnet.nce.ufrj.br) 
 *
 *About:
 *Use this code to clean the EEPROM with zero bytes.
 *After cleaning the EEPROM Arduino will turn on the builtin led.
*/

#include<EEPROM.h>

int EEPROM_Address=0;
void setup(){

 pinMode(LED_BUILTIN, OUTPUT);

 Serial.begin(9600);

 for (EEPROM_Address = 0; EEPROM_Address < EEPROM.length() ; EEPROM_Address++) {
   /* This for-block is used clean the EEPROM: it stores zeros on all EEPROM memory positions */
    EEPROM.write(EEPROM_Address, 0);
 }
EEPROM_Address=0;
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
}
