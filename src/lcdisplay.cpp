// PHLXM liquid crystal display source file
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

#include "lcdisplay.h"
#include "PHLXM.h"

Lcdisp::Lcdisp(void)
{
  delay(100);
  LiquidCrystal_I2C(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
  begin(COLUMS, ROWS, LCD_5x8DOTS);
  clear();
  backlight();
  home();
  print("PHLXM synth");
  setCursor(0,1); write(0xBE); write(0x6B);
  delay(381);
  clear();
}

void Lcdisp::update(synthProgram_t program, 
                    controllerMode_t mode, 
                    sequencerState_t state,
                    extMIDIState_t midiState)
{
  home();
  if (mode.menuChanged) clear();

  switch(mode.menu)
  {         /*  LCD columns  */
    case PC:/*123456789012345*/
      print("Prog"); //write(LCD_SPACE_SYMBOL);
      setCursor(5,0);
      for (int i=0; i<NUM_UNISON_VOICES; i++) {
        write(LCD_0_SYMBOL+i+1);
        printPointer(mode.pointer, i, 6+i*3, 0);
        write(LCD_SPACE_SYMBOL);
      }
      setCursor(2,1);
      if (mode.option<0x10)  write(LCD_SPACE_SYMBOL);
      print(mode.option, HEX); write(LCD_RIGHT_SYMBOL);
      for (int i=0; i<NUM_UNISON_VOICES; i++) {
        setCursor(5+i*3,1);
        print(program.voiceProgram[i], HEX); write(LCD_SPACE_SYMBOL);
      }
      break;

    case GEN:
      setCursor(3,0); print("Vol");
      setCursor(7,0); print("Dt");
      setCursor(10,0); print("Pn");
      //setCursor(13,0); print("BPM");
      setCursor(0,1);
      if (mode.option<0x10)  write(LCD_SPACE_SYMBOL);
      print(mode.option, HEX); write(LCD_RIGHT_SYMBOL);
      printPointer(mode.pointer, 0, 3, 1);
      print(program.masterVol, HEX);
      printPointer(mode.pointer, 1, 7, 1);
      print(state.spread>>6, HEX);
      // External MIDI clock debug
      //setCursor(10,1); print(midiState.millisPerTick); 
      //write(LCD_SPACE_SYMBOL); write(LCD_SPACE_SYMBOL);
      
      printPointer(mode.pointer, 2, 10, 1);
      print(program.panspread>>4, HEX);
      printPointer(mode.pointer, 3, 13, 1);
      //if(mode.pointer == 3) print(mode.tempo, HEX); // should be state.tempo
      //else { setCursor(13,1); print(state.tempo); write(LCD_SPACE_SYMBOL); }
      

      break;

    case HARM_MODE:
      setCursor(4,0);
      print("Root");
      setCursor(9,0);
      print("St");
      setCursor(12,0);
      print("Nn");
      setCursor(15,0);
      print("O");
      // TODO print the root note (A,A#,B,C,C#,D,D#,E,F,F#,G,G#)
      setCursor(0,1);
      if (mode.option>99)  print(mode.option);
      else if (mode.option>9) { write(LCD_SPACE_SYMBOL); print(mode.option); }
      else {  write(LCD_SPACE_SYMBOL); write(LCD_SPACE_SYMBOL); print(mode.option);  }
      setCursor(3,1); write(LCD_RIGHT_SYMBOL);
      printPointer(mode.pointer, 0, 4, 1);
      if (state.root>99)  print(state.root);
      else if (state.root>9) { write(LCD_SPACE_SYMBOL); print(state.root); }
      else {  write(LCD_SPACE_SYMBOL); write(LCD_SPACE_SYMBOL); print(state.root);  }
      //setCursor(6,1); print(state.root);
      printPointer(mode.pointer, 1, 9, 1);
      setCursor(10,1);
      print(state.chordStep);
      printPointer(mode.pointer, 2, 12, 1);
      print(state.numChordNotes,HEX);
      printPointer(mode.pointer, 3, 15, 1);
      break;

    case SEQ:
      print("Step"); write(LCD_SPACE_SYMBOL);
      setCursor(6,0);
      for (int i=0; i<NUM_STEPS0; i++)
        write(LCD_0_SYMBOL+mode.pSeq[i]);
      setCursor(15,0);write(LCD_0_SYMBOL+state.currentStep+1);
      setCursor(6,1);
      for (int i=0; i<NUM_STEPS0; i++) 
        printPointer(mode.pointer, i, 6+i,1);
      setCursor(14,1); 
      write(LCD_LEFT_SYMBOL); write(LCD_0_SYMBOL+(mode.option));
      break;

    case SEQ_MODE:
      print("Seq Mode");
      break;

    case MIX:
      print("Mixer");
      break;

    case CHORUS:
      print("Chorus");
      break;

    case REVERB:
      print("Reverb");
      break;

    default:
      print("Rabit hole");
      break;
  }
}

void Lcdisp::printPointer(int p, int i, int col, int row)
{
  setCursor(col, row);
  if (i == p) write(LCD_STAR_SYMBOL);
  else write(LCD_SPACE_SYMBOL);
}

/*--------------------------------------------------*/

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


void Leds::update(bool leds[NUM_LEDS])
{
  for (int i = 0; i<NUM_LEDS; i++) {
    status[i] = leds[i];  // this is needed to keep status updated :)
    if(status[i]) digitalWrite(pin[i], LOW);
    else digitalWrite(pin[i], HIGH);
  }
}