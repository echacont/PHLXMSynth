// echacont
// PHLXM source file
#include "PHLXM.h"

PHLXM::PHLXM(void)
{ }

void PHLXM::run(void)
{
  leds.update(sq.sqLeds);
  contrl.updateStatus();          // reads controller into status
  contrl.updateControl(sq.seq);   // what needs to be done. update program
  sq.progChange(contrl.program);  // apply program change
  //sq.initSequence(contrl.root);
  disp.update(contrl.program,
              contrl.controlMode,
              contrl.status,
              sq.seq,
              contrl.root);

}

Controller::Controller(void)
{
  buttonPins[BUTTON_0] = PIN_BUTTON_0;
  buttonPins[BUTTON_1] = PIN_BUTTON_1;
  buttonPins[BUTTON_2] = PIN_BUTTON_2;

  for (int i = 0; i<NUM_POTS; i++) {
    status_prev.potChanged[i] = false;
    status_prev.potValue[i] = 0;
  }
  for (int i = 0; i<NUM_BUTTONS; i++) {
    status.buttonChanged[i] = false;
    status_prev.buttonValue[i] = false;
    // initialize Arduino pins
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  // debouncer
  for (int i=0; i<NUM_BUTTONS; i++)
    for (int j=0; j<NUM_DEBOUNCES; j++)
      buttondeBounce[j][i] = false;

  // initialize the controller mode
  controlMode.menu = SEQ;
  controlMode.trans = STOP;
  controlMode.pointer = 0;
  controlMode.count = 0;
  // initialize a program
  program.bank = 0;
  for (int i=0; i<NUM_UNISON_VOICES; i++) {
    program.voiceProgram[i] = INITIAL_PROGRAM;
  }
  program.update = true;

  root = DEFAULT_ROOT;
}

void Controller::updateStatus(void)
{
  // potentiometers
  for (int i = 0; i<NUM_POTS; i++) {
    status.potValue[i] = analogRead(i)>>3;
    status.potChanged[i] = false;
    if (status.potValue[i] != status_prev.potValue[i]) {
      status.potChanged[i] = true;
    }
    status_prev.potValue[i] = status.potValue[i];
  }
  // buttons
  for (int i = 0; i<NUM_BUTTONS; i++) {
    status.buttonChanged[i] = false;
    if (!digitalRead(buttonPins[i]))
      status.buttonValue[i] = true;
    else status.buttonValue[i] = false;
    
    for(int j=1; j<NUM_DEBOUNCES; j++) 
      buttondeBounce[j-1][i] = buttondeBounce[j][i];
    buttondeBounce[NUM_DEBOUNCES-1][i] = status.buttonValue[i];

    bool debounce = true;
    for(int j=0; j<NUM_DEBOUNCES; j++)
      // all recorded values in the buffer must be different
      // than previous value (button was pressed)
      if (buttondeBounce[j][i] == status_prev.buttonValue[i])
        debounce = false;
    if  (debounce && 
        (status.buttonValue[i] != status_prev.buttonValue[i])) {
      status.buttonChanged[i] = true;
      status_prev.buttonValue[i] = status.buttonValue[i];
    }
  }
}

void Controller::updateControl(int* seq)
{
  program.update = false;
  if (controlMode.menu != controlMode.prev_menu)
    controlMode.pointer = 0; // reset pointer if just entered menu
  // state machine kinda
  // transport machine uses buttons 1 (STOP) and 2 (PLAY/PAUSE)
  // TODO: transport machine
  switch (controlMode.menu)
  {
  case PC:  // program change
    if (status.potChanged[POT_0] == true) {
      controlMode.pointer = status.potValue[POT_0]>>5; // get 2 MSB
    }
    if (status.potChanged[POT_1] == true) {
      program.voiceProgram[controlMode.pointer] = status.potValue[POT_1]; // 7 bits
    }
    // Button 1 is Enter
    if (status.buttonChanged[BUTTON_1] && status.buttonValue[BUTTON_1])
      program.update = true;
    break;
  
  case SEQ:
    if (status.potChanged[POT_0] == true) {
      controlMode.pointer = status.potValue[POT_0]>>3; // get 4 MSB
    }
    if (status.potChanged[POT_1] == true) {
      seq[controlMode.pointer] = status.potValue[POT_1]>>4; // 3 bits
    }
    break;

  default:
    break;
  }

  // scroll the menu
  if(status.buttonChanged[BUTTON_0] && status.buttonValue[BUTTON_0]) {
    controlMode.prev_menu = controlMode.menu;
    controlMode.menu = controlMode.menu+1;
  }
  if (controlMode.menu == last) controlMode.menu = 0;

  // clear potChanged and buttonChanged flags
  for (int i=0; i<NUM_POTS; i++)
    status.potChanged[i] = false;
  for (int i=0; i<NUM_BUTTONS; i++)
    status.buttonChanged[i] = false;
  // clear debounce buffer

}
