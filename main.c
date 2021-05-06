// LufftWetterStation
#include "wetterstation.h"

int main(int cmdcnt,char *cmdparameter[],char *env[])
{
	int n = 0;						// Rundenzähler
	int errorcnt = 0;				// Fehlerzähler
    int i;							// Temporäre Schleifenzähler
    int abbruch = 0;				// Abbruchbedingung für Dauerschleife 0 = Dauerlauf, sonst Ende
    int chanlist1[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int chanlist2[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    struct master globalkonfig;		// Lokale Globale PC-Daten
    struct kanal *channels;			// Kanalliste für temporäre Abfrage
    channels              = NULL;
    struct kanal *ptr;				// Kanalliste für temporäre Abfrage (Schleifen)
    ptr                   = NULL;

    printf("\nStart\n");

	globalkonfig.PCAdr		= 0xF001;				// lokale default PC-Adresse
    globalkonfig.konfigpath = NULL;
    globalkonfig.serial     = NULL;
    globalkonfig.station	= NULL;

    if(DEBUG > 1)
    {
    	printf("Cmd          : %d \n",cmdcnt);
    	for(i=0;cmdparameter[i] != 0;i++)
    		printf("cmdparameter : %s \n",cmdparameter[i]);
    	for(i=0;env[i] != 0;i++)
    		printf("env          : %s \n",env[i]);
    }

    if(DEBUG > 1)
    	printf("Suche / Lade Konfiguration !\n");
    searchkonfig(&globalkonfig);							// Suchen und Laden der Konfigdatei

    if(DEBUG > 1)
    	printf("Suche / Lade Schnittstelle !\n");
    searchport(&globalkonfig);								// Suchen und Laden der serielen Schnittstellen

    // Testausgabe für Simulator zum erstellen der Datenfelder für Floatzahlen
    //union messdatenmix testfall;
    //testfall.g = 23.000;
    //printf("%x %x %x %x %f\n",testfall.z[0],testfall.z[1],testfall.z[2],testfall.z[3],testfall.g);

    if(globalkonfig.konfigpath == NULL)
    {
    	printf("Keine Konfiguration gefunden\n");
    	getStationList(&globalkonfig,*globalkonfig.serial,WSxTyp,STARTSCANSTATION,STOPSCANSTATION);	// Suchen nach den Stationsaddressen (Beispiel: Nur Typ 7 - von 1 bis 3)
    	getDeviceinfo(globalkonfig.station);	// Laden der Stationsdaten (Globale Stationsdaten)
    	getChanList(globalkonfig.station);		// Laden der Kanaldaten (Kanallisten)

    	chanlist1[0] =100;
    	chanlist1[1] =160;
    	chanlist1[2] =110;
    	chanlist1[3] =170;
    	chanlist1[4] =111;
    	chanlist1[5] =112;
    	chanlist1[6] =114;
    	chanlist1[7] =200;
    	chanlist1[8] =260;
    	chanlist1[9] =205;
    	chanlist1[10]=265;
    	chanlist1[11]=210;
    	chanlist1[12]=270;
    	chanlist1[13]=215;
    	chanlist1[14]=300;
    	chanlist1[15]=360;
    	chanlist1[16]=305;
    	chanlist1[17]=365;
    	chanlist1[18]=310;
    	chanlist1[19]=10000;

    	chanlist2[0] =400;
    	chanlist2[1] =460;
    	chanlist2[2] =480;
    	chanlist2[3] =401;
    	chanlist2[4] =403;
    	chanlist2[5] =500;
    	chanlist2[6] =580;
    	chanlist2[7] =501;
    	chanlist2[8] =503;
    	chanlist2[9] =805;
    	chanlist2[10]=806;
    	chanlist2[11]=510;
    	chanlist2[12]=600;
    	chanlist2[13]=605;
    	chanlist2[14]=700;
    	chanlist2[15]=800;
    	chanlist2[16]=617;
    	chanlist2[17]=677;
    	chanlist2[18]=900;
    	chanlist2[19]=960;
    }

// Permanente Schleife zum Laden und Speichern der Wetterdaten
    do{
    	channels= malloc(sizeof(struct kanal));
    	channels->next = NULL;
    	channels->nummer = -1;
//    	getSingleData(globalkonfig.station,channels,900);	// Lade einen Datenpunkt
//    	getSingleData(globalkonfig.station,channels,100);	// Lade einen Datenpunkt
//    	getSingleData(globalkonfig.station,channels,10000);	// Lade einen Datenpunkt

    	getMultiData(globalkonfig.station,channels,chanlist1);	// Lade mehrere Datenpunkte (max. 20)
    	getMultiData(globalkonfig.station,channels,chanlist2);	// Lade mehrere Datenpunkte (max. 20)

    	ptr = channels;
    	while(ptr != NULL)
    	{
    		printf("Ergebnis: Kanal %d %s ",ptr->nummer,ptr->groesse);
			printMWtyp(ptr->mwtyp);
			printValueByDatatyp(ptr->datetyp,ptr->value);
			printf(" %s",ptr->einheit);
			printf("\n");
			ptr = ptr->next;
    	}
        free (channels);
    	channels = NULL;
        
//        getStatus(globalkonfig.station);	// Letzter Status der Kommunikation
//        getError(globalkonfig.station);		// Letzter Fehler der Kommunikation

    	if(errorcnt > 10)
        	doReset(globalkonfig.station);	// Software-Reset für den Fehlerfall

        sleep(TIMEOUT);	// Minimale Wartezeit bis zum nächsten Durchlauf
        n++;			// Rundenzähler
        printf("DEBUG: RUNDE %d\n",n);
    }while(! abbruch);	// Wenn nicht 0 ,dann Ende

    if(DEBUG > 1)
    {
    	if(globalkonfig.station->channels != NULL)
    	{
    		ptr = globalkonfig.station->channels;
    		printf("Ausgabe:\n");

    		for(i=0;i < globalkonfig.station->live.aktcntchannels -1;i++)
    		{
    			printf("Speicher: %03d - %02d - %03d - %03d - %p\n",ptr->lfdnr,ptr->block,ptr->maxnummer,ptr->nummer,ptr);
                ptr = ptr->next;
    		}
    	}
    }
    
    while(globalkonfig.serial != NULL)
    {
    	close(globalkonfig.serial->fdserial);		// Schliessen des File-Zeigers der serialen Schnittstelle
    	globalkonfig.serial->fdserial = -1;			// Variable als Ungültig markieren
    	//globalkonfig.serial->serialport = NULL;
    	globalkonfig.serial = globalkonfig.serial->next;
    }

    // Speicher freigeben    
    while(channels != NULL)
    {
        ptr = channels->next;	// Speicher nächsten Zeiger zwischen
        free(channels);			// Lösche aktuellen Zeiger
        channels = NULL;		// Zeiger als Ungültig markieren
        channels = ptr->next;	// alter Zeiger aufgeben und nächsten (zwischengespeichterten) Zeiger als aktuellen Zeiger nehmen
    };

    printf("Ende\n");
    return(abbruch);			// Abbruchbedinung ist Rückgabewert an Betriebs-System
}
