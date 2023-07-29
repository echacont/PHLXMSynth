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

void Lcdisp::update(Program program, controllerMode_t controlMode)
{
  home();  
  switch(controlMode.mode)
  {         /*  LCD columns  */
    case PC:/*123456789012345*/
      print("Prog 1  2  3  4");
      setCursor(0,1);
      print(controlMode.pointer+1);
      for (int i=0; i<NUM_UNISON_VOICES; i++) {
        setCursor(5+i*3,1);
        print(program.voiceProgram[i], HEX);
        write(LCD_SPACE_SYMBOL);
      }
      break;
    case SEQ:
      break;
    case MODE:
      break;
    case FX:
      break;
    default:
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