// echacont
// FLXMSynth_seq header file

#ifndef FLXMSYNTH_SEQ_H
#define FLXMSYNTH_SEQ_H

// MIDI_Shield
#define PIN_LED_GRN         6
#define PIN_LED_RED         7
#define PIN_PLAY_INPUT      2
#define PIN_CONTINUE_INPUT  3
#define NUM_POTS            2
// LCD
#define COLUMS              20   //LCD columns
#define ROWS                2    //LCD rows
#define LCD_SPACE_SYMBOL    0x20 //space symbol from LCD ROM, see p.9 of GDM2004D datasheet
#define LCD_NOTE1_SYMBOL    0x91
// SEQUENCER
#define NUM_STEPS0          16      
#define TICKS_PER_STEP      4

// constantes en memoria
const int ticksPerBeat    = 4;
const int voices          = 8;
const int spread          = 420;
const int panspread       = 20;
const int baseTempo       = 60;
struct note {
  int pitch;
  int velocity;
};
typedef struct note note_t;
struct potStatus {
  bool status[NUM_POTS];
  int value[NUM_POTS];
};
typedef struct potStatus potStatus_t;

// state
int mode = 0;
int tempo = 120;        // BPM
potStatus_t potStatus_new, potStatus_prev;

// sequencer
bool steps0[NUM_STEPS0] = { true, true, true, true,
                            true, true, true, true,
                            true, true, true, true,
                            true, true, true, false  }; 

note_t seq0[NUM_STEPS0];

int  steps0_p = 0;
int  currentTicks = 0;

// scales
int root = 52;
int pitch = 52;
int scale0[7] = {2, 2, 1, 2, 2, 2, 1};     // major scale
int scale1[7] = {2, 1, 2, 2, 2, 1, 2};     // minor scale


#include <MsTimer2.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Fluxamasynth.h>
Fluxamasynth synth;
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);


void changeProgram(int program);

#endif //FLXMSYNTH_SEQ_H