// PHLXM sequencer->synth main.cpp
// Copyright 2023 Eleonora Chacón Taylor
// This program is distributed under the terms of the GNU General Public License v3

// TODO: move timer interrupt code to the PHLXM/Sequencer class maybe

#include <Arduino.h>
#include <MsTimer2.h>
#include "PHLXM.h"

class ExtMIDI {
  public:
  extMIDIState_t midiState;
  void initMIDIState(void);
  ExtMIDI(HardwareSerial &s) { initMIDIState(); }
  void checkMIDI(void);
  //ExtMIDI(void);
};

ExtMIDI* midi = NULL;

PHLXM* phlxm = NULL;

void tick ()
{
  phlxm->sq.tick();
  int millisPerTick = phlxm->contrl.getMillisPerTick();
  if (millisPerTick) {
    MsTimer2::stop();
    MsTimer2::set(millisPerTick, tick);
    MsTimer2::start();
  }
}

void setup () 
{
  phlxm = new PHLXM();
  MsTimer2::set(phlxm->contrl.mode.millisPerTick, tick);
  MsTimer2::start();
  Serial.begin(31250);
  midi = new ExtMIDI(Serial);
}

void loop () 
{
  phlxm->run(midi->midiState);
  midi->checkMIDI();
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