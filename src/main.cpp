#include <Arduino.h>
#include <MsTimer2.h>

#include "defines.h"
#include "PHLXM.h"
#include "lcdisplay.h"


PHLXM* phlxm = NULL;

void tick()
{
  phlxm->sq.tick();
}

void setup() 
{
  phlxm = new PHLXM();
  MsTimer2::set(phlxm->sq.millisPerTick, tick);
  MsTimer2::start();
}

void loop() 
{
  phlxm->run();
};

/*
void changeProgram(int program) 
{
  if (program > 127) program = 127;
  if (program < 0) program = 0;
  for (int i=0; i<voices; i++) 
    synth.programChange(127, i, program);
}

void changeTempo(int pot)
{
  tempo = baseTempo+pot;
  int millisPerTick = 60000/(tempo*ticksPerBeat);
  MsTimer2::stop();
  MsTimer2::set(millisPerTick, tick);
  MsTimer2::start();
}

potStatus_t potentiometers(potStatus_t potStatus_prev)
{
  potStatus_t potStatus_new; 
  potStatus_new.value[0] = analogRead(0)>>3;
  potStatus_new.value[1] = analogRead(1)>>3;
  for (int i = 0; i<NUM_POTS; i++) {
    potStatus_new.status[i] = false;
    if (potStatus_new.value[i] != potStatus_prev.value[i]) {
      potStatus_new.status[i] = true;
    }
  }
  return potStatus_new;
}

void update(potStatus_t potStatus)
{
  for(int p=0; p<NUM_POTS; p++) {
    if (potStatus.status[p]) {

      switch(mode) {
        // program change
        case 0:
          switch(p) {
            case 0: 
              changeProgram(potStatus.value[p]);
              break;
            case 1: 
              changeTempo(potStatus.value[p]);
              break;
          }
          break;
        default:
          break;
      }

    }
  }
}

*/