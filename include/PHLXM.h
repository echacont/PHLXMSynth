// echacont
// PHLXM header file

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

  // sequence parameters


  // constructor
  Controller(void);
  void updateStatus(void);        // see what input is being applied
  void updateMode();   // with the input, what is being controlled?
};

// PHLXM - top system class
class PHLXM
{
  private:
  // controller
  Controller contrl;
  // LCD
  Lcdisp disp;

  public:
  Leds leds;
  // sequencer
  Sequencer sq;
  // constructor
  PHLXM(void);
  void run(void);
  void tick(void);
};

#endif //PHLXM_H