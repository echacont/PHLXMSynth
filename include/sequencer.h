// echacont
// PHLXM sequencer header file

#ifndef PHLXM_SEQUENCER_H
#define PHLXM_SEQUENCER_H

#include <Fluxamasynth.h>
#include "defines.h"

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
  void initSequence(int root);
  void progChange(synthProgram_t program);
  void playStep(int step);
};

#endif // PHLXM_SEQUENCER_H