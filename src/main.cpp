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

#include <Arduino.h>
#include "PHLXM.h"
#include <TimerInterrupt.h>
#include <MIDI.h>



PHLXM*  phlxm = NULL;
ExtMIDI* extmidi = NULL;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);

volatile bool runSequencerTick = false;

// external midi
void TimerHandler1()
{
  // make visible over GPIO for debug
  static bool toggle1 = false;
  //digitalWrite(53, toggle1);
  toggle1 = !toggle1;
  
  extmidi->checkMIDI();
}

// internal sequencer
void TimerHandler2()
{
  runSequencerTick = true;
  //getMillisPerTick returns zero if there's no change needed
  int millisPerTick = phlxm->contrl.getMillisPerTick();
  if (millisPerTick > 0)
    ITimer2.attachInterruptInterval(millisPerTick, TimerHandler2, 0);
}

void setup () 
{
  phlxm = new PHLXM();
  extmidi = new ExtMIDI();

  // Initialize Hardware Serial (pins 1-2) for External MIDI
  //Serial.begin(31250);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();

  // Initialize Timer1 and attach ISR. This will be used to keep track
  // if incoming MIDI messages from the exterior, including clock messages.
  ITimer1.init();
  ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS, TimerHandler1, 0);

  // Initialize Timer2 and attach ISR. 
  // Timer2 will be used to clock the internal sequencer
  // External MIDI clock then don't use internal clock
  //ITimer2.init();
  //ITimer2.attachInterruptInterval(phlxm->contrl.mode.millisPerTick, TimerHandler2, 0);
}

void loop () 
{
  phlxm->run(extmidi->midiState);
  if (runSequencerTick) {
    phlxm->sq.tick();
    runSequencerTick = false;
  }
};

/*-------------------------------------------------------------------------*/

// TODO: maybe use the full MIDI library now?
void ExtMIDI::checkMIDI() 
{
  static bool toggle3 = false;
  if(MIDI.read())
  {
    switch(MIDI.getType())
    {
      case midi::Clock:
        digitalWrite(53, toggle3);
        toggle3 = !toggle3;
        runSequencerTick = true;    
        break;
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
  */
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