// echacont
// PHLXM source file
#include "PHLXM.h"

PHLXM::PHLXM(void)
{
  // initialize variables
  mode = PC;
  trans = STOP;

}

Controller::Controller(void)
{
  for (int i = 0; i<NUM_POTS; i++) {
    potStatus_prev.status[i] = false;
    potStatus_prev.value[i] = 0;
  }
}

// sequencer constructor also calls base class constructor
Sequencer::Sequencer(void) : Fluxamasynth()
{

}

Lcdisp::Lcdisp(void)
{
  delay(100);
  lcd = LiquidCrystal_I2C(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
  lcd.begin(COLUMS, ROWS, LCD_5x8DOTS);
  lcd.clear();
  lcd.backlight();
  lcd.home();
  lcd.print("PHLXM is born");
  lcd.write(LCD_NOTE1_SYMBOL);
}

Leds::Leds(void)
{
  for (int i = 0; i<NUM_LEDS; i++)
    leds.status[i] = false;

  // LED outputs
  pinMode(PIN_LED_GRN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  digitalWrite(PIN_LED_GRN, HIGH);
  digitalWrite(PIN_LED_RED, HIGH);
}

void Leds::greenLed(bool on)
{
  if(on)
    digitalWrite(PIN_LED_GRN, LOW);
  else 
    digitalWrite(PIN_LED_GRN, HIGH);
}

void Leds::redLed(bool on)
{
  if(on)
    digitalWrite(PIN_LED_RED, LOW);
  else 
    digitalWrite(PIN_LED_RED, HIGH);
}