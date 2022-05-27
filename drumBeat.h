#ifndef DRUM_BEAT_H
#define DRUM_BEAT_H

#include "audioMixer.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#define NUMBER_OF_BEATS 3
typedef enum BeatType_e BeatType;
enum BeatType_e{
    NONE,
    ROCK,
    ALTERNATIVE
};

void startDrumBeat(void);
void stopDrumBeat(void);

BeatType getDrumBeat();
void setDrumBeat(BeatType newBeat);

int getBPM();
void incrementBPM();
void decrementBPM();

void playBassDrum();
void playHiHat();
void playSnare();

#endif