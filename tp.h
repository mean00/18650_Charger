/**
 
 */
#pragma once
#include "st7xx_screen.h"
#include "ad_timer.h"

// From time to time, we stop the charge to get the battery voltage
// So we stop for X second every Y seconds

#define READ_DELAY      5000 // ms when we cut off current and measure the battery
#define POLLING_PERIOD  (5*60) // Every 5mn s

enum State
{
    STATE_IDLE,
    STATE_CHARGING,
    STATE_CHARGED,
    STATE_ERROR
};

/**
 * \class Charger
 * \brief Small class than handles one chager
 * @param dex  : Index of that charger (0,1,...)
 * @param sc   : Screen , common
 * @param inChargePin     : Command pin to enable charge, drive mostfet gate
 * @param inVbaPin        : Analog pin to measure vbat, backup for INA when charge is off
 * @param intChargingPin  : Input pin, active low to notify the TP4056 is charging 
 * @param inChargeDone    : Input pin, active low to notify the TP4056 charge is done
 */

class Charger
{
public:
             Charger(int dex,ST77_Screen *sc, int inChargePin,int inVbaPin);
        void run(void);
protected:
        
        ST77_Screen         *screen;
        State               state;
        Adafruit_INA219     sensor219; // Declare and instance of INA219
        Timer               timer;
        int chargeCommandPin;      // D6 : Charge control, active Low
        int vbatPin         ;     // a2 : vBAT
        void    enableCharge(bool onoff);
        int     voltageToPercent(int volt);
        float _batteryCurrentVoltage;
        int   index;
        int   lowCurrentCounter;
        

};

// EOF