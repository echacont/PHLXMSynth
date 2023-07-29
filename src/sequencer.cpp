// echacont
// PHLXM sequencer source file

#include "sequencer.h"

// TODO:
// eliminate voices and replace with NUM_UNISON_VOICES

// sequencer constructor also calls base class constructor
Sequencer::Sequencer(void) : Fluxamasynth()
{
  tempo = BASE_TEMPO;
  trans = STOP;
  voices = NUM_UNISON_VOICES;
  spread = UNISON_PITCH_SPREAD;
  panspread = UNISON_PAN_SPREAD;
  millisPerTick = 60000/(tempo*TICKS_PER_BEAT); // 250    
  currentTick = 0;
  currentStep = 0;
  for (int i=0; i<NUM_LEDS; i++) sqLeds[i] = false;
  setMasterVolume(72);
  for (int i=0; i<voices; i++) {
    allNotesOff(i); 
    setChannelVolume(i, 32);  
    pan(i,127-((i-voices/2)*panspread));
    setChorus(i, 6, 32, 80, 8 );
    setReverb(i, 7, 72, 72);
  }

  initSequences(DEFAULT_ROOT);
  progChange();
}

void Sequencer::tick(void)
{
  sqLeds[1] = !sqLeds[1];

  playStep(currentStep);

  currentTick++;
  if (currentTick == TICKS_PER_STEP) 
  {
    currentTick = 0;
    currentStep++;
    if (currentStep == NUM_STEPS0) currentStep = 0;
  }
}

void Sequencer::playStep(int step)
{
  if (currentTick == 0) 
    for(int i = 0; i < voices; i++) {
      noteOn(i, seq[step], 100);
      pitchBend(i, 512 + (i-voices/2)*(spread/voices));
    }
  if (currentTick == TICKS_PER_STEP-1)
    for(int i = 0; i < voices; i++) {
      noteOff(i, seq[step]);
  }
}

void Sequencer::initSequences(int root)
{
  int interval = 0;
  int degree = 0;
  int scaleSize = SCALE_SIZE;
  for(int step = 0; step < NUM_STEPS0; step++) {
    degree = step%scaleSize;
    if (step == 0) interval = 0;
    else interval = interval+minor_scale1[degree];
    seq[step]= root+interval;
  } 
}
void Sequencer::progChange(Program program)
{
  for (int i=0; i<NUM_UNISON_VOICES; i++) {
    programChange(program.bank, i, program.voiceProgram[i]);
  }
}
void Sequencer::progChange(void)
{
  for (int i=0; i<NUM_UNISON_VOICES; i++) {
    programChange(currentProgram.bank, i, currentProgram.voiceProgram[i]);
  }
}

Program::Program(void)
{
  bank = 0;
  for (int i=0; i<NUM_UNISON_VOICES; i++) {
    voiceProgram[i] = INITIAL_PROGRAM;
  }
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
