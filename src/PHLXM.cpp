// PHLXM source file
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

#include "PHLXM.h"

PHLXM::PHLXM(void)
{ }

void PHLXM::run(extFlags_t flags)
{
  // right after the model constructors run at main::setup(), 
  // update synth program and sequencer state. Then get controller data, 
  // apply changes to models and come around again (and again and again....)
  sq.progChange(contrl.program);   // apply program change
  sq.updateSequencer(contrl.mode, flags);// apply user sequence

  // TODO: these flags could be checked inside of tick and updates
  //if (flags.runSequencerTick)
  //  sq.tick();
  //if (flags.updateDisplay) 
  {
    disp.update(contrl.program,     // LCD has a lot of stuff to show
                contrl.mode,
                sq.state);        
  }

  contrl.updateStatus();   // reads controller into status (user input)
  contrl.updateMode(flags);    // what needs to be done? Do it.
}

/*--------------------------------------------------*/

Controller::Controller(void)
{
  buttonPins[BUTTON_0] = PIN_BUTTON_0;
  buttonPins[BUTTON_1] = PIN_BUTTON_1;
  buttonPins[BUTTON_2] = PIN_BUTTON_2;

  for (int i = 0; i<NUM_POTS; i++) {
    status_prev.potChanged[i] = false;
    status_prev.potValue[i] = 0;
  }
  for (int i = 0; i<NUM_BUTTONS; i++) {
    status.buttonChanged[i] = false;
    status_prev.buttonValue[i] = false;
    // initialize Arduino pins
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  // debouncer
  for (int i=0; i<NUM_BUTTONS; i++)
    for (int j=0; j<NUM_DEBOUNCES; j++)
      buttondeBounce[j][i] = false;

  // initialize the controller mode
  mode.menu = REVERB;
  mode.trans = STOP;
  mode.pointer = 0;
  mode.value = 0;
  mode.option = 0;
  mode.fxParam = 0;
  mode.fxValue = 0;
  mode.root = DEFAULT_ROOT;
  mode.chordStep = CHORD_STEP;
  mode.numChordNotes = NUM_CHORD_NOTES;
  ///////////////////////////////////////////////////////
  mode.tempo = BASE_TEMPO;
  mode.millisPerTick = 60000/(mode.tempo*TICKS_PER_BEAT);
  ///////////////////////////////////////////////////////
  for (int i=0; i<NUM_STEPS0; i++)
    mode.pSeq[i] = 1;
  mode.spread = UNISON_PITCH_SPREAD;
  // flags
  mode.menuChanged = true;
  mode.updateSeq = true;
  mode.tempoChange = true;
  mode.allNotesOff = false;
  // MIDI divisor
  mode.divisor = 24;

  // initialize a program
  for (int i=0; i<NUM_UNISON_VOICES; i++) {
    program.voiceProgram[i] = INITIAL_PROGRAM;
    program.voiceVol[i] = INITIAL_VOICE_VOL;
    // voice FX
    program.chorusType[i]  = 0;
    program.chorusLevel[i] = 0;
    program.chorusDelay[i] = 0;
    program.chorusFdbk[i]  = 0;
    program.chorusRate[i]  = 0;
    program.chorusDepth[i] = 0;
    program.reverbType[i]  = 0;
    program.reverbLevel[i] = 0;
    program.reverbFdbk[i]  = 0;
  }
  program.bank = 127; // use the MT-32 sound bank
  program.masterVol = INITIAL_MASTER_VOL;
  program.panspread = UNISON_PAN_SPREAD; 
  program.update = true;
}

void Controller::updateStatus(void)
{
  // clear potChanged and buttonChanged flags
  for (int i=0; i<NUM_POTS; i++)
    status.potChanged[i] = false;
  for (int i=0; i<NUM_BUTTONS; i++)
    status.buttonChanged[i] = false;

  // potentiometers
  for (int i = 0; i<NUM_POTS; i++) {
    status.potValue[i] = analogRead(i)>>3;
    status.potChanged[i] = false;
    if (status.potValue[i] != status_prev.potValue[i]) {
      status.potChanged[i] = true;
    }
    status_prev.potValue[i] = status.potValue[i];
  }
  // buttons
  for (int i = 0; i<NUM_BUTTONS; i++) {
    status.buttonChanged[i] = false;
    if (!digitalRead(buttonPins[i]))
      status.buttonValue[i] = true;
    else status.buttonValue[i] = false;
    
    for(int j=1; j<NUM_DEBOUNCES; j++) 
      buttondeBounce[j-1][i] = buttondeBounce[j][i];
    buttondeBounce[NUM_DEBOUNCES-1][i] = status.buttonValue[i];

    bool debounce = true;
    for(int j=0; j<NUM_DEBOUNCES; j++)
      // all recorded values in the buffer must be different
      // than previous value (button was pressed)
      if (buttondeBounce[j][i] == status_prev.buttonValue[i])
        debounce = false;
    if  (debounce && 
        (status.buttonValue[i] != status_prev.buttonValue[i])) {
      status.buttonChanged[i] = true;
      status_prev.buttonValue[i] = status.buttonValue[i];
    }
  }
}

void Controller::updateMode(extFlags_t flags)
{
  // clear flags and reset pointer and option artifacts
  if (mode.menuChanged) {
    mode.pointer = 0; // reset pointer if just entered menu
    mode.option = 0;
    mode.menuChanged = false;
  }
  program.update = false;
  mode.allNotesOff = false;
  mode.updateSeq = false;

  // state machine kinda
  switch (mode.menu)
  {
  case PC:  // program change
    if (status.potChanged[POT_0] == true) {
      mode.pointer = status.potValue[POT_0]>>5; // get 2 MSB
    }
    if (status.potChanged[POT_1] == true) {
      mode.option = status.potValue[POT_1]; // 7 bits
    }
    // Button 1 is Enter
    if (status.buttonChanged[BUTTON_1] && status.buttonValue[BUTTON_1]) {
      program.voiceProgram[mode.pointer] = mode.option;
      program.update = true;
    }
    break;
  
  case SEQ:
    if (status.potChanged[POT_0] == true) {
      // get 3 MSB because sequencer has 8 steps
      mode.pointer = status.potValue[POT_0]>>4; 
    }
    if (status.potChanged[POT_1] == true) {
      // 3 bits are needed to decode the scale up to an octave
      mode.option = status.potValue[POT_1]>>4;
    }
    if (status.buttonChanged[BUTTON_1] && status.buttonValue[BUTTON_1]) {
      mode.pSeq[mode.pointer] = mode.option;
      mode.updateSeq = true;
    }
    break;

  case HARM_MODE:
    if (status.potChanged[POT_0] == true) {
      // get 2 MSB for three options (Root, Step, All notes off)
      mode.pointer = status.potValue[POT_0]>>5; 
    }
    if (status.potChanged[POT_1] == true) {
      if (mode.pointer == 0) // Root
        // cannot be zero (could be interpreted as silence by sequencer)
        // this can be source of bugs in the future
        mode.option = status.potValue[POT_1]+1;
      if (mode.pointer == 1)   // Chord steps
        // 3 bits are needed to decode the scale up to an octave
        mode.option = status.potValue[POT_1]>>4;
      if (mode.pointer == 2)   // number of notes per chord
        // 3 bits just because what could happen
        mode.option = status.potValue[POT_1]>>4;
    }
    if (status.buttonChanged[BUTTON_1] && status.buttonValue[BUTTON_1]) {
      if (mode.pointer == 0) {// ROot
        mode.root = mode.option;
        mode.updateSeq = true;
      }
      if (mode.pointer == 1) {
        mode.chordStep = mode.option;
        mode.updateSeq = true;
      }
      if (mode.pointer == 2) {
        mode.numChordNotes = mode.option;
        mode.updateSeq = true;
      }
      if (mode.pointer == 3) {
        mode.allNotesOff = true;
      }
    }
    break; 

  case GEN:
    if (status.potChanged[POT_0] == true) {
      // get 2 MSB for three options (main, volume, detune, pan spread)
      mode.pointer = status.potValue[POT_0]>>5; 
    } 
    if (status.potChanged[POT_1] == true) {
      if ((mode.pointer == 0)) // Volume 
        mode.option = status.potValue[POT_1];
      else if ((mode.pointer == 3)) // Divisor
        mode.option = (status.potValue[POT_1]>>2)+1;   // from 1 up to 32
      else // Detune (spread) or Pan spread
        // get 8 options
        mode.option = status.potValue[POT_1]>>4;
    }
    if (status.buttonChanged[BUTTON_1] && status.buttonValue[BUTTON_1]) {
      switch (mode.pointer)
      {
        case 0: 
          program.masterVol = mode.option;
          program.update = true;
          break;
        case 1: 
          // shift left 6 bits to get full bend range of 512
          mode.spread = mode.option<<6;
          mode.updateSeq = true;
          break;
        case 2:
          program.panspread = mode.option<<4;
          program.update = true;
          break;
        case 3: 
          mode.divisor = mode.option;
          mode.updateSeq = true;
          //mode.tempo = mode.option + BASE_TEMPO;
          //mode.millisPerTick = 60000/(mode.tempo*TICKS_PER_BEAT);
          //mode.tempoChange = true;
          break;
      }
    }
    break;

  case MIX:
    if (status.potChanged[POT_0] == true) {
      mode.pointer = status.potValue[POT_0]>>5; // get 2 MSB (4 voices)
    }
    if (status.potChanged[POT_1] == true) {
      mode.option = status.potValue[POT_1]; // 7 bits
    }
    if (status.buttonChanged[BUTTON_1] && status.buttonValue[BUTTON_1]) {
      program.voiceVol[mode.pointer] = mode.option;
      program.update = true; 
    }
    break;

  case CHORUS:
    // This menu is different: one pointer selects voice, a second one
    // selects the parameter and the third/fourth selects the value
    if (status.potChanged[POT_0] == true) {
      mode.pointer = status.potValue[POT_0]>>5; // get 2 MSB (4 options)
    }
    if (status.potChanged[POT_1] == true) {
      mode.value = status.potValue[POT_1];
    }
    if (status.buttonChanged[BUTTON_1] && status.buttonValue[BUTTON_1]) {
      // choose between the 4 voices
      if (mode.pointer == 0) { mode.option = mode.value>>5; }
      // choose between 8 parameters (not all are used)
      else if (mode.pointer == 1) { mode.fxParam = mode.value>>4; } 
      // choose value: range depends on what parameter was chosen (fxParam)
      else 
      {
        if      (mode.fxParam == CHORUS_LEVEL) { program.chorusLevel[mode.option] = mode.value; }
        else if (mode.fxParam == CHORUS_TYPE)  { program.chorusType[mode.option]  = mode.value>>4; }
        else if (mode.fxParam == CHORUS_DELAY) { program.chorusType[mode.option]  = mode.value; } 
        else if (mode.fxParam == CHORUS_FDBK)  { program.chorusFdbk[mode.option]  = mode.value; } 
        else if (mode.fxParam == CHORUS_RATE)  { program.chorusRate[mode.option]  = mode.value; } 
        else if (mode.fxParam == CHORUS_DEPTH) { program.chorusDepth[mode.option] = mode.value; } 
        program.update = true; // only update program when new value is entered
      }
      
    }
    break;

  case REVERB:
    // This menu is different: one pointer selects voice, a second one
    // selects the parameter and the third/fourth selects the value
    if (status.potChanged[POT_0] == true) {
      mode.pointer = status.potValue[POT_0]>>5; // get 2 MSB (4 options)
    }
    if (status.potChanged[POT_1] == true) {
      mode.value = status.potValue[POT_1];
    }
    if (status.buttonChanged[BUTTON_1] && status.buttonValue[BUTTON_1]) {
      // choose between the 4 voices
      if (mode.pointer == 0) { mode.option = mode.value>>5; }
      // choose between 3 parameters (not all are used)
      else if (mode.pointer == 1) { mode.fxParam = mode.value>>5; } 
      // choose value: range depends on what parameter was chosen (fxParam)
      else 
      {
        if      (mode.fxParam == REVERB_LEVEL) { program.reverbLevel[mode.option] = mode.value; }
        else if (mode.fxParam == REVERB_TYPE)  { program.reverbType[mode.option]  = mode.value>>4; }
        else if (mode.fxParam == REVERB_FDBK)  { program.reverbFdbk[mode.option]  = mode.value; } 
        program.update = true; // only update program when new value is entered
      }
      
    }
    break;

  default:
    break;
  
  } // switch(mode.menu)

  // scroll the menu
  if(status.buttonChanged[BUTTON_0] && status.buttonValue[BUTTON_0]) {
    mode.menu = mode.menu+1;
    mode.menuChanged = true;
  }
  if (mode.menu == last) mode.menu = 0;


  // transport machine
  // transport machine uses button 2 for (PASE/STOP) and (PLAY)
  /*
  if(status.buttonChanged[BUTTON_2]) //} && status.buttonValue[BUTTON_0])
  {
    switch (mode.trans)
    {
      case STOP:
        if (status.buttonValue[BUTTON_2]) mode.trans = PLAY;
        break;

      case PAUSE:
        if (!status.buttonValue[BUTTON_2])
            mode.trans = STOP;
        break;

      case PLAY:
        if (status.buttonValue[BUTTON_2]) mode.trans = PAUSE;
        break;
    }
  }
  */

}

int Controller::getMillisPerTick(void) 
{
  if (mode.tempoChange) {
    mode.tempoChange = false;
    return mode.millisPerTick;
  } else {
    return 0;
  }
}
