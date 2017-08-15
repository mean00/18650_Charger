
/**
 * Simple TP4056 + INA219 based circuit
 */

// This is needed for the ino/cmake stuff to work,
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
// This is needed

#include "tp.h"
ST77_Screen screen;
Charger     *charger;

/**
 */
void setup(void) 
{

  screen.setup();
  
  charger= new Charger(&screen,6,A2,2,4); 
  
  
}
/**
 */

/**
 * 
 */
void loop(void) 
{
  screen.blank();
  charger->run();  
  delay(500);
 
}
// EOF

