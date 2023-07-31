#include <Arduino.h>
#include <MsTimer2.h>

#include "defines.h"
#include "PHLXM.h"
#include "lcdisplay.h"


PHLXM* phlxm = NULL;

void tick ()
{
  phlxm->sq.tick();
  if (phlxm->contrl.mode.tempoChange) {
    MsTimer2::stop();
    MsTimer2::set(phlxm->sq.state.millisPerTick, tick);
    MsTimer2::start();
  }
}

// TODO: move timer interrupt code to the PHLXM/Sequencer class maybe
void setup () 
{
  phlxm = new PHLXM();
  MsTimer2::set(phlxm->sq.state.millisPerTick, tick);
  MsTimer2::start();
}

void loop () 
{
  phlxm->run();
};
