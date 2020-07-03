// LufftWetterStation
#include "wetterstation.h"

int main(int cmdcnt,char *cmdparameter[],char *env[])
{
	int n = 0;						// Rundenzähler
	int errorcnt = 0;				// Fehlerzähler
    int i;							// Temporäre Schleifenzähler
    int abbruch = 0;				// Abbruchbedingung für Dauerschleife 0 = Dauerlauf, sonst Ende
    struct master globalkonfig;			// Lokale Globale PC-Daten
    struct kanal *channels;			// Kanalliste für temporäre Abfrage
    channels              = NULL;
    struct kanal *ptr;				// Kanalliste für temporäre Abfrage (Schleifen)
    ptr                   = NULL;

    // Vorschläge der Konfigurationspfade - werden der Reihe nach durchprobiert. Der erste Treffer wird benutzt.
    char konfigpath[9][30] = {
    		{"./wetter.conf"},
			{"~/wetter.conf"},
			{"~/lufft/wetter.conf"},
			{"/etc/wetter.conf"},
			{"/etc/lufft/wetter.conf"},
			{"/usr/lufft/wetter.conf"},
			{"/var/lufft/wetter.conf"},
			{"/usr/share/lufft/wetter.conf"},
			{"/var/share/lufft/wetter.conf"}
			};

    globalkonfig.konfigpath    = NULL;						// Initialisierung auf NULL
    for(i=0;i < 9;i++)										// Suche das konfigpath-Array durch
    	if(readKonfig(konfigpath[i],&globalkonfig) > 0)		// Wenn eine gültige Dateigefunden wird
    	{
    		globalkonfig.konfigpath    = (char*)&konfigpath[i];	// Wird diese gespeichert
    		break;												// Der Suchlauf sofort bendet.
    	}

    if(DEBUG > 1)
    {
    	printf("Cmd          : %d \n",cmdcnt);
    	for(i=0;cmdparameter[i] != 0;i++)
    		printf("cmdparameter : %s \n",cmdparameter[i]);
    	for(i=0;env[i] != 0;i++)
    		printf("env          : %s \n",env[i]);
    }

    // Vorschläge der Serialen Schnittstellen
    char serialport[255];	// Freies Feld für freie Konfiguration
    memset(serialport,0x0,255);
    strncpy(serialport,"/dev/ttyUSB0",12);

    printf("\nStart\n");

    // Testausgabe für Simulator zum erstellen der Datenfelder für Floatzahlen
    //union messdatenmix testfall;
    //testfall.g = 23.000;
    //printf("%x %x %x %x %f\n",testfall.z[0],testfall.z[1],testfall.z[2],testfall.z[3],testfall.g);

    globalkonfig.PCAdr		  = 0xF001;				// lokale PC-Adresse
    globalkonfig.konfigpath    = konfigpath [0];	// Link auf den Konfig-File-Pfad

    globalkonfig.station = malloc(sizeof(struct devdaten));
    globalkonfig.station->serialport  	= (char*)&serialport; // link auf die default Seriale Schnittstelle
    globalkonfig.station->StationAdr    = 0x7001;			// erste Stationsadresse (Initialisierung)
    globalkonfig.station->channels      = NULL;				// Leere Liste der Kanäle (Initialisierung)
    globalkonfig.station->EEPROMSize    = -1;				// Unbekanntes EEPROM (Initialisierung)
    globalkonfig.station->AnzahlKanaele = -1;				// Anzahl der Kanäle unbekannt (Initialisierung)
    globalkonfig.station->AnzahlBloecke = -1;				// Anzahl der Blöcke (Kanallisten % 100) - (Initialisierung)

    globalkonfig.station->fdserial = SerialPortInit(globalkonfig.station->serialport);	// Öffnen der serialen Schnittstelle
    												// Der File-Zeiger fdserial wird für die gesammte Kommunikation benötigt.
    getStationList(&globalkonfig,7,1,3);	// Suchen nach den Stationsaddressen (Beispiel: Nur Typ 7 - von 1 bis 3)

    getDeviceinfo(globalkonfig.station);	// Laden der Stationsdaten (Globale Stationsdaten)
    getChanList(globalkonfig.station);	// Laden der Kanaldaten (Kanallisten)

// Permanente Schleife zum Laden und Speichern der Wetterdaten
    do{
    	channels= malloc(sizeof(struct kanal));
    	channels->next = NULL;
    	channels->nummer = -1;
    	getSingleData(globalkonfig.station,channels,900);	// Lade einen Datenpunkt

    	int chanlist[20]={100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195};
    	getMultiData(globalkonfig.station,channels,chanlist);	// Lade mehrere Datenpunkte (max. 20)

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
        
        getStatus(globalkonfig.station);	// Letzter Status der Kommunikation
        getError(globalkonfig.station);		// Letzter Fehler der Kommunikation

    	if(errorcnt > 10)
        	doReset(globalkonfig.station);	// Software-Reset für den Fehlerfall

        sleep(TIMEOUT);	// Minimale Wartezeit bis zum nächsten Durchlauf
        n++;			// Rundenzähler
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
    
    close(globalkonfig.station->fdserial);		// Schliessen des File-Zeigers der serialen Schnittstelle
    globalkonfig.station->fdserial = -1;			// Variable als Ungültig markieren
    
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
