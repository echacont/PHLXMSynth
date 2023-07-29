// echacont
// FLXMSynth_seq defines file

#ifndef DEFINES_H
#define DEFINES_H

// MIDI_Shield
#define PIN_LED_GRN         6
#define PIN_LED_RED         7
#define PIN_PLAY_INPUT      2
#define PIN_CONTINUE_INPUT  3
#define NUM_POTS            2
#define NUM_BUTTONS         3
#define NUM_LEDS            2
#define GRN_LED             0
#define RED_LED             1
// LCD
#define COLUMS              20   //LCD columns
#define ROWS                2    //LCD rows
#define LCD_SPACE_SYMBOL    0x20 //space symbol from LCD ROM, see p.9 of GDM2004D datasheet
#define LCD_NOTE1_SYMBOL    0x91
// SEQUENCER
#define NUM_STEPS0          16      
#define TICKS_PER_STEP      4
#define BASE_TEMPO          60

// enumerated types
enum mode_e { PC, MODE, FX };
enum transport_e { STOP, PLAY, PAUSE };

// data structures
struct note {
  int pitch;
  int velocity;
};

struct potStatus {
  bool status[NUM_POTS];
  int  value[NUM_POTS];
};

struct buttonStatus {
  bool status[NUM_BUTTONS];
  bool value[NUM_BUTTONS];
};

typedef struct note note_t;
typedef struct potStatus potStatus_t;
typedef struct buttonStatus buttonStatus_t;

struct step {
  int nextStep;
  bool enable;
  int gate;
  note_t notes[];
};

typedef struct step step_t;

struct leds {
  bool status[NUM_LEDS];
  int  pin[NUM_LEDS];
};

typedef struct leds leds_t;

#endif // DEFINES_H