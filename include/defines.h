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
#define NUM_DEBOUNCES       3
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
#define TICKS_PER_STEP      24 
#define TICKS_PER_BEAT      24  // used for millisPerTick calculations
#define BASE_TEMPO          60
#define DEFAULT_ROOT        52  // E
#define NUM_UNISON_VOICES   4
#define NUM_CHORD_NOTES     3
#define CHORD_STEP          2
#define UNISON_PITCH_SPREAD 320
#define UNISON_PAN_SPREAD   32
#define INITIAL_PROGRAM     32
#define INITIAL_MASTER_VOL  72

// MIDI
#define MIDI_TICKS_PER_BEAT 24
#define MIDI_TICKS_PER_SEQ_TICK  MIDI_TICKS_PER_BEAT/TICKS_PER_STEP
#define MIDI_STATUS_CLOCK   0xF8

// music defines
#define SCALE_SIZE          7

// enumerated types
enum mode_e { PC, GEN, MIX, SEQ, HARM_MODE, SEQ_MODE, CHORUS, REVERB, last };
enum transport_e { STOP, PLAY, PAUSE };

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
  int tempo;
  int millisPerTick;
  bool tempoChange;
  // user interface
  bool menuChanged;
  int pointer;
  int option;
  int counter;
  int numChordNotes;
  int spread;     // unison detune using midi note bend
  int panspread;
  // "MIDI input"
  int root;     // input note
  int chordStep;
  bool allNotesOff;
  // user sequence
  bool updateSeq;
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
  int chorusFdbk[NUM_UNISON_VOICES];
  int reverbType[NUM_UNISON_VOICES];
  int reverbLevel[NUM_UNISON_VOICES];
  int reverbFdbk[NUM_UNISON_VOICES];
  int panspread;
  bool update;
};
typedef struct Program_s synthProgram_t;

struct sequencerState {
  transport_e trans;
  int tempo;  // rename to bpm
  int root;
  int chordStep;
  int numChordNotes;
  int voices;
  int spread;     // detune, total range 1024, middle is  512
  int panspread;
  int millisPerTick;
  int currentTick;
  int currentStep;
};
typedef struct sequencerState sequencerState_t;

// External MIDI state
struct extMIDIState
{
  int midiTicks;
  // milliseconds per sequencer tick (slower than MIDI tick)
  int millisPerTick;
  int prevMillisPerQuarterNote;
  bool changeMillisPerTick;

};
typedef struct extMIDIState extMIDIState_t;

#endif // DEFINES_H