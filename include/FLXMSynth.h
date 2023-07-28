// echacont
// FLXMSynth_seq header file

#ifndef FLXMSYNTH_SEQ_H
#define FLXMSYNTH_SEQ_H

#include "defines.h"

// constantes en memoria
const int ticksPerBeat    = 4;
const int voices          = 8;
const int spread          = 420;
const int panspread       = 20;
const int baseTempo       = 60;

struct note {
  int pitch;
  int velocity;
};

struct potStatus {
  bool status[NUM_POTS];
  int  value[NUM_POTS];
};

struct buttonStatus {
  bool status[NUM_BUTTONS];
  bool value[NUM_BUTTONS];
};

typedef struct note note_t;
typedef struct potStatus potStatus_t;
typedef struct buttonStatus buttonStatus_t;

struct step {
  int nextStep;
  bool enable;
  int gate;
  note_t notes[];
};

typedef struct step step_t;

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


#include <MsTimer2.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Fluxamasynth.h>
Fluxamasynth synth;
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

// controller
class controller
{
  private:
  potStatus_t potStatus, potStatus_prev;
  buttonStatus_t buttonStatus, buttonStatus_prev;
  // update values and return true if there was a change
  bool update(void);
};

// sequencer
class sequencer 
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
};

// LCD display
class lcdisp
{
  private:
  LiquidCrystal_I2C lcd;
    //PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE
  
  void update(void);
};

enum mode_e { PC, MODE, FX };
enum transport_e { STOP, PLAY, PAUSE };

// PHLXM - top system class
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
  Fluxamasynth synth;
  // LCD
  lcdisp disp;

  public:

};

#endif //FLXMSYNTH_SEQ_H