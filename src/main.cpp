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
#define TIMER1_INTERVAL_MS            10
#define TIMER2_INTERVAL_MS            251

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
  //flags.updateDisplay = true;
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

  phlxm->run(flags);
}

void loop () 
{
  //phlxm->run(flags);
  flags.updateDisplay = false;
  if (flags.runSequencerTick) {
    //phlxm->sq.tick();
    flags.runSequencerTick = false;
  }
};

/*-------------------------------------------------------------------------*/

void checkMIDI(void) 
{
  static bool toggle3 = false;
  if(MIDI.read())
  {
    switch(MIDI.getType())
    {
      case midi::Clock:
        digitalWrite(53, toggle3);
        toggle3 = !toggle3;
        flags.runSequencerTick = true;    
        break;
      default:
        break;
    }
  }
}

  /* 
  do{
    if (Serial.available()) 
    {
      byte commandByte = Serial.read();   //read first byte
      //byte noteByte = Serial.read();      //read next byte
      //byte velocityByte = Serial.read();  //read final byte
      if (commandByte == MIDI_STATUS_CLOCK) {          //if clock message
        digitalWrite(53, toggle3);
        toggle3 = !toggle3;
        midiState.midiTicks++;
        if (midiState.midiTicks > MIDI_TICKS_PER_SEQ_TICK) {
          midiState.midiTicks = 0;
          runSequencerTick = true;
        }
        if (midiState.midiTicks > MIDI_TICKS_PER_BEAT) {
          midiState.midiTicks = 0;
          int currentMillis = millis();
          // one beat (quarter note) has passed
          int currentMillisPerQuarterNote = 
              (currentMillis - midiState.prevMillisPerQuarterNote);
          // sequencer ticks are use different time scale (slower) 
          // because of CPU performance
          if (currentMillisPerQuarterNote > 0) {
            if (currentMillisPerQuarterNote != midiState.prevMillisPerQuarterNote) {
              //midiState.changeMillisPerTick = true;
              midiState.millisPerTick = currentMillisPerQuarterNote>>2; // divided by 4 (TICKS_PER_BEAT)
            }
            midiState.prevMillisPerQuarterNote = currentMillisPerQuarterNote;
          }
        } 
      }
    }
  } while (Serial.available() > 0);//when at least three bytes available
  
}


void ExtMIDI::initMIDIState(void)
{
  midiState.midiTicks = 0;
  midiState.millisPerTick = 0;
  midiState.prevMillisPerQuarterNote = 0;
  midiState.changeMillisPerTick = false;
  midiState.updateDisplay = false;
}

void ExtMIDI::handleSync(void)
{
  midiState.millisPerTick++;
}

void ExtMIDI::handleStart(void) 
{
  midiState.millisPerTick++;
}

void ExtMIDI::handleNoteOn(unsigned int channel, unsigned int note, unsigned int velocity)
{
  midiState.millisPerTick++;
}
*/