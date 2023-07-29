// echacont
// PHLXM header file

#ifndef PHLXM_H
#define PHLXM_H

#include <Fluxamasynth.h>
#include "defines.h"
#include "lcdisplay.h"



// data structures
/*
struct Note {
  int pitch;      // MIDI pitch    [0, 127]
  int velocity;   // MIDI velocity [0, 127]
  int gate;       // how long this note is? [1, TICKS_PER_STEP]
  int start;      // when this note starts? [0, TICKS_PER_STEP-1]
  Note(void);
  setPitch(int p);
  setNote(int p, int v, int g, int s);

}; 
class Step 
{
  int nextStep;
  bool enable;
  Note notes[MAX_NOTES];
  public:
  Step();
  setStep(int step, int pitch);
};
*/

struct potStatus {
  bool status[NUM_POTS];
  int  value[NUM_POTS];
};

struct buttonStatus {
  bool status[NUM_BUTTONS];
  bool value[NUM_BUTTONS];
};

//typedef struct note note_t;
typedef struct potStatus potStatus_t;
typedef struct buttonStatus buttonStatus_t;

//typedef struct sstep step_t;

struct leds {
  bool status[NUM_LEDS];
  int  pin[NUM_LEDS];
};

typedef struct leds leds_t;

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
  public:
  // scales
  //int major_scale[7]  = {2, 2, 1, 2, 2, 2, 1};     // major scale
  int minor_scale1[SCALE_SIZE] = {2, 1, 2, 2, 2, 1, 2};     // minor scale

  // step sequence
  int seq[NUM_STEPS0];
  //Step seq[NUM_STEPS0];

  // synthesis
  int voices;
  int spread;
  int panspread;

  // transport
  transport_e trans;
  int tempo;
  int millisPerTick;
  int currentTick;
  int currentStep;
  
  bool sqLeds[NUM_LEDS];

  Sequencer(void);
  void tick(void);
  void initSequences(int root);
  void playStep(int step);

};


class Program
{
  public:
  int voiceProgram[NUM_UNISON_VOICES];
  int voicePan[NUM_UNISON_VOICES];
  int voiceVol[NUM_UNISON_VOICES];
  int masterVol;
  int chorus;
  int reverb;
  Program(void);
};

// PHLXM - top system class
class PHLXM
{
  private:
  mode_e mode;
  // controller
  //Controller contrl;
  // LCD
  Lcdisp disp;

  public:
  Program currentProgram;
  Leds leds;
  // sequencer
  Sequencer sq;
  // constructor
  PHLXM(void);
  void run(void);
  void tick(void);

};

#endif //PHLXM_H