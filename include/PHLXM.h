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
  public:
  controllerStatus_t status, status_prev;
  controllerMode_t controlMode;
  synthProgram_t program;
  // constructor
  Controller(void);
  void updateStatus(void);    // see what input is being applied
  void updateControl();          // with the input, what is being controlled?
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