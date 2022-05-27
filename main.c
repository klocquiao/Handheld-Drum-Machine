#include "audioMixer.h"
#include "drumBeat.h"
#include "joystick.h"
#include "receiver.h"
#include "accelerometer.h"

void startBeatBox() {
    AudioMixer_init();
    startDrumBeat();
    startJoystick();
    startAccelerometer();
    startReceiver();

    while (1) {};

    stopReceiver();
    stopAccelerometer();
    stopJoystick();
    stopDrumBeat();
    AudioMixer_cleanup();
}


int main() {
    startBeatBox();
}
