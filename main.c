// LufftWetterStation

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "serial.c"
#include "lufft.c"

#define TIMEOUT 10	// Sekunden

int main()
{
    int n = 0;
    int i;
    int count;
    int fdserial;
    char arrayTX[SerialArray];
    char arrayRX[SerialArray];
    
    fdserial=SerialPortInit();

//    do{
        memset(arrayTX,0,SerialArray);
        memset(arrayRX,0,SerialArray);
        count = encode(arrayTX);

        printf(" - DebugTX: - ");
        for(i=0;i<30;i++)
            printf("%x - ",arrayTX[i]);
        printf("\n");

        send(fdserial,arrayTX,count);
        usleep(150000);
        count = recv(fdserial,arrayRX);
//        decode(count);
        printf(" - Run: %d Laenge: %d DebugRX: - ",n,count);
        for(i=0;i<30;i++)
            printf("%x - ",arrayRX[i]);
        printf("\n");
    
//        sleep(TIMEOUT);
        n++;
//    }while(1);
    
    close(fdserial);
    return(0);
}
