// LufftWetterStation

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TIMEOUT 10	// Sekunden
#define DEBUG 1

#include "lufft.c"

int main()
{
    int n = 0;
    int fdserial;
    struct lufftdaten station;
    struct Kanal startchannel;
    
    station.StationAddr = 0x7001;
    station.PCAddr = 0xf001;
    station.channel = &startchannel;
    
    fdserial=SerialPortInit();

//    do{
        request(fdserial,1,&station,0,0);    
        request(fdserial,2,&station,0,0);    
//        sleep(TIMEOUT);
//        n++;
//    }while(1);
    
    close(fdserial);
    free(station.channel);
    return(0);
}
