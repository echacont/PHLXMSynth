// PHLXM sequencer->synth main.cpp
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

// TODO: move timer interrupt code to the PHLXM/Sequencer class maybe

// Important:
// Some of these defines need to exist before <TimerInterrupt.h> is loaded
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0
#define USE_TIMER_1                   true
#define USE_TIMER_2                   true
#define USE_TIMER_3                   false
#define USE_TIMER_4                   false
#define USE_TIMER_5                   false
#define TIMER1_INTERVAL_MS            1       // check for MIDI messages each ms. 
#define TIMER2_INTERVAL_MS            101

#include <Arduino.h>
#include "PHLXM.h"
#include <TimerInterrupt.h>
#include <MIDI.h>

PHLXM*  phlxm = NULL;
extFlags_t flags;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);

void checkMIDI(void);

// external midi
void TimerHandler1()
{
  static bool toggle3 = false;
  noInterrupts();
  // instrumentation external signal GPIO 53
  toggle3 = !toggle3; digitalWrite(53, toggle3);
  checkMIDI();
  toggle3 = !toggle3; digitalWrite(53, toggle3);
  interrupts();
}

// display refresh
void TimerHandler2()
{
  flags.updateDisplay = true;
}

void setup () 
{
  phlxm = new PHLXM();
  flags.updateDisplay = false;
  flags.runSequencerTick = false;

  // Initialize External MIDI
  //Serial.begin(31250);
  MIDI.begin(MIDI_CHANNEL);
  MIDI.turnThruOff();

  // Initialize Timer1 and attach ISR. This will be used to keep track
  // if incoming MIDI messages from the exterior, including clock messages.
  ITimer1.init();
  ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS, TimerHandler1, 0);

  // Initialize Timer2 and attach ISR. 
  // Timer2 will be used to do display refresh
  ITimer2.init();
  ITimer2.attachInterruptInterval(TIMER2_INTERVAL_MS, TimerHandler2, 0);

  pinMode(PIN_LED_GRN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  digitalWrite(PIN_LED_GRN, HIGH);
  digitalWrite(PIN_LED_RED, HIGH);
  phlxm->sq.progChange(phlxm->contrl.program);
}

void loop () 
{
  phlxm->run(flags);
};

/*-------------------------------------------------------------------------*/

void checkMIDI(void)
{
  
  if(MIDI.read())
  {
    switch(MIDI.getType())
    {
      case midi::Clock:
        phlxm->sq.tick();
        break;

      case midi::Start:
        phlxm->contrl.mode.trans = PLAY;
        break;
      
      case midi::Stop:
        phlxm->contrl.mode.trans = STOP;
        phlxm->sq.playAllNotesOff();
        break;

      case midi::Continue:
        phlxm->contrl.mode.trans = PLAY;
        break;      

      case midi::NoteOn:
        int incomingPitch = MIDI.getData1();  
        if (incomingPitch != phlxm->contrl.mode.root) {
          flags.updateSequence = true;
          phlxm->contrl.mode.root = incomingPitch;
        }
        //MIDI.getData2();  
        break;
      
      case midi::NoteOff:
        break;

      default:
        break;
    }
  }
  
}