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
#define TIMER2_INTERVAL_MS            200


#include <Arduino.h>
#include "PHLXM.h"
#include <TimerInterrupt.h>

ExtMIDI* midi = NULL;

PHLXM* phlxm = NULL;

void tick ()
{
  phlxm->sq.tick();
  int millisPerTick = phlxm->contrl.getMillisPerTick();
  if (millisPerTick) {
    //MsTimer2::stop();
    //MsTimer2::set(millisPerTick, tick);
    //MsTimer2::start();
  }
}

// external midi
void TimerHandler1()
{
  static bool toggle1 = false;

  //timer interrupt toggles  GRN_LED
  digitalWrite(6, toggle1);
  digitalWrite(53, toggle1);
  toggle1 = !toggle1;
}

// internal sequencer
void TimerHandler2()
{
  phlxm->sq.tick();
  // getMillisPerTick returns zero if there's no change needed
  int millisPerTick = phlxm->contrl.getMillisPerTick();
  if (millisPerTick > 0)
    ITimer2.attachInterruptInterval(millisPerTick, TimerHandler2, 0);

  // make visible over GPIO for debug
  //static bool toggle2 = false;
  //digitalWrite(51, toggle2);
  //toggle2 = !toggle2;
}

void setup () 
{
  phlxm = new PHLXM();
  //MsTimer2::set(phlxm->contrl.mode.millisPerTick, tick);
  //MsTimer2::start();

  // Initialize Hardware Serial (pins 1-2) for External MIDI
  Serial.begin(31250);
  midi = new ExtMIDI(Serial);

  // Initialize Timer1 and attach ISR. This will be used to keep track
  // if incoming MIDI messages from the exterior, including clock messages.
  ITimer1.init();
  ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS, TimerHandler1, 0);
  // Initialize Timer2 and attach ISR. 
  // Timer2 will be used to clock the internal sequencer
  ITimer2.init();
  ITimer2.attachInterruptInterval(phlxm->contrl.mode.millisPerTick, TimerHandler2, 0);
}

void loop () 
{
  phlxm->run(midi->midiState);
  //midi->checkMIDI();
};

/*-------------------------------------------------------------------------*/

void ExtMIDI::checkMIDI() 
{
  do{
    if (Serial.available()) 
    {
      byte commandByte = Serial.read();   //read first byte
      byte noteByte = Serial.read();      //read next byte
      byte velocityByte = Serial.read();  //read final byte
      if (commandByte == MIDI_STATUS_CLOCK) {          //if clock message
        midiState.midiTicks++;
        if (midiState.midiTicks > MIDI_TICKS_PER_BEAT) {
          midiState.midiTicks = 0;
          int currentMillis = millis();
          // one beat (quarter note) has passed
          int currentMillisPerQuarterNote = 
              (currentMillis - midiState.prevMillisPerQuarterNote);
          // sequencer ticks are use different time scale (slower) 
          // because of CPU performance
          if ((currentMillisPerQuarterNote != midiState.prevMillisPerQuarterNote) &&
              (currentMillisPerQuarterNote > 0)) {
            midiState.changeMillisPerTick = true;
            midiState.millisPerTick = currentMillisPerQuarterNote>>2; // divided by 4 (TICKS_PER_BEAT)
            midiState.prevMillisPerQuarterNote = currentMillisPerQuarterNote;
          }
        }
      }
    }
  }
  while (Serial.available() > 0);//when at least three bytes available
}

void ExtMIDI::initMIDIState(void)
{
  midiState.midiTicks = 0;
  midiState.millisPerTick = 0;
  midiState.prevMillisPerQuarterNote = 0;
  midiState.changeMillisPerTick = false;
}
/*
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