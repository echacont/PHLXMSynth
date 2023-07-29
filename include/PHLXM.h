// echacont
// PHLXM header file

#ifndef PHLXM_H
#define PHLXM_H

#include <Fluxamasynth.h>
#include "defines.h"
#include "sequencer.h"
#include "lcdisplay.h"

struct potStatus {
  bool status[NUM_POTS];
  int  value[NUM_POTS];
};

struct buttonStatus {
  bool status[NUM_BUTTONS];
  bool value[NUM_BUTTONS];
};

typedef struct potStatus potStatus_t;
typedef struct buttonStatus buttonStatus_t;

// controller
class Controller
{
  private:
  potStatus_t potStatus, potStatus_prev;
  buttonStatus_t buttonStatus, buttonStatus_prev;
  public:
  // constructor
  Controller(void);
  // update values and return true if there was a change
  bool update(void);
};

// PHLXM - top system class
class PHLXM
{
  private:
  mode_e mode;
  // controller
  //Controller contrl;
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