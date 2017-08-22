/**
 * GPL V2.1
    mean (c) 2017
 * 
 */
#include "powerBudget.h"

/**
 */
PowerBudget::PowerBudget(int milliAmpsBuget)
{
    maxConsumption=milliAmpsBuget;
    for(int i=0;i<AD_MAX_UNIT;i++)
        consumtionPerUnit[i]=0;
    totalConsumption=0;
}
/**
 */
void    PowerBudget::setConsumption(int dex, int consumption)
{
    consumtionPerUnit[dex]=consumption;
    totalConsumption=0;
     for(int i=0;i<AD_MAX_UNIT;i++)
        totalConsumption+=consumtionPerUnit[i];
}
/**
 */
bool    PowerBudget::askForCurrent(int dex, int consumption)
{
    if(consumption+totalConsumption<maxConsumption)
        return true;
    else
        return false;
}
// EOF