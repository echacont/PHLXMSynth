// PHLXM header file
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

#ifndef PHLXM_H
#define PHLXM_H

#include <Fluxamasynth.h>
#include "defines.h"
#include "sequencer.h"
#include "lcdisplay.h"

/*
class ExtMIDI {
  public:
  extMIDIState_t midiState;
  

  void initMIDIState(void);
  //ExtMIDI(HardwareSerial &s) { initMIDIState(); }
  void checkMIDI(void);
  ExtMIDI(void)  { initMIDIState(); }
};
*/

// controller
class Controller
{
  private:
  int buttonPins[NUM_BUTTONS];
  bool buttondeBounce[NUM_DEBOUNCES][NUM_BUTTONS];
  public:

  controllerStatus_t status, status_prev;
  controllerMode_t mode;
  synthProgram_t program;

  // constructor
  Controller(void);
  void updateStatus(void);        // see what input is being applied
  void updateMode(void);   // with the input, what is being controlled?
  int getMillisPerTick(void);
};

// PHLXM - top system class
class PHLXM
{
  private:
  public:
  // controller
  Controller contrl;  
  // sequencer
  Sequencer sq;
  // LCD and LEDS
  Lcdisp disp;
  Leds leds;

  // constructor
  PHLXM(void);
  void run(extFlags_t);
  void tick(void);
};





#endif //PHLXM_H