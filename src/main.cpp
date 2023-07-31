// echacont
// PHLXM sequencer->synth
#include <Arduino.h>
#include <MsTimer2.h>
#include "PHLXM.h"

// TODO: move timer interrupt code to the PHLXM/Sequencer class maybe

PHLXM* phlxm = NULL;

void tick ()
{
  phlxm->sq.tick();
  int millisPerTick = phlxm->contrl.getMillisPerTick();
  if (millisPerTick) {
    MsTimer2::stop();
    MsTimer2::set(millisPerTick, tick);
    MsTimer2::start();
  }
}
void setup () 
{
  phlxm = new PHLXM();
  MsTimer2::set(phlxm->contrl.mode.millisPerTick, tick);
  MsTimer2::start();
}
void loop () 
{
  phlxm->run();
};
