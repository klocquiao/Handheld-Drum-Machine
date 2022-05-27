#include "joystick.h"

#define NUM_DIRECTIONS 5
#define MAX_READ_LENGTH 2
#define EXPORT_DELAY_MS 330000000

#define DIRECTION "/direction"
#define VALUE "/value"
#define EDGE "/edge"

#define JOYSTICK_UP_PATH "/sys/class/gpio/gpio26" 
#define JOYSTICK_DOWN_PATH "/sys/class/gpio/gpio46"
#define JOYSTICK_LEFT_PATH "/sys/class/gpio/gpio65"
#define JOYSTICK_RIGHT_PATH "/sys/class/gpio/gpio47"
#define JOYSTICK_CENTER_PATH "/sys/class/gpio/gpio27"
#define GPIO_EXPORT "/sys/class/gpio/export"

static const Joystick JoystickConfig[] = {
	{JOYSTICK_UP_PATH DIRECTION, JOYSTICK_UP_PATH VALUE, JOYSTICK_UP_PATH EDGE, "26"},
	{JOYSTICK_RIGHT_PATH DIRECTION, JOYSTICK_RIGHT_PATH VALUE, JOYSTICK_RIGHT_PATH EDGE, "46"},
	{JOYSTICK_DOWN_PATH DIRECTION, JOYSTICK_DOWN_PATH VALUE, JOYSTICK_DOWN_PATH EDGE, "65"},
	{JOYSTICK_LEFT_PATH DIRECTION, JOYSTICK_LEFT_PATH VALUE, JOYSTICK_LEFT_PATH EDGE, "47"},
    {JOYSTICK_CENTER_PATH DIRECTION, JOYSTICK_CENTER_PATH VALUE, JOYSTICK_CENTER_PATH EDGE, "27"}
};

static pthread_t idJoystick;
static bool isStopping = false;
static void* joystickRunner();
static void initializeJoystick();
static void enableJoystickExport(void);

static JoystickDirection getJoystickResponse();
static int checkJoystickDirection();
static void writeFile(const char* filepath, const char* option);

void startJoystick() {
	enableJoystickExport();
    initializeJoystick();
    pthread_create(&idJoystick, NULL, joystickRunner, NULL);
}

void stopJoystick() {
    pthread_join(idJoystick, NULL);
}

static void enableJoystickExport() {
	struct timespec reqDelay = {0, EXPORT_DELAY_MS};

	for (int i = 0; i < NUM_DIRECTIONS; i++) {
		writeFile(GPIO_EXPORT, JoystickConfig[i].pinNumber);
	}

	nanosleep(&reqDelay, (struct timespec *) NULL);
}

static void* joystickRunner() {
    while(!isStopping) {
        JoystickDirection dir = getJoystickResponse();
        switch(dir) {
            case UP:
                AudioMixer_incrementVolume();
                break;
            case DOWN:
                AudioMixer_decrementVolume();
                break;
            case LEFT:
                decrementBPM();
                break;
            case RIGHT:
                incrementBPM();
                break;
            case CENTER:
                setDrumBeat((getDrumBeat() + 1) % NUMBER_OF_BEATS);
                break;
        }
    }

	return NULL;
}

static void initializeJoystick() {
	for (int i = 0; i < NUM_DIRECTIONS; i++) {
		writeFile(GPIO_EXPORT, JoystickConfig[i].pinNumber);
		writeFile(JoystickConfig[i].directionPath, "in");
		writeFile(JoystickConfig[i].edgePath, "both");
	}
}

static JoystickDirection getJoystickResponse() {
	int epollfd = epoll_create(1);
	
	struct epoll_event epollStruct;
	epollStruct.events = EPOLLIN | EPOLLET | EPOLLPRI;

	for (int i = 0; i < NUM_DIRECTIONS; i++) {
		int fd = open(JoystickConfig[i].valuePath, O_RDONLY | O_NONBLOCK);
		epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &epollStruct);
	}

	for (int i = 0; i < 5; i++) {
		epoll_wait(epollfd, &epollStruct, 1, -1);  
	}

	JoystickDirection joystickDirection = checkJoystickDirection(); 
	epoll_wait(epollfd, &epollStruct, 1, -1);  
	close(epollfd);
	
	return joystickDirection; 
}

static int checkJoystickDirection() {
	for (int i = 0; i < NUM_DIRECTIONS; i++) {
		FILE* fd = fopen(JoystickConfig[i].valuePath, "r");
		char buff[MAX_READ_LENGTH];	
		fgets(buff, MAX_READ_LENGTH, fd);	
		
		if (strcmp(buff, "0") == 0) {
			return i;
		}
		fclose(fd);
	}
	return -1;
}

static void writeFile(const char* filepath, const char* option) {
	FILE* fd = fopen(filepath, "w");
	fprintf(fd, "%s", option);
	fclose(fd);
}