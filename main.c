// LufftWetterStation

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define TIMEOUT 10	// Sekunden
#define DEBUG 1

#include "serial.c"
#include "lufft.c"


void debugdisplay(char array[SerialArray],int count)
{
    int i;

    printf(" - Debug Länge %d: - ",count);
    for(i=0;i<count;i++)
        printf("%02x - ",array[i]);
    printf("\n");
}

int main()
{
    int n = 0;
    int count;
    int fdserial;
    char arrayTX[SerialArray];
    char arrayRX[SerialArray];
    struct lufftdaten station;
    
    station.StationAddr = 0x7001;
    station.PCAddr = 0xf001;
    
    fdserial=SerialPortInit();

    do{
        memset(arrayTX,0,SerialArray);
        memset(arrayRX,0,SerialArray);
        count = encode(arrayTX,1);
        if(DEBUG)
            debugdisplay(arrayTX,count);

        send(fdserial,arrayTX,count);
        sleep(1);
        count = recv(fdserial,arrayRX);
        if(DEBUG)
            debugdisplay(arrayRX,count);

        decode(arrayRX,count,&station);
    
        sleep(TIMEOUT);
        n++;
    }while(1);
    
    close(fdserial);
    return(0);
}
