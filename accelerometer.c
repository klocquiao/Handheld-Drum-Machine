#include "accelerometer.h"

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"

#define NUMBER_OF_REGISTERS_R 7
#define WEIGHT 0.999
#define MOTION_THRESHOLD 3500

#define POLL_RATE_MS 10000000
#define DEBOUNCE_RATE_MS 200000000

#define STATUS 0x00
#define OUT_X_MSB 0x01
#define OUT_X_LSB 0x02
#define OUT_Y_MSB 0x03
#define OUT_Y_LSB 0x04
#define OUT_Z_MSB 0x05
#define OUT_Z_LSB 0x06

#define CTRL_REG1 0x2A
#define ACCELEROMETER_ADDRESS 0x1C

static int i2cFileDesc;
static pthread_t idAccelerometer;
static pthread_t idXAxis;
static pthread_t idYAxis;
static pthread_t idZAxis;

static void* accelerometerRunner();
static void* debounceRunner(void* arg);

static int initializeAccelerometer();
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
static void readI2cRegAcc(int i2cFileDesc, unsigned char regAddr, unsigned char* buffer);
static double calculateSampleAverage(double sample, double average);

bool isStopping = false;

double xAverage;
double yAverage;
double zAverage;

bool xIsDebounce = false;
bool yIsDebounce = false;
bool zIsDebounce = false;

long long int sampleCount = 0;

void startAccelerometer(void) {
    i2cFileDesc = initializeAccelerometer() ;
    writeI2cReg(i2cFileDesc, CTRL_REG1, 1);

    pthread_create(&idAccelerometer, NULL, accelerometerRunner, NULL);
}

 void* accelerometerRunner() {
    while(!isStopping) {
        struct timespec reqDelay = {0, POLL_RATE_MS};
        unsigned char buffer[NUMBER_OF_REGISTERS_R];
        readI2cRegAcc(i2cFileDesc, STATUS, buffer);

        int16_t x = (buffer[OUT_X_MSB] << 8) | (buffer[OUT_X_LSB]);
        int16_t y = (buffer[OUT_Y_MSB] << 8) | (buffer[OUT_Y_LSB]);
        int16_t z = (buffer[OUT_Z_MSB] << 8) | (buffer[OUT_Z_LSB]);
        
        if (fabs(x - xAverage) > MOTION_THRESHOLD && !xIsDebounce) {
            playSnare();
            pthread_create(&idXAxis, NULL, debounceRunner, &xIsDebounce);
        }
        if (fabs(y - yAverage) > MOTION_THRESHOLD && !yIsDebounce) {
            playBassDrum();
            pthread_create(&idYAxis, NULL, debounceRunner, &yIsDebounce);
        }        
        if (fabs(z - zAverage) > MOTION_THRESHOLD && !zIsDebounce) {
            playHiHat();
            pthread_create(&idZAxis, NULL, debounceRunner, &zIsDebounce);
        }
        
        xAverage = calculateSampleAverage(x, xAverage);
        yAverage = calculateSampleAverage(y, yAverage);
        zAverage = calculateSampleAverage(z, zAverage);

        sampleCount++;
        nanosleep(&reqDelay, (struct timespec *) NULL);
    }

    return NULL;
}

void stopAccelerometer(void) {
    pthread_join(idAccelerometer, NULL);
}


static void* debounceRunner(void* arg) {
    bool* pIsBool = (bool*) arg;
    struct timespec reqDelay = {0, DEBOUNCE_RATE_MS};
    *pIsBool = true;
    nanosleep(&reqDelay, (struct timespec *) NULL);
    *pIsBool = false;

    return NULL;
}

static int initializeAccelerometer() {
    int i2cFileDesc = open(I2CDRV_LINUX_BUS1, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, ACCELEROMETER_ADDRESS);

    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }

    return i2cFileDesc;
}

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value) {
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

static void readI2cRegAcc(int i2cFileDesc, unsigned char regAddr, unsigned char* buffer)
{
    // To read a register, must first write the address
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr)) {
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }
    // Now read the value and return it
    res = read(i2cFileDesc, buffer, NUMBER_OF_REGISTERS_R);
    if (res != NUMBER_OF_REGISTERS_R) {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }
}

static double calculateSampleAverage(double sample, double average) {
    if (sampleCount == 0) {
        return sample;
    }
    else {
        return (WEIGHT * sample) + ((1.0 - WEIGHT) * average); 
    }
}
