// echacont
// FLXMSynth_seq header file

#ifndef FLXMSYNTH_SEQ_H
#define FLXMSYNTH_SEQ_H

#include "defines.h"

#include <MsTimer2.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Fluxamasynth.h>

// controller
class controller
{
  private:
  potStatus_t potStatus, potStatus_prev;
  buttonStatus_t buttonStatus, buttonStatus_prev;
  // update values and return true if there was a change
  bool update(void);
};

// sequencer control the synth
// so it inherits Fluxamasynth
class sequencer : private Fluxamasynth
{
  private:
  const int ticksPerBeat    = 4;
  const int baseTempo       = 60;
  int voices = 8;
  int spread = 420;
  int panspread = 20;
  int tempo;
  // step sequence
  step_t seq[NUM_STEPS0];
  // sequencer
  Fluxamasynth synth;
};

// LCD display
// inherits LiquidCrystal_I2C 
class lcdisp : private LiquidCrystal_I2C
{
  private:
  LiquidCrystal_I2C lcd;
    //PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE
  
  void update(void);
};

// PHLXM - top system class
// inherits Fluxamasynth, LiquidCrystal_I2C classes
class PHLXM
{
  private:
  mode_e mode;
  transport_e trans;
  // controller
  controller contrl;
  // sequencer
  sequencer sq;
  // synthesizer
  
  // LCD
  lcdisp disp;

  public:
  PHLXM(void);

};

/*
// constantes en memoria
const int ticksPerBeat    = 4;
const int voices          = 8;
const int spread          = 420;
const int panspread       = 20;
const int baseTempo       = 60;

// state
int mode = 0;
int tempo = 120;        // BPM
potStatus_t potStatus_new, potStatus_prev;

// sequencer
bool steps0[NUM_STEPS0] = { true, true, true, true,
                            true, true, true, true,
                            true, true, true, true,
                            true, true, true, false  }; 

note_t seq0[NUM_STEPS0];

int  steps0_p = 0;
int  currentTicks = 0;

// scales
int root = 52;
int pitch = 52;
int scale0[7] = {2, 2, 1, 2, 2, 2, 1};     // major scale
int scale1[7] = {2, 1, 2, 2, 2, 1, 2};     // minor scale

Fluxamasynth synth;
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
*/

#endif //FLXMSYNTH_SEQ_H