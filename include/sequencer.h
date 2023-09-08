// PHLXM sequencer header file
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

#ifndef PHLXM_SEQUENCER_H
#define PHLXM_SEQUENCER_H

#include <Fluxamasynth.h>
#include "defines.h"

// this of course can be a good way to blow up the memory
class fineStepSequence {
  public:
  // these arrays contain interval values, not absolute pitches.
  int8_t onSeq[MIDI_TICKS_PER_BEAT];
  int8_t offSeq[MIDI_TICKS_PER_BEAT];
  fineStepSequence(void);
  void programFineStep(sq_mode_e mode, 
                       int8_t intervals[SCALE_SIZE], 
                       int8_t numChordNotes, 
                       int8_t chordStep);
};

// sequencer control the synth so it 
// derives from Fluxamasynth
class Sequencer : private Fluxamasynth
{
  private:
  public:
  // scales
  //int major_scale[7]  = {2, 2, 1, 2, 2, 2, 1};     // major scale
  int8_t minor_scale[SCALE_SIZE] =  {2, 1, 2, 2, 2, 1, 2};     // minor scale
  int8_t minor_scaleI[SCALE_SIZE] = {0, 2, 3, 5, 7, 9, 10};     // minor scale

  // step sequence
  int8_t seq[NUM_STEPS0];
  //Step seq[NUM_STEPS0];
  
  // fine step sequence
  fineStepSequence tseq;

  sequencerState_t state; 

  Sequencer(void);
  void tick(void);
  void initFineSequence(int);
  void updateSequencer(controllerMode_t mode, extFlags_t* flags);
  void progChange(synthProgram_t program);
  void playStep(int step);
  void playChord(int pitch, int numNotes, bool gate);
  void playFineSequenceTick(int);
  void playAllNotesOff(void);
};

#endif // PHLXM_SEQUENCER_H