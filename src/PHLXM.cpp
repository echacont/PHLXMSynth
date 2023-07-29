// echacont
// PHLXM source file
#include "PHLXM.h"

PHLXM::PHLXM(void)
{
  // initialize variables
  mode = PC;
  trans = STOP;

}
void PHLXM::run(void)
{
  leds.update(sq.sqLeds);
  disp.update(sq.millisPerTick);
}

Controller::Controller(void)
{
  for (int i = 0; i<NUM_POTS; i++) {
    potStatus_prev.status[i] = false;
    potStatus_prev.value[i] = 0;
  }
}

Step::Step()
{
  nextStep = 0;
  enable = false;
}

Step::setStep(int step, int pitch)
{
  if (step < NUM_STEPS0) nextStep = step+1;
  else nextStep = 0;
  enable = true;
  notes[0].setPitch(pitch);
}

// sequencer constructor also calls base class constructor
Sequencer::Sequencer(void) : Fluxamasynth()
{
  for (int i=0; i<voices; i++) {
    synth.allNotesOff(i); 
    synth.setChannelVolume(i, 32);  
    synth.pan(i,127-((i-voices/2)*panspread));
    synth.setChorus(i, 6, 32, 80, 8 );
    synth.setReverb(i, 7, 72, 72);
  }
  synth.setMasterVolume(72);

  millisPerTick = 60000/(tempo*ticksPerBeat); // 250
  for(int i=0; i<NUM_LEDS; i++)
    sqLeds[i] = false;
}

void Sequencer::tick(void)
{
  sqLeds[1] = !sqLeds[1];
}

void Sequencer::initSequences(void)
{
  int interval = 0;
  int degree = 0;
  int scaleSize = sizeof(minor_scale1)/sizeof(int);
  for(int step = 0; step < NUM_STEPS0; step++) {
    if (step != 0) interval = interval + minor_scale1[degree];
    int pitch = root+interval;
    seq[step].setStep(step,pitch);
    //seq[step].velocity = 100;

    if (degree > scaleSize) degree = 0;
  }
}

Lcdisp::Lcdisp(void)
{
  delay(100);
  lcd = LiquidCrystal_I2C(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
  lcd.begin(COLUMS, ROWS, LCD_5x8DOTS);
  lcd.clear();
  lcd.backlight();
  lcd.home();
  lcd.print("PHLXM was born");
  lcd.write(LCD_NOTE1_SYMBOL);
  delay(3000);
  lcd.clear();
}

void Lcdisp::update(int value)
{
  lcd.home();
  lcd.setCursor(0,1);
  lcd.print(value);
  lcd.write(LCD_SPACE_SYMBOL);
}


Leds::Leds(void)
{
  pin[0] = PIN_LED_GRN;
  pin[1] = PIN_LED_RED;
  for (int i = 0; i<NUM_LEDS; i++) {
    status[i] = false;
    pinMode(pin[i], OUTPUT);
    digitalWrite(pin[i], HIGH);
  }
}

void Leds::update(void)
{
  for (int i = 0; i<NUM_LEDS; i++) {
    if(status[i]) digitalWrite(pin[i], LOW);
    else digitalWrite(pin[i], HIGH);
  }
}
void Leds::update(bool leds[NUM_LEDS])
{
  for (int i = 0; i<NUM_LEDS; i++) {
    status[i] = leds[i];
    if(status[i]) digitalWrite(pin[i], LOW);
    else digitalWrite(pin[i], HIGH);
  }
}
void Leds::update(int led, bool status)
{
  if(status) digitalWrite(pin[led], LOW);
  else digitalWrite(pin[led], HIGH);
}
Note::Note(void)
{
  pitch = DEFAULT_ROOT;
  velocity = 100;
  gate = 1;
  start = 0;
}
Note::setPitch(int p)
{
  pitch = p;
  velocity = 100;
  gate = 1;
  start = 0;
}
Note::setNote(int p, int v, int g, int s)
{
  pitch = p;
  velocity = v;
  gate = g;
  start = s;
}
