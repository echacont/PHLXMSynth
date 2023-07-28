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


// create FLXMSynth object
class FLXMSynth 
{
  private:
  // controller
  enum mode_e { PC, MODE, FX } mode = PC;
  potStatus_t potStatus_new, potStatus_prev;
  buttonStatus_t buttonStatus_new, buttonStatus_prev;


  // sequencer
  const int ticksPerBeat    = 4;
  const int baseTempo       = 60;
  int voices = 8;
  int spread = 420;
  int panspread = 20;
  int tempo;
  // step sequence
  step_t seq[NUM_STEPS0];
  
  // synthesizer
  Fluxamasynth synth;

  // LCD
  LiquidCrystal_I2C lcd;
    //PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE

  public:

};

#endif //FLXMSYNTH_SEQ_H