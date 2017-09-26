/**
 */

#pragma once

/**
 */
class CurrentVoltageSensor
{
public:
  virtual int getVoltage()=0;
  virtual int getCurrent()=0;
  virtual ~CurrentVoltageSensor() {};
};