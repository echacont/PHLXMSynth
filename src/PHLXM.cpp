// echacont
// PHLXM source file
#include "PHLXM.h"

PHLXM::PHLXM(void)
{ }

void PHLXM::run(void)
{
  leds.update(sq.sqLeds);
  contrl.updateController();     // reads controller bits
  contrl.updateMode(sq.currentProgram);           // what needs to be done?
  sq.progChange();
  disp.update(sq.currentProgram,
              contrl.controlMode);

}

Controller::Controller(void)
{
  for (int i = 0; i<NUM_POTS; i++) {
    status_prev.potChanged[i] = false;
    status_prev.potValue[i] = 0;
  }
  for (int i = 0; i<NUM_BUTTONS; i++) {
    status_prev.buttonChanged[i] = false;
    status_prev.buttonValue[i] = 0;
  }
  controlMode.mode = PC;
  controlMode.trans = STOP;
  controlMode.pointer = 0;
}

void Controller::updateController(void)
{
  for (int i = 0; i<NUM_POTS; i++) {
    status.potValue[i] = analogRead(i)>>3;
    status.potChanged[i] = false;
    if (status.potValue[i] != status_prev.potValue[i]) {
      status.potChanged[i] = true;
    }
  }  
}

void Controller::updateMode(Program program)
{
  // state machine kinda
  switch (controlMode.mode)
  {
  case PC:  // program change
    if (status.potChanged[0])
      controlMode.pointer = status.potValue[0]>>5; // get 2 MSB
    if (status.potChanged[1])
      program.voiceProgram[controlMode.pointer] = status.potValue[1]; // 7 bits
    break;
  
  default:
    break;
  }

}

controllerStatus_t Controller::updateController(controllerStatus_t status_prev) 
{
  controllerStatus_t status_new; 

  for (int i = 0; i<NUM_POTS; i++) {
    status_new.potValue[i] = analogRead(i)>>3;
    status_new.potChanged[i] = false;
    if (status_new.potValue[i] != status_prev.potValue[i]) {
      status_new.potChanged[i] = true;
    }
  }
  return status_new;
}
