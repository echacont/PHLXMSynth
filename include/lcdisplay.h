// echacont
// PHLXM liquid crystal display header file

#ifndef LCDISPLAY_H
#define LCDISPLAY_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "defines.h"
#include "sequencer.h"

struct leds {
  bool status[NUM_LEDS];
  int  pin[NUM_LEDS];
};

typedef struct leds leds_t;

// Lcdisplay is derived from LiquidCrystal_I2C 
class Lcdisp : private LiquidCrystal_I2C
{
  private:
  public:
  Lcdisp(void);
  void update(void);
  void update(synthProgram_t program, 
              controllerMode_t mode, 
              controllerStatus_t status); // this is here just for debug
};

class Leds
{
  private:
  int  pin[NUM_LEDS];
  public:
  bool status[NUM_LEDS];
  // constructor
  Leds(void);
  void update(void);
  void update(bool leds[]);
  void update(int led, bool status);
};

enum PC_opts_e { VPC } ;

#endif // LCDISPLAY_H