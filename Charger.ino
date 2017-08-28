
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
Charger     *charger[3];
PowerBudget  *budget;

/**
 */
void setup(void) 
{
  screen.setup();  
  budget=new PowerBudget(2000); // 2A budget
  charger[0]= new Charger(0,&screen,2,A2,budget);
  charger[1]= new Charger(1,&screen,3,A3,budget);
  charger[2]= new Charger(2,&screen,6,A6,budget);
  
}
/**
 */

/**
 * 
 */
void loop(void) 
{
    for(int i=0;i<3;i++)
        charger[i]->run();  
  delay(500);
}
// EOF

