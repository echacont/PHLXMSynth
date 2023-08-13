# PHLXM Sequencer-Synthesizer

PHLXM is a "frankenstein" built on top of the 8-bit Atmel AVR Arduino platform 
(UNO, MEGA2540). Synthesis is done by [DREAM's SAM2695] (https://www.dream.fr/pdf/Serie2000/
SAM_Datasheets/SAM2695.pdf), found on the [Fluxamasynth shield] (https://moderndevice.com/products/
fluxamasynth-shield). This is a single-chip 64-voice polyphony General MIDI ROMPLER with effects. A 
second bank seems to implement MT-32 sounds. Control is achieved by the third story in this stack: 
the [MIDI shield] (https://www.sparkfun.com/products/12898) from SparkFun Electronics. Display is 
done on a cheap 16x2 LCD module.  

![IMG_4179_PHLXM_sq_s](https://github.com/echacont/PHLXMSynth/assets/102091860/dac81a0b-ff56-47e6-88ce-a99c725563b6)

