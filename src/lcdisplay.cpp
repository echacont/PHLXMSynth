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
                    fineStepSequence tseq)
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
      setCursor(13,0); print("Div");
      setCursor(0,1);
      if (mode.option<0x10)  write(LCD_SPACE_SYMBOL);
      print(mode.option, HEX); write(LCD_RIGHT_SYMBOL);
      printPointer(mode.pointer, 0, 3, 1);
      print(program.masterVol, HEX);
      printPointer(mode.pointer, 1, 7, 1);
      print(state.spread>>6, HEX);
      printPointer(mode.pointer, 2, 10, 1);
      print(program.panspread>>4, HEX);
      printPointer(mode.pointer, 3, 13, 1);
      print(state.divisor, HEX); write(LCD_SPACE_SYMBOL);
      break;

    case HARM_MODE:
      print ("H_M");
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
      setCursor(0,1);
      print(state.currentTick);  write(LCD_SPACE_SYMBOL);
      setCursor(3,1);
      if (tseq.onSeq[state.currentTick] > 0) print(tseq.onSeq[state.currentTick], HEX);  write(LCD_SPACE_SYMBOL);
      setCursor(6,1);
      for (int i=0; i<NUM_STEPS0; i++) 
        printStepAndPointer(mode.pointer, state.currentStep, i, 6+i,1);
      setCursor(14,1); 
      write(LCD_LEFT_SYMBOL); write(LCD_0_SYMBOL+(mode.option));
      break;

    case SEQ_MODE:
      print("SqM");
      setCursor(4,0);
      print("Mod");
      setCursor(0,1);
      if (mode.option>99)  print(mode.option);
      else if (mode.option>9) { write(LCD_SPACE_SYMBOL); print(mode.option); }
      else {  write(LCD_SPACE_SYMBOL); write(LCD_SPACE_SYMBOL); print(mode.option);  }        
      setCursor(3,1); write(LCD_RIGHT_SYMBOL);
      printPointer(mode.pointer, 0, 4, 1);
      setCursor(5,1); 
      if (state.mode == CHORD) print("ch");
      else if (state.mode == ARP1) print ("a1");      
      break;


    case MIX:
      print("Mix");
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
        print(program.voiceVol[i], HEX); write(LCD_SPACE_SYMBOL);
      }
      break;

    case CHORUS:
      print("Chr");
      setCursor(4,0);
      print("Vo");
      printPointer(mode.pointer, 1, 7, 0);
      setCursor(8,0); // two behaviors: if this is selected, scroll through options using pot value
      if (mode.pointer == 1) {
        if      (mode.value>>4 == CHORUS_LEVEL ) { print("Level  ");    }
        else if (mode.value>>4 == CHORUS_TYPE  ) { print("Type   ");     }
        else if (mode.value>>4 == CHORUS_DELAY ) { print("Delay  ");    }
        else if (mode.value>>4 == CHORUS_FDBK  ) { print("Fdbak  "); }
        else if (mode.value>>4 == CHORUS_RATE  ) { print("Rate   ");     }
        else if (mode.value>>4 == CHORUS_DEPTH ) { print("Depth  ");    }
        else {                                     print("----   "); }
        if (mode.value>>4 == mode.fxParam) write(LCD_LEFT_SYMBOL);
        else write(LCD_SPACE_SYMBOL); 
      } else { // else show current parameter (program.fxParam)
        if      (mode.fxParam == CHORUS_LEVEL ) { print("Level  ");    }
        else if (mode.fxParam == CHORUS_TYPE  ) { print("Type   ");     }
        else if (mode.fxParam == CHORUS_DELAY ) { print("Delay  ");    }
        else if (mode.fxParam == CHORUS_FDBK  ) { print("Fdbck  "); }
        else if (mode.fxParam == CHORUS_RATE  ) { print("Rate   ");     }
        else if (mode.fxParam == CHORUS_DEPTH ) { print("Depth  ");    }
        else {                                    print("----   "); }
        write(LCD_LEFT_SYMBOL);
      }
      setCursor(1,1);
      //if (mode.value<0x10)  write(LCD_SPACE_SYMBOL);      // Make this into a function
      //print(mode.value, HEX); write(LCD_RIGHT_SYMBOL);
      if (mode.pointer == 0) { // voice selection
        write(LCD_SPACE_SYMBOL);
        print((mode.value>>5)+1); // voice 1, 2, 3 or 4
        setCursor(3,1); write(LCD_RIGHT_SYMBOL);
      } else if (mode.pointer == 1) { // parameter being chosen, display nothing here
        write(LCD_SPACE_SYMBOL);write(LCD_SPACE_SYMBOL);write(LCD_SPACE_SYMBOL);
      } else { // parameter value selection. Range depends on what parameter
        if      (mode.fxParam == CHORUS_LEVEL ) { print(mode.value, HEX);    }
        else if (mode.fxParam == CHORUS_TYPE  ) { print(mode.value>>4, HEX);    }
        else if (mode.fxParam == CHORUS_DELAY ) { print(mode.value>>4, HEX);    }
        else if (mode.fxParam == CHORUS_FDBK  ) { print(mode.value>>4, HEX);    }
        else if (mode.fxParam == CHORUS_RATE  ) { print(mode.value>>4, HEX);    }
        else if (mode.fxParam == CHORUS_DEPTH ) { print(mode.value>>4, HEX);    }
        else {                                    print("--"); }; 
        setCursor(3,1); write(LCD_RIGHT_SYMBOL);
      }
      // selected voice number (mode.option)
      printPointer(mode.pointer, 0, 4, 1);
      print(mode.option+1); // select voice 1, 2, 3 or 4
      // print current parameter value (programmed)
      setCursor(7,1);
      if (mode.pointer > 1) write(LCD_STAR_SYMBOL);
      else write(LCD_SPACE_SYMBOL);
      setCursor(8,1); 
      if (mode.pointer == 1) { // two behaviors: if selecting parameter, show current value
        if      (mode.value>>4 == CHORUS_LEVEL ) { print(program.chorusLevel[mode.option], HEX);    }
        else if (mode.value>>4 == CHORUS_TYPE  ) { print(program.chorusType[mode.option], HEX); } // chorus type
        else if (mode.value>>4 == CHORUS_DELAY ) { print(program.chorusDelay[mode.option], HEX);    }
        else if (mode.value>>4 == CHORUS_FDBK  ) { print(program.chorusFdbk[mode.option], HEX); }
        else if (mode.value>>4 == CHORUS_RATE  ) { print(program.chorusRate[mode.option], HEX);     }
        else if (mode.value>>4 == CHORUS_DEPTH ) { print(program.chorusDepth[mode.option], HEX);    }
        else {                                     print("--"); }
      } else {  // else, show value corresponding to selected parameter (fxParam)
        if      (mode.fxParam == CHORUS_LEVEL ) { print(program.chorusLevel[mode.option], HEX);    }
        else if (mode.fxParam == CHORUS_TYPE  ) { print(program.chorusType[mode.option], HEX); } // chorus type
        else if (mode.fxParam == CHORUS_DELAY ) { print(program.chorusDelay[mode.option], HEX);    }
        else if (mode.fxParam == CHORUS_FDBK  ) { print(program.chorusFdbk[mode.option], HEX); }
        else if (mode.fxParam == CHORUS_RATE  ) { print(program.chorusRate[mode.option], HEX);     }
        else if (mode.fxParam == CHORUS_DEPTH ) { print(program.chorusDepth[mode.option], HEX);    }
        else {                                    print("--"); } 
      }
      write(LCD_SPACE_SYMBOL);
      break; // case CHORUS

    case REVERB:
      print("Rvb");
      setCursor(4,0);
      print("Vo");
      printPointer(mode.pointer, 1, 7, 0);
      setCursor(8,0); // two behaviors: if this is selected, scroll through options using pot value
      if (mode.pointer == 1) {
        if      (mode.value>>5 == REVERB_LEVEL ) { print("Level  ");    }
        else if (mode.value>>5 == REVERB_TYPE  ) { print("Type   ");     }
        else if (mode.value>>5 == REVERB_FDBK  ) { print("Fdbak  "); }
        else {                                     print("----   "); }
        if (mode.value>>5 == mode.fxParam) write(LCD_LEFT_SYMBOL);
        else write(LCD_SPACE_SYMBOL); 
      } else { // else show current parameter (program.fxParam)
        if      (mode.fxParam == REVERB_LEVEL ) { print("Level  ");    }
        else if (mode.fxParam == REVERB_TYPE  ) { print("Type   ");     }
        else if (mode.fxParam == REVERB_FDBK  ) { print("Fdbck  "); }
        else {                                    print("----   "); }
        write(LCD_LEFT_SYMBOL);
      }
      setCursor(1,1);
      //if (mode.value<0x10)  write(LCD_SPACE_SYMBOL);      // Make this into a function
      //print(mode.value, HEX); write(LCD_RIGHT_SYMBOL);
      if (mode.pointer == 0) { // voice selection
        write(LCD_SPACE_SYMBOL);
        print((mode.value>>5)+1); // voice 1, 2, 3 or 4
        setCursor(3,1); write(LCD_RIGHT_SYMBOL);
      } else if (mode.pointer == 1) { // parameter being chosen, display nothing here
        write(LCD_SPACE_SYMBOL);write(LCD_SPACE_SYMBOL);write(LCD_SPACE_SYMBOL);
      } else { // parameter value selection. Range depends on what parameter
        if      (mode.fxParam == REVERB_LEVEL ) { print(mode.value, HEX);    }
        else if (mode.fxParam == REVERB_TYPE  ) { print(mode.value>>4, HEX);    }
        else if (mode.fxParam == REVERB_FDBK  ) { print(mode.value, HEX);    }
        else {                                    print("--"); }; 
        setCursor(3,1); write(LCD_RIGHT_SYMBOL);
      }
      // selected voice number (mode.option)
      printPointer(mode.pointer, 0, 4, 1);
      print(mode.option+1); // select voice 1, 2, 3 or 4
      // print current parameter value (programmed)
      setCursor(7,1);
      if (mode.pointer > 1) write(LCD_STAR_SYMBOL);
      else write(LCD_SPACE_SYMBOL);
      setCursor(8,1); 
      if (mode.pointer == 1) { // two behaviors: if selecting parameter, show current value
        if      (mode.value>>5 == REVERB_LEVEL ) { print(program.reverbLevel[mode.option], HEX);    }
        else if (mode.value>>5 == REVERB_TYPE  ) { print(program.reverbType[mode.option], HEX); } // reverb type
        else if (mode.value>>5 == REVERB_FDBK  ) { print(program.reverbFdbk[mode.option], HEX); }
        else {                                     print("--"); }
      } else {  // else, show value corresponding to selected parameter (fxParam)
        if      (mode.fxParam == REVERB_LEVEL ) { print(program.reverbLevel[mode.option], HEX);    }
        else if (mode.fxParam == REVERB_TYPE  ) { print(program.reverbType[mode.option], HEX); } // reverb type
        else if (mode.fxParam == REVERB_FDBK  ) { print(program.reverbFdbk[mode.option], HEX); }
        else {                                    print("--"); } 
      }
      write(LCD_SPACE_SYMBOL);

      break; // case: REVERB

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

void Lcdisp::printStepAndPointer(int p, int8_t s, int i, int col, int row)
{
  setCursor(col, row);
  if (i == p) write(LCD_STAR_SYMBOL);
  else if (i == s) write(LCD_SQUARE_SYMBOL);
  else write(LCD_SPACE_SYMBOL);
}

/*--------------------------------------------------*/
