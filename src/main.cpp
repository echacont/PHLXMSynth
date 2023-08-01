// PHLXM sequencer->synth main.cpp
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

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
