/**
 * 
 * 
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
 */
 Charger::Charger(int dex,ST77_Screen *sc, int inChargePin,int inVbaPin, int intCharginfPin, int inChargeDone) :  timer(POLLING_PERIOD)
 {
     
    index=dex;
    
    lowCurrentCounter=0;
     
    sensor219.begin();
    screen=sc;

    chargeCommandPin = inChargePin;      // D6 : Charge control, active Low
    vbatPin          = inVbaPin;     // a2 : vBAT
    chargingLedPin   = intCharginfPin;      // Active Low
    chargeDonePin    = inChargeDone;      // Active Low

    state=STATE_IDLE;
    pinMode(vbatPin,          INPUT);
    pinMode(chargeCommandPin, OUTPUT); // MOSFET command as output
    pinMode(chargingLedPin,   INPUT);  // "Charging" Led, dont let it be pullup now it will power up the TP4056
    pinMode(chargeDonePin,    INPUT);  // Same

    enableCharge(true); // send a charge pulse to reset the DW01
    delay(100);
    enableCharge(false);
    delay(1000); 
        
 }
 
 /**
  */
 void Charger::run(void)
 {
  float busVoltage = 0;
  float current = 0; // Measure in milli amps
  float power = 0;
  static int raw=0;
  busVoltage = sensor219.getBusVoltage_V();
  current = sensor219.getCurrent_mA();
  
  double rawBatVoltage=analogRead(vbatPin);
  rawBatVoltage=(rawBatVoltage*5000.)/1024.;
  int batVoltage=(int)rawBatVoltage;
  
  int volt=(int)(busVoltage*1000.);
  int charging=digitalRead(chargingLedPin);
  int charged=digitalRead(chargeDonePin);

  const char *stateName="xxx";
  ScreenState screenState;
  switch(state)
  {
    case STATE_IDLE:
        stateName="IDLE";
        enableCharge(false);
        screenState=ScreenState_Idle;
        if(batVoltage>2300)
        {
            lowCurrentCounter=0;
            state=STATE_CHARGING;    
             _batteryCurrentVoltage=volt;
            enableCharge(true);
            delay(100);
            
        }
        break;
    case STATE_CHARGING:
        stateName="CHARGING";
        screenState=ScreenState_Charging;
        if(current<2) // maybe disconnect or charge done
        {
            lowCurrentCounter++;
            if(lowCurrentCounter>3)
            {
                enableCharge(false);
                delay(300);
                _batteryCurrentVoltage = 1000.*sensor219.getBusVoltage_V();
                if(_batteryCurrentVoltage>4180) // done
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
                   state=STATE_IDLE;
                }
                screen->updateState(index,screenState);
                return;
            }
        }else
        {
            lowCurrentCounter=0;
        }
        if(timer.rdv())
        {
            // Switch off mostfet so we can get a reading
            enableCharge(false);
            delay(READ_DELAY);
            _batteryCurrentVoltage = 1000.*sensor219.getBusVoltage_V();
            enableCharge(true);
            timer.reset();
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
      screen->updateState(index,screenState);
  else
  {
      screen->updateStateCharging(index,voltageToPercent(_batteryCurrentVoltage),(int)current,_batteryCurrentVoltage);
  }
  
 
 }