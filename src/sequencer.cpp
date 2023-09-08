// PHLXM sequencer source file
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

#include "sequencer.h"

// TODO:
// eliminate voices and replace with NUM_UNISON_VOICES

// sequencer constructor also calls base class constructor
Sequencer::Sequencer(void) : Fluxamasynth()
{
  state.updateSeq = true;
  state.allNotesOff = true;
  // some of this can be removed as state is updated from mode first thing after setup()
  state.trans = STOP;
  state.voices = NUM_UNISON_VOICES;
  state.currentTick = 0;
  state.currentStep = 0;
  state.root = DEFAULT_ROOT;
  state.chordStep = CHORD_STEP;
  state.numChordNotes = NUM_CHORD_NOTES;
  state.divisor = MIDI_TICKS_PER_BEAT;
  state.seqCycles = 0;
  state.notesPlayed = 0;
  //state.mode = ARP1;
  
  for (int i=0; i<state.voices; i++) {
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
      state.currentTick++;
      if (state.currentTick > state.divisor-1) {
        state.currentTick = 0;
        state.currentStep++;
      }
      if (state.currentStep > NUM_STEPS0-1) { 
          state.currentStep = 0;
          state.seqCycles++;
      }
      playStep(state.currentStep);
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
      if (seq[step] > 0) playFineSequenceTick(seq[step]);
      break;

    case ARP2:
      if (seq[step] > 0) playFineSequenceTick(seq[step]);
      break;

    default:
      break;
  }
  // clean up synth voices (stuck notes)
  if ((state.currentStep == NUM_STEPS0-1) && (state.currentTick == state.divisor-1))
    for(int i = 0; i < state.voices; i++) 
      allNotesOff(state.notesPlayed%state.voices);
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

// TODO: integrate chord mode here
void Sequencer::playFineSequenceTick(int pitch)
{
  switch (state.mode)
  {
  case ARP1:
    if (tseq.onSeq[state.currentTick] > 0) {
      for(int i = 0; i < state.voices; i++) {
        //if ((state.currentStep+state.seqCycles+state.notesPlayed)%state.voices > 0) 
        {
          noteOn(i, pitch+tseq.onSeq[state.currentTick]-1, ((state.currentTick-state.notesPlayed)%14)+86);
          pitchBend(i, 512 + (i-state.voices/2)*(state.spread/state.voices));
          state.notesPlayed++;
        }
      }
    }
    break;
  
  case ARP2:
    if (tseq.onSeq[state.currentTick] > 0) {
      if (state.currentTick%state.numChordNotes == 0) {
        noteOn(state.currentTick%state.voices, pitch+tseq.onSeq[state.currentTick]+minor_scaleI[(state.currentTick%state.voices*state.chordStep)%7], ((state.currentTick-state.notesPlayed)%14)+86);
        pitchBend(state.currentTick%state.voices, 512 + (state.currentTick%state.voices-state.voices/2)*(state.spread/state.voices));
        state.notesPlayed++;
      }
    }
    break;

  default:
    break;
  }

  //if (tseq.offSeq[state.currentTick] > 0) {
    //for(int i = 0; i < state.voices; i++) 
      //noteOff(i, pitch+tseq.offSeq[state.currentTick]-1);
  //}
}


void Sequencer::updateSequencer(controllerMode_t mode, extFlags_t* flags)
{
  if (state.updateSeq || flags->updateSequence) {
    state.updateSeq = false;
    flags->updateSequence = false;
    state.spread = mode.spread;

        for (int i=0; i<NUM_STEPS0; i++) {
          // pSeq is scale degree based so 1 is root
          // Also, if 0 is found then it is preserved (silence)
          if (mode.pSeq[i] == 0) seq[i] = 0;
          else seq[i] = (mode.pSeq[i]-1)+state.root;
          if (state.mode == ARP1) 
            tseq.programFineStep(state.mode, minor_scaleI, state.numChordNotes, state.chordStep);
          else if (state.mode == ARP2) 
            tseq.programFineStep(state.mode, minor_scaleI, state.numChordNotes, state.chordStep);
        }
    // ARP mode
    if (mode.arpMode == CHORD_MODE) state.mode = CHORD;
    else if (mode.arpMode == ARP1_MODE) state.mode = ARP1;
    else   state.mode = ARP2;
  }

  if (state.allNotesOff) {
    state.allNotesOff = false;
    for(int i = 0; i < 16; i++)
      allNotesOff(i);
  }

  // transport
  state.trans = mode.trans;

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
  for (int j = 0; j < MIDI_TICKS_PER_BEAT; j++)
  {
        onSeq[j] = 0;     // zero means don't issue anything to Fluxamasynth
        offSeq[j] = 0;
  }
}

void fineStepSequence::programFineStep(sq_mode_e mode, int8_t intervals[SCALE_SIZE], int8_t numChordNotes, int8_t chordStep)
{
  int8_t degree, tStep, gate, n;
  switch(mode)
  {
    case ARP1:
      degree = 0;
      tStep = MIDI_TICKS_PER_BEAT/numChordNotes;
      //int tStep = 6;
      gate = tStep-1;
      n = 0;
      for (int j = 0; j < MIDI_TICKS_PER_BEAT; j++) {
        if ( (j%tStep) == 0 ) {
          degree = (n*chordStep)%SCALE_SIZE;
          onSeq[j] = intervals[degree]+1;
        } else onSeq[j] = 0;
        if ( (j%tStep) == gate ) {
          offSeq[j] = intervals[degree]+1;
          n++;
        } else offSeq[j] = 0;
      }
      break;

    case ARP2:
      degree = 0;
      n = 0;
      for (int j = 0; j < MIDI_TICKS_PER_BEAT; j++) {
          degree = (n*chordStep)%SCALE_SIZE;
          onSeq[j] = intervals[degree]+1;
          n++;
          if (n>numChordNotes) n = 0;
      }
      break;

    case CHORD:
      break;

    default:
      break;
  }
}