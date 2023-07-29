// echacont
// PHLXM header file

#ifndef FLXMSYNTH_SEQ_H
#define FLXMSYNTH_SEQ_H

#include "defines.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Fluxamasynth.h>

// enumerated types
enum mode_e { PC, MODE, FX };
enum transport_e { STOP, PLAY, PAUSE };

// data structures
struct Note {
  int pitch;      // MIDI pitch    [0, 127]
  int velocity;   // MIDI velocity [0, 127]
  int gate;       // how long this note is? [1, TICKS_PER_STEP]
  int start;      // when this note starts? [0, TICKS_PER_STEP-1]
  Note(void);
  setPitch(int p);
  setNote(int p, int v, int g, int s);

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

//typedef struct sstep step_t;

struct leds {
  bool status[NUM_LEDS];
  int  pin[NUM_LEDS];
};

typedef struct leds leds_t;

class Step 
{
  int nextStep;
  bool enable;
  Note notes[MAX_NOTES];
  public:
  Step();
  setStep(int step, int pitch);
};

// controller
class Controller
{
  private:
  potStatus_t potStatus, potStatus_prev;
  buttonStatus_t buttonStatus, buttonStatus_prev;
  public:
  // constructor
  Controller(void);
  // update values and return true if there was a change
  bool update(void);
};

// sequencer control the synth so it 
// derives from Fluxamasynth
class Sequencer : private Fluxamasynth
{
  private:
  // scales
  int root = 52;
  int pitch = root;
  int major_scale[7] = {2, 2, 1, 2, 2, 2, 1};     // major scale
  int minor_scale1[7] = {2, 1, 2, 2, 2, 1, 2};     // minor scale
  public:
  int ticksPerBeat = 4;
  int millisPerTick =  125;
  int voices = 8;
  int spread = 420;
  int panspread = 20;
  int tempo = BASE_TEMPO;
  // step sequence
  Step seq[NUM_STEPS0];
  // sequencer
  Fluxamasynth synth;

  bool sqLeds[NUM_LEDS];
  // constructor
  Sequencer(void);
  void tick(void);
  void initSequences(void);

};

// Lcdisplay is derived from LiquidCrystal_I2C 
class Lcdisp : private LiquidCrystal_I2C
{
  private:
  LiquidCrystal_I2C lcd;
  public:
  Lcdisp(void);
  void update(void);
  void update(int value);
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
  void Leds::update(bool leds[]);
  void Leds::update(int led, bool status);
  //void greenLed(bool);
  //void redLed(bool);
};

// PHLXM - top system class
// derives Fluxamasynth, LiquidCrystal_I2C classes
class PHLXM
{
  private:
  mode_e mode;
  transport_e trans;
  // controller
  Controller contrl;
  // LCD
  Lcdisp disp;

  public:
  Leds leds;
  // sequencer
  Sequencer sq;
  // constructor
  PHLXM(void);
  void run(void);
  void tick(void);

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



Fluxamasynth synth;
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
*/

#endif //FLXMSYNTH_SEQ_H