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
#define MAX_NOTES           64
#define TICKS_PER_STEP      4
#define TICKS_PER_BEAT      4
#define BASE_TEMPO          60
#define DEFAULT_ROOT        52  // E
#define NUM_UNISON_VOICES   8
#define UNISON_PITCH_SPREAD 320
#define UNISON_PAN_SPREAD   20

// music defines
#define SCALE_SIZE          7


#endif // DEFINES_H