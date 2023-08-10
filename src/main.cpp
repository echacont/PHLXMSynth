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
#define TIMER1_INTERVAL_MS            7
#define TIMER2_INTERVAL_MS            54

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
  checkMIDI();
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
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();

  // Initialize Timer1 and attach ISR. This will be used to keep track
  // if incoming MIDI messages from the exterior, including clock messages.
  ITimer1.init();
  ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS, TimerHandler1, 0);

  // Initialize Timer2 and attach ISR. 
  // Timer2 will be used to do display refresh
  ITimer2.init();
  ITimer2.attachInterruptInterval(TIMER2_INTERVAL_MS, TimerHandler2, 0);

  //phlxm->run(flags);
}

void loop () 
{
  phlxm->run(flags);

  // chear flags (should have been consumed inside phlxm->run())
  flags.updateDisplay = false;
  flags.runSequencerTick = false;
};

/*-------------------------------------------------------------------------*/

void checkMIDI(void) 
{
  static bool toggle3 = false;

  noInterrupts();
  if(MIDI.read())
  {
    switch(MIDI.getType())
    {
      case midi::Clock:
        // instrumentation external signal GPIO 53
        toggle3 = !toggle3;
        digitalWrite(53, toggle3);

        phlxm->sq.tick();
        flags.runSequencerTick = true;    
        break;

      case midi::Start:
        phlxm->contrl.mode.trans = PLAY;
        break;
      
      case midi::Stop:
        phlxm->contrl.mode.trans = STOP;
        break;

      case midi::Continue:
        phlxm->contrl.mode.trans = PLAY;
        break;      

      default:
        break;
    }
  }
  interrupts();
}