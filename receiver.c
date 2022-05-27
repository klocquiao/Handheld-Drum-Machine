#include "receiver.h"
#define PORT 12345
#define MAX_LEN 1024
#define UPTIME_FILEPATH "/proc/uptime"
static pthread_t receiverID;

static bool isShuttingDown = false;
static int socketDescriptor;
static struct sockaddr_in localSin;
static struct sockaddr_in remoteSin;

static void* receiverRunner();

static void replyHandler(const char* command);
static void sendBeat();
static void sendVolume();
static void sendTempo();
static void sendUptime();

static void sendReply(const char* reply);
static void commandError();

void startReceiver() {
    memset(&localSin, 0, sizeof(localSin));
    localSin.sin_family = AF_INET; 
    localSin.sin_addr.s_addr = htonl(INADDR_ANY);
    localSin.sin_port = htons(PORT);
    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    bind (socketDescriptor, (struct sockaddr*) &localSin, sizeof(localSin));
    pthread_create(&receiverID, NULL, receiverRunner, NULL);
}

void stopReceiver(void) {
    isShuttingDown = true;
    pthread_cancel(receiverID);
    pthread_join(receiverID, NULL);
    close(socketDescriptor);
}

void* receiverRunner() {
    while(!isShuttingDown) {        
        char messageRx[MAX_LEN];
        unsigned int sinLen = sizeof(remoteSin);
        int bytesRx = recvfrom(socketDescriptor,
            messageRx, MAX_LEN - 1, 0,
            (struct sockaddr *) &remoteSin, &sinLen);
        messageRx[bytesRx] = '\0';

        replyHandler(messageRx);
    }

    return NULL;
}

static void replyHandler(const char* messageRx) {
    //BEAT
    if(strncmp(messageRx, "none", MAX_LEN) == 0) {
        setDrumBeat(NONE);
        sendBeat();
    }    
    else if(strncmp(messageRx, "rock", MAX_LEN) == 0) {
        setDrumBeat(ROCK);
        sendBeat();
    }   
    else if(strncmp(messageRx, "alternative", MAX_LEN) == 0) {
        setDrumBeat(ALTERNATIVE);
        sendBeat();
    }
    else if(strncmp(messageRx, "beat", MAX_LEN) == 0) {
        sendBeat();
    }
    
    //SOUND
    else if(strncmp(messageRx, "bass", MAX_LEN) == 0) {
        playBassDrum();
    }
    else if(strncmp(messageRx, "hihat", MAX_LEN) == 0) {
        playHiHat();
    }   
    else if(strncmp(messageRx, "snare", MAX_LEN) == 0) {
        playSnare();
    }

    //VOLUME
    else if(strncmp(messageRx, "vup", MAX_LEN) == 0) {
        AudioMixer_incrementVolume();
        sendVolume();
    }
    else if(strncmp(messageRx, "vdown", MAX_LEN) == 0) {
        AudioMixer_decrementVolume();
        sendVolume();
    }
    else if(strncmp(messageRx, "volume", MAX_LEN) == 0) {
        sendVolume();
    }

    //TEMPO
    else if(strncmp(messageRx, "tup", MAX_LEN) == 0) {
        incrementBPM();
        sendTempo();
    }
    else if(strncmp(messageRx, "tdown", MAX_LEN) == 0) {
        decrementBPM();
        sendTempo();
    }
    else if(strncmp(messageRx, "tempo", MAX_LEN) == 0) {
        sendTempo();
    }

    else if(strncmp(messageRx, "uptime", MAX_LEN) == 0) {
        sendUptime();
    }
    else {
        commandError();
    }
}

static void sendBeat() {
    switch(getDrumBeat()) {
        case NONE: 
            sendReply("none:beat");
            break;  
        case ROCK: 
            sendReply("rock:beat");
            break;  
        case ALTERNATIVE: 
            sendReply("alternative:beat");
            break;  
    }
} 

static void sendVolume() {
    char str[MAX_LEN];
    sprintf(str, "%d", AudioMixer_getVolume());
    sendReply(strcat(str, ":volume"));
}

static void sendTempo() {
    char str[MAX_LEN];
    sprintf(str, "%d", getBPM());
    sendReply(strcat(str, ":tempo"));
}

static void sendUptime() {
    FILE* uptimeFile = fopen(UPTIME_FILEPATH,"r");
    char temp[MAX_LEN];
    if (uptimeFile == NULL) {
        printf("File does not exist!\n");
    } 
    else {
        fscanf(uptimeFile, "%s", temp);
        fclose(uptimeFile);
    }

    char time[MAX_LEN];
    int uptime = atoi(temp);
    int hours = uptime / 3600; 
	int minutes = (uptime -(3600 * hours)) / 60;
	int seconds = (uptime - (3600 * hours) - (minutes * 60));

    snprintf(time, MAX_LEN, "%i:%i:%i", hours, minutes, seconds);
    sendReply(time);
}

static void commandError() {
    char messageTx[MAX_LEN];
    sprintf(messageTx, 
        "Unsupported command entered!"
    );

    sendReply(messageTx);
}

static void sendReply(const char* reply) {
    int sinLen = sizeof(remoteSin);
    sendto(socketDescriptor,
    reply, strlen(reply),
    0,
    (struct sockaddr *) &remoteSin, sinLen);
}
