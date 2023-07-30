// echacont
// PHLXM liquid crystal display source file

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
  delay(381);
  clear();
}

void Lcdisp::update(synthProgram_t program, 
                    controllerMode_t mode, 
                    sequencerState_t state)
                   // controllerStatus_t status) // get rid of this
{
  home();
  switch(mode.menu)
  {         /*  LCD columns  */
    case PC:/*123456789012345*/
      print("Prog 1  2  3  4");
      setCursor(0,1);
      print(mode.pointer+1); write(LCD_SPACE_SYMBOL);
      for (int i=0; i<NUM_UNISON_VOICES; i++) {
        setCursor(5+i*3,1);
        print(program.voiceProgram[i], HEX); write(LCD_SPACE_SYMBOL);
      }
      break;
    case SPREAD:
      print("Sprd   bend pan ");
      break;
    case HARM_MODE:
      print("Harm            ");  
      break;

    case SEQ:
      print("Step"); write(LCD_SPACE_SYMBOL);
      setCursor(6,0);
      for (int i=0; i<NUM_STEPS0; i++)
        write(LCD_0_SYMBOL+mode.pSeq[i]);
      setCursor(15,0);write(LCD_0_SYMBOL+state.currentStep+1);
      // TODO print the root note (A,A#,B,C,C#,D,D#,E,F,F#,G,G#)
      setCursor(0,1); print("r "); print(mode.root); write(LCD_SPACE_SYMBOL);
      setCursor(6,1);
      for (int i=0; i<NUM_STEPS0; i++) 
        if (i == mode.pointer) write(LCD_STAR_SYMBOL);
        else write(LCD_SPACE_SYMBOL);
      setCursor(15,1); write(LCD_0_SYMBOL+(mode.option));
      break;

    case FX:
      print("FX              ");
      break;
    default:
      print("Go away!        ");
      break;
  }
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