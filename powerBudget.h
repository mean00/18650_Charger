/**
 * GPL V2.1
    mean (c) 2017
 * 
 */
#pragma once

#define AD_MAX_UNIT 8

class PowerBudget
{
public:
                PowerBudget(int milliAmpsBuget);
        void    setConsumption(int dex, int consumption);
        bool    askForCurrent(int dex, int consumption);
protected:
        int     consumtionPerUnit[AD_MAX_UNIT];
        int     maxConsumption;
        int     totalConsumption;
  
};

