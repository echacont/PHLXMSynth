// PHLXM header file
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

#ifndef PHLXM_H
#define PHLXM_H

#include <Fluxamasynth.h>
#include "defines.h"
#include "sequencer.h"
#include "lcdisplay.h"

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
  void updateMode(extFlags_t);   // with the input, what is being controlled?
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
  // Liquid Crystal Display
  Lcdisp disp;

  // constructor
  PHLXM(void);
  void run(extFlags_t);
  void tick(void);
};

#endif //PHLXM_H