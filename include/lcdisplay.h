// PHLXM liquid crystal display header file
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

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
              sequencerState_t state,
              extMIDIState_t midiState); 
  void Lcdisp::printPointer(int p, int i, int col, int row);
};

class Leds
{
  private:
  int  pin[NUM_LEDS];
  public:
  bool status[NUM_LEDS];
  // constructor
  Leds(void);
  void update(bool leds[]);
  void update(int led, bool status);
};

enum PC_opts_e { VPC } ;

#endif // LCDISPLAY_H