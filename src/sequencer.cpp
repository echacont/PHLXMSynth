// echacont
// PHLXM sequencer source file

#include "sequencer.h"

// TODO:
// eliminate voices and replace with NUM_UNISON_VOICES

// sequencer constructor also calls base class constructor
Sequencer::Sequencer(void) : Fluxamasynth()
{
  // some of this can be removed as state is updated from mode first thing after setup()
  state.trans = STOP;
  state.voices = NUM_UNISON_VOICES;
  state.spread = UNISON_PITCH_SPREAD;
  state.panspread = UNISON_PAN_SPREAD;  
  state.currentTick = 0;
  state.currentStep = 0;
  state.root = DEFAULT_ROOT;
  state.chordStep = CHORD_STEP;
  state.numChordNotes = NUM_CHORD_NOTES;

  for (int i=0; i<NUM_LEDS; i++) sqLeds[i] = false;
  
  setMasterVolume(72);
  for (int i=0; i<state.voices; i++) {
    allNotesOff(i); 
    setChannelVolume(i, 32);  
    pan(i,127-((i-state.voices/2)*state.panspread));
    setChorus(i, 6, 32, 80, 8 );
    setReverb(i, 7, 72, 72);
  }

  initSequence();
}

void Sequencer::tick(void)
{
  sqLeds[1] = !sqLeds[1];

  switch (state.trans) 
  {
    case PLAY:
      playStep(state.currentStep);
      state.currentTick++;
      if (state.currentTick == TICKS_PER_STEP) 
      {
        state.currentTick = 0;
        state.currentStep++;
        if (state.currentStep == NUM_STEPS0) state.currentStep = 0;
      }
      sqLeds[0] = true;
      break;
    case PAUSE:
      sqLeds[0] = !sqLeds[0];
      state.currentTick = TICKS_PER_STEP-1;
      break;
    case STOP:
      sqLeds[0] = false;
      state.currentStep = 0;
      state.currentTick = 0;
      break;
  }
}

void Sequencer::playStep(int step)
{
  if (seq[step] != 0)
  {
    if (state.currentTick == 0) 
      playChord(seq[step], state.numChordNotes, true);
    if (state.currentTick == TICKS_PER_STEP-1)
      playChord(seq[step], state.numChordNotes, false);
  }
}

void Sequencer::playChord(int pitch, int numNotes, bool gate)
{
    for (int j=0; j<numNotes; j++)
    {
      for(int i = 0; i < state.voices; i++) {
        int degree = (j*state.chordStep)%SCALE_SIZE;
        int octaves = (j*state.chordStep-degree)/SCALE_SIZE;
        if (gate) {
          noteOn(i, pitch+12*octaves+minor_scaleI[degree], 100);
          pitchBend(i, 512 + (i-state.voices/2)*(state.spread/state.voices));
        } else {
          noteOff(i, pitch+12*octaves+minor_scaleI[degree]);
        }
      }
    }
}

void Sequencer::initSequence(void)
{
/*int interval = 0;
  int degree = 0;
  int scaleSize = SCALE_SIZE;
  for(int step = 0; step < NUM_STEPS0; step++) {
    degree = step%scaleSize;
    if (step == 0) interval = 0;
    else interval = interval+minor_scale1[degree];
    seq[step]= root+interval;
  } */
  for(int step = 0; step < NUM_STEPS0; step++)
    seq[step]= state.root;
}

void Sequencer::updateSequencer(controllerMode_t mode)
{
  if (mode.updateSeq) {
    state.root = mode.root;
    state.chordStep = mode.chordStep;
    state.numChordNotes = mode.numChordNotes;
    for (int i=0; i<NUM_STEPS0; i++)
      // pSeq is scale degree based so 1 is root
      // Also, if 0 is found then it is preserved (silence)
      if (mode.pSeq[i] == 0) seq[i] = 0;
      else seq[i] = (mode.pSeq[i]-1)+state.root;
  }

  if (mode.allNotesOff) 
    for(int i = 0; i < 16; i++)
      allNotesOff(i);

  // transport
  state.trans = mode.trans;
  // tempo bpm
  if (mode.tempoChange) state.tempo = mode.tempo;

}

void Sequencer::progChange(synthProgram_t program)
{
  if (program.update == true) 
  {
    for (int i=0; i<NUM_UNISON_VOICES; i++) {
      programChange(program.bank, i, program.voiceProgram[i]);
    }
  }
  program.update = false;
}



/*
Note::Note(void)
{
  pitch = DEFAULT_ROOT;
  velocity = 100;
  gate = 1;
  start = 0;
}
Note::setPitch(int p)
{
  pitch = p;
  velocity = 100;
  gate = 1;
  start = 0;
}
Note::setNote(int p, int v, int g, int s)
{
  pitch = p;
  velocity = v;
  gate = g;
  start = s;
}
Step::Step()
{
  nextStep = 0;
  enable = false;
}

Step::setStep(int step, int pitch)
{
  if (step < NUM_STEPS0) nextStep = step+1;
  else nextStep = 0;
  enable = true;
  notes[0].setPitch(pitch);
}
*/
