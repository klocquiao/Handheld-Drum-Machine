#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "drumBeat.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h> 
#include <fcntl.h> 

void startAccelerometer(void);
void stopAccelerometer(void);

#endif