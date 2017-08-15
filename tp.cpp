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

void prettyPrint(char *out, float value)
{
    int m=(int)(value/1000.);
    value-=m*1000.;
    sprintf(out,"%d.%03d",(int)m,(int)value);
    
}

/**
 */
void Charger::enableCharge(bool onoff)
{
   if(onoff) 
       digitalWrite(chargeCommandPin,LOW); // Send pulse to TP4056
   else
       digitalWrite(chargeCommandPin,HIGH); // Send pulse to TP4056
}

/**
 */
 Charger::Charger(ST77_Screen *sc, int inChargePin,int inVbaPin, int intCharginfPin, int inChargeDone) :  timer(POLLING_PERIOD)
 {
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
        delay(1000); // Let it discharge
        
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
  switch(state)
  {
    case STATE_IDLE:
        stateName="IDLE";
        if(batVoltage>2300)
        {
            state=STATE_CHARGING;            
            enableCharge(true);
            timer.reset();
        }
        break;
    case STATE_CHARGING:
        stateName="CHARGING";
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
    case STATE_ERROR: 
        stateName="Done";
        enableCharge(false);
        break;        
  }
  
  
  char buffer[60],buffer2[20],buffer3[20];
  
  screen->print(8,stateName);
  prettyPrint(buffer2,volt);
  prettyPrint(buffer3,batVoltage);
  sprintf(buffer,"A:%sv-B:%sv",buffer2,buffer3);
  screen->print(28,buffer);
  sprintf(buffer,"%d: %d-%d",(int)(current),charging,charged);
  screen->print(56,buffer);
  
  prettyPrint(buffer2,_batteryCurrentVoltage);
  screen->print(70,buffer2);
     
 }