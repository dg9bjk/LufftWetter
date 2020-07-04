#include "wetterstation.h"

// Abfrage-API für LUFFT Wetterstation.

// Die "Gefährlichen Funktionen" wurden nicht aufgenommen, weil diese die Station zerstören können.
// Konfiguration mit der Originalsoftware "ConfigTool.NET" ist pflicht, um die Gewährleistung nicht
// zu verlieren.

// Scan des Netzwerkes nach Wetterstation
int getStationList(struct master *globalkonfig,struct serport serial,int typ,int start,int stop)
{
	int count = 0;										// Zähler der gefundenen Stationen
	int i;												// Temporäre Laufvariablen
	int addr;											// Temporäre Addresse
	struct devdaten *ptr;								// Neuer Zeiger auf zu suchende Station
	struct devdaten *testptr;							// Testzeiger auf bestehende Stationsliste

	if(start < 1)										// Begrenzung der Suchvariablen nach Lufft-Konvention
			start = 1;
	if(stop > 255)
		stop = 255;
	if (typ < 1)										// Begrenzung Typ nach Lufft-Konvention
		typ = 1;
	if (typ > 10)
		typ = 10;

	ptr = malloc(sizeof(struct devdaten));				// Aufverdacht eine Struktur zum Suchen - wird am Ende wieder gelöscht
	ptr->next = NULL;

	count = 1;											// Zähler für die Suche
	for(i = start;i <= stop;i++)									// Suche der Reihe nach alle Addressen durch
	{
	    addr = (typ & 0x000F) << 12;								// Adresse codieren -> Lufft-Konvention
	    addr += (i & 0x00FF);
	    ptr->StationAdr    = addr;									// Adressen in die temporäre Struktur schreiben
	    ptr->PCAdr         = globalkonfig->PCAdr;
	    ptr->aktHardware   = 0;										// Null-Initialisierung
	    ptr->aktSoftware   = 0;
		ptr->AnzahlBloecke = 0;
		ptr->AnzahlKanaele = 0;
		ptr->EEPROMSize    = 0;
		ptr->fdserial	   = serial.fdserial;
		ptr->serialport    = serial.serialport;
		memset(ptr->Geraetebeschreibung,0x0,Befehllaenge);
		memset(ptr->Geraetebezeichnung,0x0,Befehllaenge);

		getVersion(ptr);											// Teste ins Unbekannte, ob eine Version gemeldet wird
	    if((ptr->aktHardware != 0) | (ptr->aktSoftware != 0))		// Eine Station antwortet
	    {

	        if(globalkonfig->station != NULL)
	        {
				testptr = globalkonfig->station;
	        	while(testptr != NULL)								// Solange suchen, bis Ende der Kette erreicht ist
				{
	        		if(testptr->StationAdr != addr)					// Wenn gleiche NICHT Addresse gefunden wurde
	        		{
	        			if(testptr->next == NULL)					// Am Ende der Kette keine gefundene identische Adresse
	        			{
	        		    	getDeviceinfo(ptr);						// Lade Zusatzinformatinen
	        		    	getChanList(ptr);
	        	        	ptr->lfdnr = count;							// Laufende nummer schreiben
	        	        	count++;									// lfdnr erhöhen
	        	        	testptr->next = ptr;					// Hänge neue Station an die Kette an
	        	        	ptr = malloc(sizeof(struct devdaten));	// erzeuge neue (leere) Struktur
	        	        	ptr->next = NULL;
	        	        	testptr = NULL;
	        			}
	        			else
	        				testptr = testptr->next;					// Lade nächste Station
	        		}
	        		else
	        			break;										// Addresse schon in der Kette
				}
	        }
	        else                         // "globalkonfig->station == NULL", Wenn noch keine Station definiert wurde,
	        {														// Hänge diese als erste an
	        	ptr->lfdnr = count;
	        	globalkonfig->station = ptr;
	        	count++;
	        	ptr = malloc(sizeof(struct devdaten));
	        	ptr->next = NULL;
	        }
	    }
	}
	free(ptr);							// Offener Zeiger auf Struktur löschen, da keine Station mehr erfragt wird

	if(globalkonfig->station == NULL)
	{
		printf("Keine Stationen gefunden\n");
		count = -1;
	}

	return(count);
}

// Abfrage der aktuellen Versionnummern (Hardware / Software)
// Aktuelle Daten richtig Rückgabe = 0
// Fehler Rückgabe = -1
int getVersion(struct devdaten *station)
{
    int result = 0;

    result=request(1,station,NULL,0,0);    // Versionsabfrage

    return(result);
}

// Abfrage der Stationsdaten
// Aktuelle Daten richtig Rückgabe = 0
// Fehler Rückgabe = -1
int getDeviceinfo(struct devdaten *station)
{
    int result1 = 0;
    int result2 = 0;
    int result3 = 0;
    int result4 = 0;
    int result5 = 0;

    result1=request(21,station,NULL,0,0); // Gerätebezeichnung
    result2=request(22,station,NULL,0,0); // Gerätebeschreibung
    result3=request(23,station,NULL,0,0); // Hard- und Softwareversion
    result4=request(24,station,NULL,0,0); // erweiterte Info
    result5=request(25,station,NULL,0,0); // Groesse EEPROM

    return(result1 & result2 & result3 & result4 & result5);
}

// Abfrage der Kanalkonfiguration (Scan kann mehrere Minuten dauern)
// Aktuelle Anzahl der Kanäle Rückgabe > 0
// Fehler Rückgabe = -1
int getChanList(struct devdaten *station)
{
    int result1 = 0;
    int result2 = 0;
    int result3 = 0;
    int i;
    struct kanal *ptr;

    result1 = request(26,station,NULL,0,0); // Anzahl der Kanaele und Blöcke


    if(result1)
    	for(i=0;i < station->AnzahlBloecke;i++)
    		result2 = request(27,station,NULL,i,0); // Nummern der Kanaele

    ptr = station->channels;
    if(result2)
    	for(i=0;i < station->AnzahlKanaele;i++)
    	{
    		result3 = request(28,station,station->channels,ptr->nummer,0); // Kanalinfo
    		ptr = ptr->next;
    	}

    return(result1 & result2 & result3);
}

// Abfrage der Messkanalinformation
int getChanInfo(struct devdaten *station,int chan)
{
    int result = 0;

    result = request(28,station,station->channels,chan,0); // Kanalinfo

    return(result);
}

// Abfrage eines Datenpunktes
// Anzahl der Datenpunkte Rückgabe > 0
// Fehler Rückgabe = -1
int getSingleData(struct devdaten *station, struct kanal *dp, int kanalnr)
{
    int result = 0;

    result = request(3,station,dp,kanalnr,0); // Datenabfrage ein Kanal

    return(result);
}

// Abfrage von bis zu 20 Datenpunkten
// Anzahl der  Datenpunkte Rückgabe > 0
// Fehler Rückgabe = -1
int getMultiData(struct devdaten *station, struct kanal *dp, int chanlist[])
{
    int result = 0;

    result = request(4,station,dp,0,chanlist); // Datenabfrage mehrere Kanäle

    return(result);
}

// Löst einen Softwarereset in der Station aus.
// die station ist dann 2 Sekunden nicht erreichbar.
// Aktuelle Daten richtig Rückgabe = 0
// Fehler Rückgabe = -1
int doReset(struct devdaten *station)
{
    int result = 0;

    result = request(5,station,NULL,0,0);    // Reset
    sleep(2);

    return(result);
}

// Abfrage der letzten Statusinformation
// (Rückmeldung zum letzten Befehl)
// Aktuelle Daten richtig Rückgabe >= 0
// Fehler Rückgabe = -1
int getStatus(struct devdaten *station)
{
    int result = 0;

    result = request(6,station,NULL,0,0);    // Statusabfrage

    return(result);
}

// Abfrage der letzten Fehlerinformation
// (Rückmeldung zum letzten Befehl)
// Aktuelle Daten richtig Rückgabe >= 0
// Fehler Rückgabe = -1
int getError(struct devdaten *station)
{
    int result = 0;

    result = request(7,station,NULL,0,0);    // Fehlerabfrage

    return(result);
}
