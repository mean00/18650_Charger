
/**
 * Simple TP4056 + INA219 based circuit
 */

// This is needed for the ino/cmake stuff to work,
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "powerBudget.h"
// This is needed

#include "tp.h"
ST77_Screen screen;
Charger     *charger;
PowerBudget  *budget;

/**
 */
void setup(void) 
{
  screen.setup();  
  budget=new PowerBudget(2000); // 2A budget
  charger= new Charger(0,&screen,6,A2,budget);
  
}
/**
 */

/**
 * 
 */
void loop(void) 
{
  //screen.blank();
  charger->run();  
  delay(500);
}
// EOF

