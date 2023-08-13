// PHLXM sequencer header file
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

#ifndef PHLXM_SEQUENCER_H
#define PHLXM_SEQUENCER_H

#include <Fluxamasynth.h>
#include "defines.h"


struct fineStepSequence {
  byte onSeq[NUM_STEPS0][MIDI_TICKS_PER_BEAT][POLYPHONY];
  byte offSeq[NUM_STEPS0][MIDI_TICKS_PER_BEAT][POLYPHONY];
  fineStepSequence(void);
  void programFineStep(sq_mode_e mode, int step, int root);
};

// sequencer control the synth so it 
// derives from Fluxamasynth
class Sequencer : private Fluxamasynth
{
  private:
  public:
  // scales
  //int major_scale[7]  = {2, 2, 1, 2, 2, 2, 1};     // major scale
  int minor_scale[SCALE_SIZE] =  {2, 1, 2, 2, 2, 1, 2};     // minor scale
  int minor_scaleI[SCALE_SIZE] = {0, 2, 3, 5, 7, 9, 10};     // minor scale

  // step sequence
  int seq[NUM_STEPS0];
  //Step seq[NUM_STEPS0];
  
  // fine step sequence
  fineStepSequence tseq;

  sequencerState_t state; 

  Sequencer(void);
  void tick(void);
  void initFineSequence(int);
  void updateSequencer(controllerMode_t mode, extFlags_t flags);
  void progChange(synthProgram_t program);
  void playStep(int step);
  void playChord(int pitch, int numNotes, bool gate);
  void playFineSequenceTick(void);
  void playAllNotesOff(void);
};

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

#endif // PHLXM_SEQUENCER_H