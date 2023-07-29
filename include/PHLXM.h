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
  // constructor
  Controller(void);
  void updateController(void);    // see what input is being applied
  void updateMode(Program program);          // with the input, what is being controlled?
  controllerStatus_t updateController(controllerStatus_t status_prev);
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