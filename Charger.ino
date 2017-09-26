
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
#include "sensor.h"
#include "SDL_Arduino_INA3221.h"

// This is needed

#include "tp.h"
ST77_Screen screen;
Charger     *charger[3];
PowerBudget  *budget;
SDL_Arduino_INA3221 *ina3221;

/**
 */
class inaSensor : public CurrentVoltageSensor
{
public:
    inaSensor(SDL_Arduino_INA3221 *na, int p)
    {
        ina=na;
        port=p;
    }
    int getVoltage()
    {
        return (int)(1000.*ina->getBusVoltage_V(port)); 
    }
    int getCurrent()
    {
         return -ina-> getCurrent_mA(port);
    }

protected:    
    int                 port;
    SDL_Arduino_INA3221 *ina;
};

/**
 */
void setup(void) 
{
  Serial.begin(57600);
  screen.setup();  
  Wire.begin();
  ina3221=new SDL_Arduino_INA3221(0x40,0.1); // equipped with 0R100 shunts
  ina3221->begin();
  budget=new PowerBudget(2000); // 2A budget
  charger[0]= new Charger(0,&screen,2,A2,budget,new inaSensor(ina3221,1));
  charger[1]= new Charger(1,&screen,3,A3,budget,new inaSensor(ina3221,2));
  charger[2]= new Charger(2,&screen,6,A6,budget,new inaSensor(ina3221,3));
  
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

