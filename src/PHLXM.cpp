// echacont
// PHLXM source file
#include "PHLXM.h"

PHLXM::PHLXM(void)
{
  // initialize variables
  //mode = PC;
}
void PHLXM::run(void)
{
  leds.update(sq.sqLeds);
  disp.update(sq.currentStep, sq.currentTick, sq.seq[sq.currentStep]);
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
  tempo = BASE_TEMPO;
  trans = STOP;
  voices = NUM_UNISON_VOICES;
  spread = UNISON_PITCH_SPREAD;
  panspread = UNISON_PAN_SPREAD;
  millisPerTick = 60000/(tempo*TICKS_PER_BEAT); // 250    
  currentTick = 0;
  currentStep = 0;
  for (int i=0; i<NUM_LEDS; i++) sqLeds[i] = false;
  setMasterVolume(72);
  for (int i=0; i<voices; i++) {
    allNotesOff(i); 
    setChannelVolume(i, 32);  
    pan(i,127-((i-voices/2)*panspread));
    setChorus(i, 6, 32, 80, 8 );
    setReverb(i, 7, 72, 72);
  }

  initSequences(DEFAULT_ROOT);
}

void Sequencer::tick(void)
{
  sqLeds[1] = !sqLeds[1];

  playStep(currentStep);

  currentTick++;
  if (currentTick == TICKS_PER_STEP) 
  {
    currentTick = 0;
    currentStep++;
    if (currentStep == NUM_STEPS0) currentStep = 0;
  }
}

void Sequencer::playStep(int step)
{
  if (currentTick == 0) 
    for(int i = 0; i < voices; i++) {
      noteOn(i, seq[step], 100);
      pitchBend(i, 512 + (i-voices/2)*(spread/voices));
    }
  if (currentTick == TICKS_PER_STEP-1)
    for(int i = 0; i < voices; i++) {
      noteOff(i, seq[step]);
  }
}

void Sequencer::initSequences(int root)
{
  int interval = 0;
  int degree = 0;
  int scaleSize = SCALE_SIZE;
  for(int step = 0; step < NUM_STEPS0; step++) {
    degree = step%scaleSize;
    if (step == 0) interval = 0;
    else interval = interval+minor_scale1[degree];
    seq[step]= root+interval;
  } 
}

Lcdisp::Lcdisp(void)
{
  delay(100);
  LiquidCrystal_I2C(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
  begin(COLUMS, ROWS, LCD_5x8DOTS);
  clear();
  backlight();
  home();
  print("PHLXM was born");
  write(LCD_NOTE1_SYMBOL);
  delay(1081);
  clear();
}

void Lcdisp::update(int value)
{
  home();
  setCursor(0,1);
  print(value);
  write(LCD_SPACE_SYMBOL);
}
void Lcdisp::update(int value1, int value2)
{
  home();
  setCursor(0,1);
  print(value1);
  write(LCD_SPACE_SYMBOL);
  setCursor(3,1);
  print(value2);
  write(LCD_SPACE_SYMBOL);
}

void Lcdisp::update(int value1, int value2, int value3)
{
  home();
  setCursor(0,1);
  print(value1);
  write(LCD_SPACE_SYMBOL);
  setCursor(3,1);
  print(value2);
  write(LCD_SPACE_SYMBOL);
  setCursor(6,1);
  print(value3);
  write(LCD_SPACE_SYMBOL);
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
