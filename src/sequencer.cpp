// PHLXM sequencer source file
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

#include "sequencer.h"

// TODO:
// eliminate voices and replace with NUM_UNISON_VOICES

// sequencer constructor also calls base class constructor
Sequencer::Sequencer(void) : Fluxamasynth()
{
  // some of this can be removed as state is updated from mode first thing after setup()
  state.trans = STOP;
  state.voices = NUM_UNISON_VOICES;
  state.currentTick = 0;
  state.currentStep = 0;
  state.root = DEFAULT_ROOT;
  state.chordStep = CHORD_STEP;
  state.numChordNotes = NUM_CHORD_NOTES;
  state.divisor = MIDI_TICKS_PER_BEAT;
  state.mode = ARP1;
  
  for (int i=0; i<state.voices; i++) {
    allNotesOff(i); 
    setChannelVolume(i, INITIAL_VOICE_VOL);  
    //pan(i,127-((i-state.voices/2)*state.panspread));
    setChorus(i, 0, 0, 0, 0 );
    setReverb(i, 0, 0, 0);
  }
  
}

void Sequencer::tick(void)
{
  switch (state.trans)
  {
    case PLAY:
      playStep(state.currentStep);
      state.currentTick++;
      if (state.currentTick == state.divisor) 
      {
        state.currentTick = 0;
        state.currentStep++;
        if (state.currentStep == NUM_STEPS0) state.currentStep = 0;
      }
      break;
    case PAUSE:
      state.currentTick = state.divisor-1;
      break;
    case STOP:
      state.currentStep = 0;
      state.currentTick = 0;
      break;
  }
}

void Sequencer::playStep(int step)
{
  switch(state.mode)
  {
    case CHORD:
      if (seq[step] > 0)
      {
        if (state.currentTick == 0) 
          playChord(seq[step], state.numChordNotes, true);
        if (state.currentTick == state.divisor-1)
          playChord(seq[step], state.numChordNotes, false);
      }
    break;

    case ARP1:
      if (seq[step] > 0) playFineSequenceTick();
      break;

    default:
      break;
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

void Sequencer::playFineSequenceTick(void)
{
  int poly = 0;
  if (tseq.onSeq[state.currentStep][state.currentTick][poly] > 0) {
      for(int i = 0; i < state.voices; i++) {
        noteOn(i, tseq.onSeq[state.currentStep][state.currentTick][poly], 100);
        pitchBend(i, 512 + (i-state.voices/2)*(state.spread/state.voices));
      }
  }
  if (tseq.offSeq[state.currentStep][state.currentTick][poly] > 0) {
    for(int i = 0; i < state.voices; i++) 
      noteOff(i, tseq.offSeq[state.currentStep][state.currentTick][poly]);
  }
}


void Sequencer::updateSequencer(controllerMode_t mode, extFlags_t flags)
{
  if (mode.updateSeq || flags.updateSequence) {
    flags.updateSequence = false;
    state.root = mode.root;
    state.chordStep = mode.chordStep;
    state.numChordNotes = mode.numChordNotes;
    state.spread = mode.spread;
    state.divisor = mode.divisor;
    switch(state.mode)
    {
      case CHORD:
        for (int i=0; i<NUM_STEPS0; i++) {
          // pSeq is scale degree based so 1 is root
          // Also, if 0 is found then it is preserved (silence)
          if (mode.pSeq[i] == 0) seq[i] = 0;
          else seq[i] = (mode.pSeq[i]-1)+state.root;
        }
        break;
      
      case ARP1:
        for (int i=0; i<NUM_STEPS0; i++) {
          // pSeq is scale degree based so 1 is root
          // Also, if 0 is found then it is preserved (silence)
          if (mode.pSeq[i] == 0) seq[i] = 0;
          else tseq.programFineStep(state.mode, i, (mode.pSeq[i]-1)+state.root);
        }
        break;

      default: break;
    }
  }

  if (mode.allNotesOff) 
    for(int i = 0; i < 16; i++)
      allNotesOff(i);

  // transport
  state.trans = mode.trans;
  // tempo bpm
  //if (mode.tempoChange) state.tempo = mode.tempo;
  switch(state.trans)
  {
    case PLAY:
      digitalWrite(PIN_LED_GRN, LOW); // turn on green led (PLAY)
      digitalWrite(PIN_LED_RED, (state.currentStep%2));
      break;
    case STOP:
      digitalWrite(PIN_LED_GRN, HIGH); // turn off green led (STOP)
      digitalWrite(PIN_LED_RED, HIGH);
      break;
    default:
      digitalWrite(PIN_LED_GRN, HIGH); // turn off led
      break;
  }


}

void Sequencer::progChange(synthProgram_t program)
{
  if (program.update == true) 
  {
    for (int i=0; i<NUM_UNISON_VOICES; i++) {
      programChange(program.bank, i, program.voiceProgram[i]);
      pan(i,127-((i-NUM_UNISON_VOICES/2)*program.panspread));
      setChannelVolume(i, program.voiceVol[i]);
      // chorus
      // channel, program, level, feedback, chorusDelay
      //setChorus(i, program.chorusType[i], program.chorusLevel[i]<<1, program.chorusFdbk[i], program.chorusDelay[i] );
      // using same chorus program for all voices
      setChorus(i, program.chorusType[0], program.chorusLevel[0]<<1, program.chorusFdbk[0]<<1, program.chorusDelay[0]<<1 );
      // reverb
      // channel, program, level, delayFeedback 
      setReverb(i, program.reverbType[0], program.reverbLevel[0]<<1, program.reverbFdbk[0]<<1);
      //setReverb(channel, program, level, delayFeedback)
    }
    setMasterVolume(program.masterVol);
  }
  // program.update flag is cleared by the same method that sets it
}

void Sequencer::playAllNotesOff(void) 
{
  for (int i=0; i<state.voices; i++) {
    allNotesOff(i); 
  }
}

/*---------------------------------------------------------------*/
// Constructor
fineStepSequence::fineStepSequence(void)
{
  for (int i = 0; i < NUM_STEPS0; i++)
    for (int j = 0; j < MIDI_TICKS_PER_BEAT; j++)
      for (int n = 0; n < POLYPHONY; n++)
      {
        onSeq[i][j][n] = 0;     // zero means don't issue anything to Fluxamasynth
        offSeq[i][j][n] = 0;
      }
}

void fineStepSequence::programFineStep(sq_mode_e mode, int step, int root)
{
  /*switch(mode)
  {
    case ARP1:*/

      int poly = 0;
      int interval = 4; // major thirds
      int pitch = root;
      //onSeq[step][0][poly] = pitch;
      //offSeq[step][23][poly] = pitch;
      for (int j = 0; j < MIDI_TICKS_PER_BEAT; j++) {
        if ( (j%(MIDI_TICKS_PER_BEAT/4) == 0) )
          onSeq[step][j][poly] = pitch;
        if ( (j%(MIDI_TICKS_PER_BEAT/4)) == (MIDI_TICKS_PER_BEAT-1) ) {
          offSeq[step][j][poly] = pitch;
          pitch = pitch + interval;
        }
      }
      /*
      for (int j = 0; j < MIDI_TICKS_PER_BEAT; j++) {
        if ( !(j%(MIDI_TICKS_PER_BEAT/POLYPHONY)) )
          if (voice < POLYPHONY) { 
            onSeq[step][j][voice] = root+voice*interval;
            voice++;
          }
        voice = 0;
        if ( j == (MIDI_TICKS_PER_BEAT-1) )  // last tick all chord goes off
          do { 
            offSeq[step][j][voice] = root+voice*interval;
            voice++;
          } while (voice < POLYPHONY);
      }
      
      break;

    case CHORD:
      break;

    default:
      break;
  }*/
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
