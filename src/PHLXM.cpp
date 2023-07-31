// echacont
// PHLXM source file
#include "PHLXM.h"

PHLXM::PHLXM(void)
{ }

void PHLXM::run(void)
{
  sq.updateSequencer(contrl.mode); // apply user sequence
  sq.progChange(contrl.program);  // apply program change
  
  leds.update(sq.sqLeds);
  disp.update(contrl.program,
              contrl.mode,
              sq.state);

  contrl.updateStatus();          // reads controller into status
  contrl.updateMode();      // what needs to be done.
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
  mode.menu = GEN;
  mode.menuChanged = true;
  mode.trans = STOP;
  mode.pointer = 0;
  mode.option = 0;
  mode.counter = 0;
  mode.root = DEFAULT_ROOT;
  mode.chordStep = CHORD_STEP;
  mode.numChordNotes = NUM_CHORD_NOTES;
  mode.allNotesOff = false;
  mode.tempo = BASE_TEMPO;
  mode.millisPerTick = 60000/(mode.tempo*TICKS_PER_BEAT);
  mode.tempoChange = true;
  for (int i=0; i<NUM_STEPS0; i++)
    mode.pSeq[i] = 1;
  mode.updateSeq = false;
  // initialize a program
  program.bank = 0;
  for (int i=0; i<NUM_UNISON_VOICES; i++) {
    program.voiceProgram[i] = INITIAL_PROGRAM;
  }
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

void Controller::updateMode()
{
  // clear flags and reset pointer and option artifacts
  if (mode.menuChanged) {
    mode.pointer = 0; // reset pointer if just entered menu
    mode.option = 0;
    mode.menuChanged = false;
  }
  program.update = false;
  mode.allNotesOff = false;
  // tempoChange is cleared when it is consumed by timer interrupt
  // routine  tick() (main.cpp)
  // mode.tempoChange = false;

  // state machine kinda
  // transport machine uses buttons 1 (STOP) and 2 (PLAY/PAUSE)
  // TODO: transport machine
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
      if (mode.pointer == 0) // Volume
        // cannot be zero (could be interpreted as silence by sequencer)
        // this can be source of bugs in the future
        mode.option = status.potValue[POT_1];
      else if (mode.pointer == 3) // BPM
        mode.option = status.potValue[POT_1];
      else
        // get 8 options
        mode.option = status.potValue[POT_1]>>4;
    }
    if (status.buttonChanged[BUTTON_1] && status.buttonValue[BUTTON_1]) {
      switch (mode.pointer)
      {
        case 0: break;
        case 1: break;
        case 2: break;
        case 3: 
          mode.tempo = mode.option + BASE_TEMPO;
          mode.millisPerTick = 60000/(mode.tempo*TICKS_PER_BEAT);
          mode.tempoChange = true;
          break;
      }
    }
    break;

  default:
    break;
  
  }

  // scroll the menu
  if(status.buttonChanged[BUTTON_0] && status.buttonValue[BUTTON_0]) {
    mode.menu = mode.menu+1;
    mode.menuChanged = true;
  }
  if (mode.menu == last) mode.menu = 0;

  // transport
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