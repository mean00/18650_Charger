/**
 * Simple wrapper on top of u8glib for ST7735 screen
 * GPL v2.1
 * (c) mean 2017
 */
#pragma once

#define WIDTH  120
#define HEIGHT (160/4)


class Adafruit_ST7735;

enum ScreenState
{
  ScreenState_Idle=0,
  ScreenState_Error,
  ScreenState_Charging,
  ScreenState_Charged,
  ScreenState_Waiting,
  ScreenState_Stabilizing
};

class ST77_Screen
{
public:  
                    ST77_Screen();
                    void setup(void);
                    void blank(void);
                    void begin();
                    void end();
                    void print(int x,int y, const char *);
                    void updateState(int index,ScreenState s);
                    void updateStateCharging(int index, int percent, int amps,int mvolt);
                    
                    
protected:  
    int               width;
    int               height;
    Adafruit_ST7735  *ptr;
  
};

