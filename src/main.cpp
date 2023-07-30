#include <Arduino.h>
#include <MsTimer2.h>

#include "defines.h"
#include "PHLXM.h"
#include "lcdisplay.h"


PHLXM* phlxm = NULL;

void tick ()
{
  phlxm->sq.tick();
}

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

/*
void changeTempo(int pot)
{
  tempo = baseTempo+pot;
  int millisPerTick = 60000/(tempo*ticksPerBeat);
  MsTimer2::stop();
  MsTimer2::set(millisPerTick, tick);
  MsTimer2::start();
}

*/