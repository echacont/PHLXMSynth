// echacont
// PHLXM source file
#include "PHLXM.h"

PHLXM::PHLXM(void)
{ }

void PHLXM::run(void)
{
  leds.update(sq.sqLeds);
  contrl.updateStatus();    // reads controller into status
  contrl.updateControl();          // what needs to be done. update program
  if (contrl.program.update == true)
    sq.progChange(contrl.program); // apply program change
  disp.update(contrl.program,
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
  // initialize the controller mode (state)
  controlMode.mode = PC;
  controlMode.trans = STOP;
  controlMode.pointer = 0;
  // initialize a program
  program.bank = 0;
  for (int i=0; i<NUM_UNISON_VOICES; i++) {
    program.voiceProgram[i] = INITIAL_PROGRAM;
  }
  program.update = true;
}

void Controller::updateStatus(void)
{
  for (int i = 0; i<NUM_POTS; i++) {
    status.potValue[i] = analogRead(i)>>3;
    status.potChanged[i] = false;
    if (status.potValue[i] != status_prev.potValue[i]) {
      status.potChanged[i] = true;
    }
    status_prev.potValue[i] = status.potValue[i];
  }
}

void Controller::updateControl()
{
  program.update = false;
  // state machine kinda
  switch (controlMode.mode)
  {
  case PC:  // program change
    if (status.potChanged[0] == true) {
      controlMode.pointer = status.potValue[0]>>5; // get 2 MSB
    }
    if (status.potChanged[1] == true) {
      program.voiceProgram[controlMode.pointer] = status.potValue[1]; // 7 bits
      program.update = true;
    }
    break;
  
  default:
    break;
  }
}
