#include <Arduino.h>

#include "defines.h"
#include "PHLXM.h"


// construct top system object
PHLXM phlxm();

void setup () {};
void loop  () {};

/*
void changeProgram(int program) 
{
  if (program > 127) program = 127;
  if (program < 0) program = 0;
  for (int i=0; i<voices; i++) 
    synth.programChange(127, i, program);
}

void tick()
{
  static boolean output = HIGH;
  digitalWrite(PIN_LED_RED, output);
  output = !output;

  bool play = true;
  if(play)
  {
    if(steps0_p == 0 ) {
      for(int i = 0; i < voices; i++) {
        synth.allNotesOff(i);
      }
    }
    if(steps0[steps0_p])
    {
      digitalWrite(PIN_LED_GRN, LOW);
      if (currentTicks == 0)
      {
        for(int i = 0; i < voices; i++) {
          synth.noteOn(i, seq0[steps0_p].pitch, seq0[steps0_p].velocity);
          synth.pitchBend(i, 512 + (i-voices/2)*(spread/voices));
        }
      }
    } 
    else  // no sound on this step
    {
      digitalWrite(PIN_LED_GRN, HIGH);
      //for(int i = 0; i < voices; i++) {
        //ynth.noteOff(i,pitch);
        //synth.allNotesOff(i);
      //}
    } 
    
    // tempo indicator (RED LED)
    if(currentTicks == 0)
    {
      digitalWrite(PIN_LED_RED, LOW);

    } else {
      digitalWrite(PIN_LED_RED, HIGH);
    }

    // overflow control
    currentTicks++;
    if (currentTicks == TICKS_PER_STEP) 
    {
      currentTicks = 0;
      steps0_p++;
      if (steps0_p == NUM_STEPS0) steps0_p = 0;
    }
  } // if (play)
  else { // stop
    digitalWrite(PIN_LED_GRN, HIGH);
    digitalWrite(PIN_LED_RED, HIGH);
    for (int i=0; i<voices; i++) {
      synth.allNotesOff(i);
    }
  }
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

void initSequences(void)
{
  int interval = 0;
  int degree = 0;
  int scaleSize = sizeof(scale1)/sizeof(int);
  for(int step = 0; step < NUM_STEPS0; step++) {
    if (step != 0) interval = interval + scale1[degree];
    seq0[step].pitch = root+interval;
    seq0[step].velocity = 100;
    if (degree > scaleSize) degree = 0;
  }
}

void display(potStatus_t potStatus) 
{
  lcd.home();
  // print header. TODO: only do if theres a change
  switch(mode) 
  {
    case 0:   // Program Change
      lcd.print("Prog  BPM seq");
      break;
    default:
      break;
  }
  lcd.setCursor(0,1);
  for(int p=0; p<NUM_POTS; p++)
  {
    if (potStatus.status[p])
    {
      // print pot values to screen
      lcd.setCursor(3*p,1);
      lcd.print(potStatus.value[p], HEX);
      lcd.write(LCD_SPACE_SYMBOL);
      // print tempo if change in pot 1
      if (p == 1) {
        lcd.setCursor(6,1);
        lcd.print(tempo);
        lcd.write(LCD_SPACE_SYMBOL);
      }
    }
  }
}

void setup() 
{
  // LED outputs
  pinMode(PIN_LED_GRN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  digitalWrite(PIN_LED_GRN, HIGH);
  digitalWrite(PIN_LED_RED, HIGH);

  //randomSeed(analogRead(0));
  delay(100);

  initSequences();

  for (int i=0; i<voices; i++) {
    synth.allNotesOff(i); 
    synth.setChannelVolume(i, 32);  
    synth.pan(i,127-((i-voices/2)*panspread));
    synth.setChorus(i, 6, 32, 80, 8 );
    synth.setReverb(i, 7, 72, 72);
  }

  synth.setMasterVolume(72);
  //changeProgram(69);

  lcd.begin(COLUMS, ROWS, LCD_5x8DOTS);
  lcd.clear();
  lcd.backlight();

  int millisPerTick = 60000/(tempo*ticksPerBeat);
  MsTimer2::set(millisPerTick, tick); // 500ms period
  MsTimer2::start();

}

void loop() {
  potStatus_prev = potStatus_new;
  potStatus_new = potentiometers(potStatus_prev);

  update(potStatus_new);
  display(potStatus_new);

}
*/