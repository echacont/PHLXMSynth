// PHLXM sequencer->synth defines file
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

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
#define PIN_BUTTON_2        2
#define PIN_BUTTON_1        3
#define PIN_BUTTON_0        4
#define NUM_DEBOUNCES       2
#define BUTTON_0            0
#define BUTTON_1            1
#define BUTTON_2            2
#define POT_0               0
#define POT_1               1
// LCD
#define COLUMS              20   //LCD columns
#define ROWS                2    //LCD rows
#define LCD_SPACE_SYMBOL    0x20 //space symbol from LCD ROM, see p.9 of GDM2004D datasheet
#define LCD_0_SYMBOL        0x30
#define LCD_STAR_SYMBOL     0x2A
#define LCD_SQUARE_SYMBOL   0xDB
#define LCD_RIGHT_SYMBOL    0x7E
#define LCD_LEFT_SYMBOL    0x7F

  // SEQUENCER
#define NUM_STEPS0          8
#define MAX_NOTES           64
#define TICKS_PER_BEAT      24  // not being used
#define BASE_TEMPO          60
#define DEFAULT_ROOT        52  // E
#define NUM_UNISON_VOICES   4
#define POLYPHONY           4   
#define NUM_CHORD_NOTES     4
#define CHORD_STEP          5
#define CHORD_MODE          0
#define ARP1_MODE           1
#define ARP2_MODE           2
// default program
#define UNISON_PITCH_SPREAD 64
#define UNISON_PAN_SPREAD   32
#define INITIAL_BANK        127 // MT-32
#define INITIAL_PROGRAM_1   31  // Fantasia
#define INITIAL_PROGRAM_2   38  // Atmosphere
#define INITIAL_PROGRAM_3   112 // Pan Flute
#define INITIAL_PROGRAM_4   93  // French Horn
#define INITIAL_MASTER_VOL  64
#define INITIAL_VOICE_VOL   64

// MIDI
#define MIDI_TICKS_PER_BEAT 48    // this is the default sequencer divisor
#define MIDI_CHANNEL        15

// music defines
#define SCALE_SIZE          7

// chorus
#define CHORUS_LEVEL        0
#define CHORUS_TYPE         1
#define CHORUS_DELAY        2
#define CHORUS_FDBK         3
#define CHORUS_RATE         4
#define CHORUS_DEPTH        5
// default chorus program
#define DEFAULT_CHORUS_LEVEL        0x30
#define DEFAULT_CHORUS_TYPE         6
#define DEFAULT_CHORUS_DELAY        0x30
#define DEFAULT_CHORUS_FDBK         0x10
#define DEFAULT_CHORUS_RATE         0x0
#define DEFAULT_CHORUS_DEPTH        0x0

// reverb
#define REVERB_LEVEL        0
#define REVERB_TYPE         1
#define REVERB_FDBK         3
// default reverb program
#define DEFAULT_REVERB_LEVEL        0x80
#define DEFAULT_REVERB_TYPE         3
#define DEFAULT_REVERB_FDBK         0x40

// enumerated types
enum mode_e { GEN, HARM_MODE, PC, MIX, SEQ, SEQ_MODE, CHORUS, REVERB, last };
enum transport_e { STOP, PLAY, PAUSE };
//enum reverb_parameter_e { VOL, PROG, DELAY, FEEDB, RATE, DEPTH, last };
// sequencer modes
enum sq_mode_e { CHORD, ARP1, ARP2 };

struct controllerStatus {
  bool potChanged[NUM_POTS];        // indicates if there was change
  bool buttonChanged[NUM_BUTTONS];  // indicates if there was change
  int  potValue[NUM_POTS];
  bool buttonValue[NUM_BUTTONS];
};
typedef struct controllerStatus controllerStatus_t;

struct controllerMode {
  mode_e menu;
  transport_e trans;

  // External MIDI input
  int root;     // input note

  // user interface
  bool updateSeq;
  bool menuChanged;
  bool allNotesOff;
  int pointer;
  int value;
  int option;
  int fxParam;
  int fxValue;
  int arpMode;
  
  // can't this go into the sequencer?
  int numChordNotes;
  int chordStep;
  int divisor;    // Ext MIDI tick divisor

  int spread;     // unison detune using midi note bend
  int panspread;


  // user control sequence
  // TODO: this could modulate something else????
  int pSeq[NUM_STEPS0];
}; 
typedef struct controllerMode controllerMode_t;

struct Program_s
{
  int bank;
  int voiceProgram[NUM_UNISON_VOICES];
  int voicePan[NUM_UNISON_VOICES];
  int voiceVol[NUM_UNISON_VOICES];
  int masterVol;
  // FX
  int chorusType[NUM_UNISON_VOICES];
  int chorusLevel[NUM_UNISON_VOICES];
  int chorusDelay[NUM_UNISON_VOICES];
  int chorusFdbk[NUM_UNISON_VOICES];
  int chorusRate[NUM_UNISON_VOICES];
  int chorusDepth[NUM_UNISON_VOICES];
  int reverbType[NUM_UNISON_VOICES];
  int reverbLevel[NUM_UNISON_VOICES];
  int reverbFdbk[NUM_UNISON_VOICES];
  int panspread;
  bool update;
};
typedef struct Program_s synthProgram_t;

struct sequencerState {
  // sequencer variables are data that needs to change per MIDI clock
  // also needed to generate step sequence structures, because this data
  // is used and interpreted by sequencer methods
  transport_e trans;
  sq_mode_e mode;
  int voices;
  int root;

  int chordStep;
  int numChordNotes;

  // time division
  int divisor;    // ticks per step
  int currentTick;
  int currentStep;

  int8_t seqCycles;
  int8_t notesPlayed;

  // can't these go into the synthProgram struct?
  int spread;     // detune, total range 1024, middle is  512
  int panspread;
};
typedef struct sequencerState sequencerState_t;

// External flags
struct extFlags
{
  volatile bool runSequencerTick;
  volatile bool updateDisplay;
  volatile bool updateSequence;
};
typedef struct extFlags extFlags_t;

#endif // DEFINES_H
