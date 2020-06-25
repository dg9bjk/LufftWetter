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
    struct devdaten station;
    struct kanal *channels;
    struct kanal *ptr;
    
    station.StationAddr = 0x7001;
    station.PCAddr = 0xf001;
    station.channels = NULL;
        
    fdserial=SerialPortInit();

        request(fdserial,1,&station,channels,0,0);    // Versionsabfrage
        request(fdserial,2,&station,channels,0,0);     // Komplette Stationsinfo
//        request(fdserial,5,&station,channels,0,0);    // Reset
//        sleep(2);
//        request(fdserial,6,&station,channels,0,0);    // Statusabfrage
//        request(fdserial,7,&station,channels,0,0);    // Fehlerabfrage
    do{

//        ptr = malloc(sizeof(struct kanal));
//        request(fdserial,3,&station,ptr,100,0);    // Datenabfrage Einzel
        request(fdserial,3,&station,station.channels,900,0);    // Datenabfrage Einzel
        ptr = station.channels;
        while((ptr->next != NULL) & (900 != ptr->nummer))
           ptr = ptr->next;

        printf("Ergebnis: Kanal %d %s ",ptr->nummer,ptr->groesse);

        if(ptr->mwtyp == 0x10)
           printf("-> Current - ");
        else if(ptr->mwtyp == 0x11)
           printf("-> Min - ");
        else if(ptr->mwtyp == 0x12)
           printf("-> Max - ");
        else if(ptr->mwtyp == 0x13)
           printf("-> Avg - ");
        else if(ptr->mwtyp == 0x14)
           printf("-> Sum - ");
        else if(ptr->mwtyp == 0x15)
           printf("-> Vct - ");
        else
           printf(" ");

        if(ptr->datetyp == 0x10)
           printf("%d",ptr->lastvalue.a);
        else if(ptr->datetyp == 0x11)
           printf("%d",ptr->lastvalue.b);
        else if(ptr->datetyp == 0x12)
           printf("%d",ptr->lastvalue.c);
        else if(ptr->datetyp == 0x13)
           printf("%d",ptr->lastvalue.d);
        else if(ptr->datetyp == 0x14)
           printf("%ld",ptr->lastvalue.e);
        else if(ptr->datetyp == 0x15)
           printf("%ld",ptr->lastvalue.f);
        else if(ptr->datetyp == 0x16)
           printf("%.3f",ptr->lastvalue.g);
        else if(ptr->datetyp == 0x17)
           printf("%.3f",ptr->lastvalue.h);
        else
           printf("Kein gültiger Wert\n");

        printf(" %s",ptr->einheit);
        printf("\n");

//        free(ptr);
        
        sleep(TIMEOUT);
        n++;
        sleep(1);
    }while(1);

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
