/**
 * Manage one instance of a TP4056
 * GPL V2.1
    mean (c) 2017
 * 
 */

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "tp.h"
/**
 * \fn prettyPrint
 * \brief manually implement sprintf for floats
 * @param out
 * @param value
 */
void prettyPrint(char *out, float value)
{
    int m=(int)(value/1000.);
    value-=m*1000.;
    sprintf(out,"%d.%03d",(int)m,(int)value);
    
}
/**
 */
class voltageValue
{
public:
    const int voltage;
    const int percent;
};
/**
 *   These are dummy values for low voltage
 */
voltageValue conversionTable[]=
{
    {4200,100},
    {4150, 95},
    {4100, 90},
    {4050, 85},
    {4000, 80},
    {3950, 75},
    {3900, 70},
    {3800, 60},
    {3700, 50},
    {3450, 40},
    {3400, 30},
    {3350, 20},
    {3300, 10}
};

/**
 * 
 * @param volt
 * @return 
 */
int     Charger::voltageToPercent(int volt)
{
    int mx=sizeof(conversionTable)/sizeof(voltageValue);
    if(volt > conversionTable[0].voltage) return 100;
    if(volt < conversionTable[mx-1].voltage) return 0;
    for(int i=mx-1;i>=0;i--)
    {
        if(volt<conversionTable[i].voltage)
            return conversionTable[i].percent;
    }
    return 0;
}
/**
 * \fn enableCharge
 * \brief 
 */
void Charger::enableCharge(bool onoff)
{
   if(onoff) 
       digitalWrite(chargeCommandPin,LOW); // Send pulse to TP4056
   else
       digitalWrite(chargeCommandPin,HIGH); // Send pulse to TP4056
}

/**
 * 
 * @return 
 */
int Charger::getCurrent()
{
   return sensor-> getCurrent();
}
/**
 * 
 * @return 
 */
int Charger::getVoltage()
{
   return sensor->getVoltage();
}
/**
 * 
 */
 Charger::Charger(int dex,ST77_Screen *sc, int inChargePin,int inVbaPin,PowerBudget *bud, CurrentVoltageSensor *s) 
 :  timer(POLLING_PERIOD),stabilizing(STABILIZING_PERIOD)
 {
    sensor=s;
    
    budget=bud;
    index=dex;
    
    lowCurrentCounter=0;        
    screen=sc;

    chargeCommandPin = inChargePin;      // D6 : Charge control, active Low
    vbatPin          = inVbaPin;     // a2 : vBAT
    
    pinMode(vbatPin,          INPUT);
    pinMode(chargeCommandPin, OUTPUT); // MOSFET command as output
    
    goToStabilizing();    
 }
 /**
  * 
  * @param resetMe
  * @return 
  */
 bool Charger::reset(bool resetMe)
 {
      enableCharge(resetMe);
 }
 /**
  */
 void Charger::goToStabilizing(void)
 {
     state=STATE_STABILIZING;
     stabilizing.reset();
     
 }
 /**
  */
 void Charger::run(void)
 {
again:
  float busVoltage = 0;
  float current = 0; // Measure in milli amps
  float power = 0;
  static int raw=0;
   
  
  busVoltage = getVoltage();
  current = getCurrent();
  
  double rawBatVoltage=analogRead(vbatPin);
  rawBatVoltage=(rawBatVoltage*5000.)/1024.;
  int batVoltage=(int)rawBatVoltage;
  
  int volt=(int)(busVoltage*1000.);

  const char *stateName="xxx";
  ScreenState screenState;
  switch(state)
  {
    case STATE_STARTING:
        lowCurrentCounter=0;
        budget->setConsumption(index,1000); // by default we consume one amp
        state=STATE_CHARGING;    
        _batteryCurrentVoltage=volt;
        enableCharge(true);
        delay(100);
        goto again;
        break;
    case STATE_WAITING:
        stateName="WAITING";
        screenState=ScreenState_Waiting;
        if(batVoltage>2000) // actually ~ 2.5v with the diode
        {
            if(budget->askForCurrent(index,1000))
            {
                budget->setConsumption(index,1000);
                state=STATE_STARTING;   
                goto again;
            }
        }else
        {
             state=STATE_IDLE;   
             goto again;
        }
        break;
    case STATE_IDLE:
        stateName="IDLE";
        enableCharge(false);
        screenState=ScreenState_Idle;
#ifdef DEBUG
        char xx[20];
        sprintf(xx,"%d",batVoltage);
        screen->print(1,20*index,xx);
        return;
#endif
        if(batVoltage>2500)
        {
            Serial.println();
            Serial.print(batVoltage);
            Serial.println();            
            if(budget->askForCurrent(index,1000))
            {
                budget->setConsumption(index,1000);
                state=STATE_STARTING;   
                goto again;
            }else
            {
                 state=STATE_WAITING;   
                 goto again;
            }
        }
        break;
    case STATE_STABILIZING:
        screenState=ScreenState_Stabilizing;
        if(stabilizing.rdv())
        {
            state=STATE_IDLE;
            goto again;
        }
        break;
    case STATE_CHARGING:
        stateName="CHARGING";
        screenState=ScreenState_Charging;
        if(current<12) // maybe disconnect or charge done
        {
            lowCurrentCounter++;
            if(lowCurrentCounter>3)
            {
                enableCharge(false);
                delay(300);
                _batteryCurrentVoltage = getVoltage();
                if(_batteryCurrentVoltage>4100) // done
                {
                    screenState=ScreenState_Charged;
                    state=STATE_CHARGED;
                }else
                {
                    // Send a reset pulse
                   enableCharge(true);
                   delay(100);
                   enableCharge(false);
                   screenState=ScreenState_Idle;
                   goToStabilizing();
                }
                screen->updateState(index+1,screenState);
                return;
            }
        }else
        {
            lowCurrentCounter=0;
        }  
        budget->setConsumption(index,current); // by default we consume one amp
        if(timer.rdv())
        {
            // Switch off mostfet so we can get a reading
#ifndef RAW_REFRESH            
            enableCharge(false);
            delay(READ_DELAY);
            _batteryCurrentVoltage = getVoltage();
            enableCharge(true);
            timer.reset();
#else
            _batteryCurrentVoltage = 1000.*sensor219.getBusVoltage_V();
#endif            
        }     
        break;
    case STATE_CHARGED:
        screenState=ScreenState_Charged;
        enableCharge(false);
        break;
    case STATE_ERROR: 
        screenState=ScreenState_Error;
        enableCharge(false);
        break;        
  }
  if(state!=STATE_CHARGING)
      screen->updateState(index+1,screenState);
  else
  {
      screen->updateStateCharging(index+1,voltageToPercent(_batteryCurrentVoltage),(int)current,_batteryCurrentVoltage);
  }
  
 
 }