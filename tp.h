
#pragma once
#include "st7xx_screen.h"
#include "ad_timer.h"


#define READ_DELAY      5000 // ms when we cut off current and measure the battery
#define POLLING_PERIOD  (5*60) // Every 5mn s

enum State
{
    STATE_IDLE,
    STATE_CHARGING,
    STATE_CHARGED,
    STATE_ERROR
};


class Charger
{
public:
             Charger(ST77_Screen *sc, int inChargePin,int inVbaPin, int intCharginfPin, int inChargeDone);
        void run(void);
protected:
        
        ST77_Screen         *screen;
        State               state;
        Adafruit_INA219     sensor219; // Declare and instance of INA219
        Timer               timer;
        int chargeCommandPin;      // D6 : Charge control, active Low
        int vbatPin         ;     // a2 : vBAT
        int chargingLedPin  ;      // Active Low
        int chargeDonePin   ;      // Active Low                
        void enableCharge(bool onoff);
        
        float _batteryCurrentVoltage;

};