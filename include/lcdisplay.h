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
              fineStepSequence tseq); 
  void printPointer(int p, int i, int col, int row);
  void printStepAndPointer(int p, int8_t s, int i, int col, int row);
  // display menu functions
  void display_GEN(synthProgram_t, controllerMode_t, sequencerState_t);
  void display_PC(synthProgram_t, controllerMode_t);
  void display_HARM_MODE(controllerMode_t, sequencerState_t);
  void display_SEQ(controllerMode_t, sequencerState_t, fineStepSequence);
  void display_SEQ_MODE(controllerMode_t, sequencerState_t);
  void display_MIX(controllerMode_t, synthProgram_t);
  void display_CHORUS(controllerMode_t, synthProgram_t);
  void display_REVERB(controllerMode_t, synthProgram_t);
};

enum PC_opts_e { VPC } ;

#endif // LCDISPLAY_H