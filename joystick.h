#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "audioMixer.h"
#include "drumBeat.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>
#include <unistd.h>
#include <sys/epoll.h> 
#include <fcntl.h> 

typedef enum JoystickDirection_e JoystickDirection;
enum JoystickDirection_e {
    UP = 0,
    RIGHT,
    DOWN,
    LEFT,
    CENTER
};

typedef struct Joystick_s Joystick;
struct Joystick_s {
    const char* directionPath;
    const char* valuePath;
    const char* edgePath;
    const char* pinNumber;
};

void startJoystick(void);
void stopJoystick(void);

#endif