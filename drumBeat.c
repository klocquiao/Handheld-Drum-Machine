#include "drumBeat.h"

#define NANOSECOND_TO_SECOND 1000000000
#define DEFAULT_BPM 90
#define BPM_MAX 200
#define BPM_MIN 60
#define BPM_INCREMENT_DECREMENT_VALUE 5

#define BASS_DRUM "wave-files/100051__menegass__gui-drum-bd-hard.wav"
#define HI_HAT "wave-files/100053__menegass__gui-drum-cc.wav"
#define SNARE "wave-files/100059__menegass__gui-drum-snare-soft.wav"

static wavedata_t bassDrumWav;
static wavedata_t hiHatWav;
static wavedata_t snareWav;

static pthread_t idDrumBeat;
static BeatType currentBeat;
static int currentBPM;
static bool isRunning = true;

static long long int calculateHalfBeat(void);
static void* drumBeatRunner();
static void playDefaultRock(void);
static void playDefaultAlternative(void);

void startDrumBeat() {
    currentBeat = ROCK;
    currentBPM = DEFAULT_BPM;
	AudioMixer_readWaveFileIntoMemory(BASS_DRUM, &bassDrumWav);
    AudioMixer_readWaveFileIntoMemory(HI_HAT, &hiHatWav);
	AudioMixer_readWaveFileIntoMemory(SNARE, &snareWav);

    pthread_create(&idDrumBeat, NULL, drumBeatRunner, NULL);
}

void stopDrumBeat() {
    isRunning = false;
    AudioMixer_freeWaveFileData(&bassDrumWav);
    AudioMixer_freeWaveFileData(&hiHatWav);
    AudioMixer_freeWaveFileData(&snareWav);
    pthread_join(idDrumBeat, NULL);
}

void* drumBeatRunner() {
    while(isRunning) {
        switch(currentBeat) {
            case ROCK:
                playDefaultRock();
                break;
            case ALTERNATIVE:
                playDefaultAlternative();
                break;
            default:
                break;
        }
    }
    return NULL;
}

BeatType getDrumBeat() {
    return currentBeat;
}

void setDrumBeat(BeatType newBeat) {
    currentBeat = newBeat;
}

int getBPM() {
    return currentBPM;
}

void incrementBPM() {
    int newBPM = currentBPM + BPM_INCREMENT_DECREMENT_VALUE;
    if (newBPM > BPM_MAX) {
        printf("ERROR: Volume must be between %i and %i.\n", BPM_MIN, BPM_MAX);
    }
    else {
        currentBPM = newBPM;
    }
}

void decrementBPM() {
    int newBPM = currentBPM - BPM_INCREMENT_DECREMENT_VALUE;
    if (newBPM < BPM_MIN) {
        printf("ERROR: Volume must be between %i and %i.\n", BPM_MIN, BPM_MAX);
    }
    else {
        currentBPM = newBPM;
    }
}

void playBassDrum() {
    AudioMixer_queueSound(&bassDrumWav);
}

void playHiHat() {
    AudioMixer_queueSound(&hiHatWav);
}

void playSnare() {
    AudioMixer_queueSound(&snareWav);
}

static void playDefaultRock() {
    struct timespec reqDelay = {0, calculateHalfBeat()};
    //1.0
    AudioMixer_queueSound(&bassDrumWav);
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //1.5
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //2.0
    AudioMixer_queueSound(&hiHatWav);
    AudioMixer_queueSound(&snareWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //2.5
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //3.0
    AudioMixer_queueSound(&bassDrumWav);
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //3.5
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //4.0
    AudioMixer_queueSound(&hiHatWav);
    AudioMixer_queueSound(&snareWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //4.5
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

static void playDefaultAlternative() {
    struct timespec reqDelay = {0, calculateHalfBeat()};
    //1.0
    AudioMixer_queueSound(&bassDrumWav);
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //1.5
    AudioMixer_queueSound(&bassDrumWav);
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //2.0
    AudioMixer_queueSound(&snareWav);
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //2.5
    AudioMixer_queueSound(&bassDrumWav);
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //3.0
    AudioMixer_queueSound(&bassDrumWav);
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //3.5
    AudioMixer_queueSound(&bassDrumWav);
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //4.0
    AudioMixer_queueSound(&snareWav);
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);

    //4.5
    AudioMixer_queueSound(&bassDrumWav);
    AudioMixer_queueSound(&hiHatWav);
    nanosleep(&reqDelay, (struct timespec *) NULL);
}


static long long int calculateHalfBeat() {
    return (60.0 / currentBPM / 2) * 1000000000;
}