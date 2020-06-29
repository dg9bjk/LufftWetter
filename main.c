// LufftWetterStation
#include "wetterstation.h"

int main()
{
    int n = 0;
    int i;
    int fdserial;
    struct devdaten station;
    struct livedata aktdata;
    struct kanal *channels;
    channels              = NULL;
    struct kanal *ptr;
    ptr                   = NULL;

    printf("Start\n");

    union messdatenmix testfall;
    testfall.g = 100.000;
    printf("%x %x %x %x %f\n",testfall.z[0],testfall.z[1],testfall.z[2],testfall.z[3],testfall.g);
    
    exit(0);
    station.StationAdr    = 0x7001;
    station.PCAddr        = 0xf001;
    station.channels      = NULL;
    station.EEPROMSize    = 0;
    station.AnzahlKanaele = 0;
    station.AnzahlBloecke = 0;

    fdserial=SerialPortInit();

//    getVersion(fdserial,&station,&aktdata);
//    getDeviceinfo(fdserial,&station,&aktdata);
    getChanList(fdserial,&station,&aktdata);

//    do{
//    	channels= station.channels;
//    	getSingleData(fdserial,&station,channels,0);
//    	getMultiData(fdserial,&station,channels,0);

/*
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
*/
        
//        doReset(fdserial,&station);	// Software-Reset für den Fehlerfall
//        getStatus(fdserial,&station);
//        getError(fdserial,&station);

//        sleep(TIMEOUT);
        n++;
        sleep(1);
//    }while(1);

    if(DEBUG > 1)
    {
        printf("Ausgabe:\n");
        ptr = station.channels;

        for(i=0;i<aktdata.aktcntchannels-1;i++)
        {
            printf("Speicher: %03d - %02d - %03d - %03d - %p\n",ptr->lfdnr,ptr->block,ptr->maxnummer,ptr->nummer,ptr);
                ptr = ptr->next;
        }
    }
    
    close(fdserial);
    
    // Speicher freigeben    
    while(channels != NULL)
    {
        ptr = channels->next;
        free(channels);
        channels = ptr->next;
    };

    printf("Ende\n");
    return(0);
}
