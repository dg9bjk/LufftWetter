#include "wetterstation.h"

// Abfrage-API für LUFFT Wetterstation.

// Die "Gefährlichen Funktionen" wurden nicht aufgenommen, weil diese die Station zerstören können.
// Konfiguration mit der Originalsoftware "ConfigTool.NET" ist pflicht, um die Gewährleistung nicht
// zu verlieren.

// Scan des Netzwerkes nach Wetterstation
int getStationList(struct master *globalkonfig,int typ,int start,int stop)
{
	int count = 0;
	int i,j;
	int addr;

	if(start < 1)
			start = 1;
	if(stop > 255)
		stop = 255;
	if (typ < 1)
		typ = 1;
	if (typ > 10)
		typ = 10;

	if(globalkonfig->station != NULL)
	{
		for(i = start,j=0;i <= stop;i++)
		{
		    addr = (typ & 0x000F) << 12;
		    addr += (i & 0x00FF);
		    globalkonfig->station->StationAdr = addr;
		    globalkonfig->station->aktHardware = 0;
		    globalkonfig->station->aktSoftware = 0;
	    	request(globalkonfig->station->fdserial,1,globalkonfig->station,NULL,globalkonfig->station->live,0,0);
		    if((globalkonfig->station->aktHardware != 0) | (globalkonfig->station->aktSoftware != 0))
		    {
		        request(globalkonfig->station->fdserial,21,globalkonfig->station,NULL,globalkonfig->station->live,0,0); // Gerätebezeichnung
		        request(globalkonfig->station->fdserial,22,globalkonfig->station,NULL,globalkonfig->station->live,0,0); // Gerätebeschreibung
		        j++;
		        globalkonfig->station->next = malloc(sizeof(struct devdaten));
		        globalkonfig->station = globalkonfig->station->next;
		    }
		}
	count = j;
	}
	else
	{
		printf("Keine Stationen definiert\n");
		count = -1;
	}
	return(count);
}

// Abfrage der aktuellen Versionnummern (Hardware / Software)
// Aktuelle Daten richtig Rückgabe = 0
// Fehler Rückgabe = -1
int getVersion(struct devdaten *station)
{
    int result = 1;
    request(station->fdserial,1,station,NULL,station->live,0,0);    // Versionsabfrage
    return(result);
}

// Abfrage der Stationsdaten
// Aktuelle Daten richtig Rückgabe = 0
// Fehler Rückgabe = -1
int getDeviceinfo(struct devdaten *station)
{
    int result = 0;

    request(station->fdserial,21,station,NULL,station->live,0,0); // Gerätebezeichnung
    request(station->fdserial,22,station,NULL,station->live,0,0); // Gerätebeschreibung
    request(station->fdserial,23,station,NULL,station->live,0,0); // Hard- und Softwareversion
    request(station->fdserial,24,station,NULL,station->live,0,0); // erweiterte Info
    request(station->fdserial,25,station,NULL,station->live,0,0); // Groesse EEPROM

    return(result);
}

// Abfrage der Kanalkonfiguration (Scan kann mehrere Minuten dauern)
// Aktuelle Anzahl der Kanäle Rückgabe > 0
// Fehler Rückgabe = -1
int getChanList(struct devdaten *station)
{
    int result = 0;
    int i;
    struct kanal *ptr;

    request(station->fdserial,26,station,NULL,station->live,0,0); // Anzahl der Kanaele

    for(i=0;i < station->AnzahlBloecke;i++)
        request(station->fdserial,27,station,NULL,station->live,i,0); // Nummern der Kanaele

    ptr = station->channels;
    for(i=0;i < station->AnzahlKanaele;i++)
    {
        request(station->fdserial,28,station,station->channels,station->live,ptr->nummer,0); // Kanalinfo
        ptr = ptr->next;
    }
    return(result);
}

// Abfrage eines Datenpunktes
// Anzahl der Datenpunkte Rückgabe > 0
// Fehler Rückgabe = -1
int getSingleData(struct devdaten *station, struct kanal *dp, int kanalnr)
{
    int result = 0;
    request(station->fdserial,3,station,dp,station->live,kanalnr,0); // Datenabfrage ein Kanal
    return(result);
}

// Abfrage von bis zu 20 Datenpunkten
// Anzahl der  Datenpunkte Rückgabe > 0
// Fehler Rückgabe = -1
int getMultiData(struct devdaten *station, struct kanal *dp, int chanlist[])
{
    int result = 0;
    request(station->fdserial,4,station,dp,station->live,0,chanlist); // Datenabfrage mehrere Kanäle
    return(result);
}

// Löst einen Softwarereset in der Station aus.
// die station ist dann 2 Sekunden nicht erreichbar.
// Aktuelle Daten richtig Rückgabe = 0
// Fehler Rückgabe = -1
int doReset(struct devdaten *station)
{
    int result = 0;
    request(station->fdserial,5,station,NULL,station->live,0,0);    // Reset
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
    request(station->fdserial,6,station,NULL,station->live,0,0);    // Statusabfrage
    return(result);
}

// Abfrage der letzten Fehlerinformation
// (Rückmeldung zum letzten Befehl)
// Aktuelle Daten richtig Rückgabe >= 0
// Fehler Rückgabe = -1
int getError(struct devdaten *station)
{
    int result = 0;
    request(station->fdserial,7,station,NULL,station->live,0,0);    // Fehlerabfrage
    return(result);
}
