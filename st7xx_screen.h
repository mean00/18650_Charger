/**
 */
#pragma once

class Adafruit_ST7735;
class ST77_Screen
{
public:  
                    ST77_Screen();
                    void setup(void);
                    void blank(void);
                    void print(int y, const char *);
protected:  
  Adafruit_ST7735  *ptr;
  
};

