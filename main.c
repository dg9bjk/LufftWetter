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
    int i;
    int fdserial;
    struct lufftdaten station;
    struct Kanal *channels;
    struct Kanal *ptr;
    
    station.StationAddr = 0x7001;
    station.PCAddr = 0xf001;
    station.channels = NULL;
        
    fdserial=SerialPortInit();

//    do{
        request(fdserial,1,&station,channels,0,0);    
        request(fdserial,2,&station,channels,0,0);    
//        sleep(TIMEOUT);
//        n++;
//    }while(1);

    printf("Ausgabe:\n");
    ptr = station.channels;
    
    if(DEBUG)
        for(i=0;i<station.aktcntchannels-1;i++)
        {
            printf("Speicher: %03d - %02d - %03d - %03d - %p\n",ptr->lfdnr,ptr->block,ptr->maxnummer,ptr->nummer,ptr);
                ptr = ptr->next;
        }
    
    close(fdserial);
    
    // Speicher freigeben    
    while(channels != NULL)
    {
        ptr = channels->next;
        free(channels);
        channels = ptr->next;
    };

    return(0);
}
