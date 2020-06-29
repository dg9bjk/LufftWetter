#include "wetterstation.h"

// Abfrage-API für LUFFT Wetterstation.

// Die "Gefährlichen Funktionen" wurden nicht aufgenommen, weil diese die Station zerstören können.
// Konfiguration mit der Originalsoftware "ConfigTool.NET" ist pflicht, um die Gewährleistung nicht
// zu verlieren.

// Abfrage der aktuellen Versionnummern (Hardware / Software)
// Aktuelle Daten richtig Rückgabe = 0
// Fehler Rückgabe = -1
int getVersion(int fdserial,struct devdaten *station,struct livedata *aktdata)
{
    int result = 0;
    request(fdserial,1,station,NULL,aktdata,0,0);    // Versionsabfrage
    return(result);
}

// Abfrage der Stationsdaten
// Aktuelle Daten richtig Rückgabe = 0
// Fehler Rückgabe = -1
int getDeviceinfo(int fdserial,struct devdaten *station,struct livedata *aktdata)
{
    int result = 0;

    request(fdserial,21,station,NULL,aktdata,0,0); // Gerätebezeichnung
    request(fdserial,22,station,NULL,aktdata,0,0); // Gerätebeschreibung
    request(fdserial,23,station,NULL,aktdata,0,0); // Hard- und Softwareversion
    request(fdserial,24,station,NULL,aktdata,0,0); // erweiterte Info
    request(fdserial,25,station,NULL,aktdata,0,0); // Groesse EEPROM

    return(result);
}

// Abfrage der Kanalkonfiguration (Scan kann mehrere Minuten dauern)
// Aktuelle Anzahl der Kanäle Rückgabe > 0
// Fehler Rückgabe = -1
int getChanList(int fdserial,struct devdaten *station,struct livedata *aktdata)
{
    int result = 0;
    int i;
    struct kanal *ptr;

    request(fdserial,26,station,NULL,aktdata,0,0); // Anzahl der Kanaele

    for(i=0;i < station->AnzahlBloecke;i++)
        request(fdserial,27,station,NULL,aktdata,i,0); // Nummern der Kanaele

    ptr = station->channels;
    for(i=0;i < station->AnzahlKanaele;i++)
    {
        request(fdserial,28,station,station->channels,aktdata,ptr->nummer,0); // Kanalinfo
        ptr = ptr->next;
    }

    return(result);
}

// Abfrage eines Datenpunktes
// Anzahl der Datenpunkte Rückgabe > 0
// Fehler Rückgabe = -1
int getSingleData(int fdserial, struct devdaten *station, struct kanal *dp,struct livedata *aktdata, int opt1)
{
    int result = 0;
    request(fdserial,3,station,NULL,aktdata,opt1,0); // Datenabfrage ein Kanal
    return(result);
}

// Abfrage von bis zu 20 Datenpunkten
// Anzahl der  Datenpunkte Rückgabe > 0
// Fehler Rückgabe = -1
int getMultiData(int fdserial,struct devdaten *station,struct kanal *dp,struct livedata *aktdata, int opt1)
{
    int result = 0;
    request(fdserial,4,station,NULL,aktdata,opt1,0); // Datenabfrage mehrere Kanäle
    return(result);
}

// Löst einen Softwarereset in der Station aus.
// die station ist dann 2 Sekunden nicht erreichbar.
// Aktuelle Daten richtig Rückgabe = 0
// Fehler Rückgabe = -1
int doReset(int fdserial,struct devdaten *station,struct livedata *aktdata)
{
    int result = 0;
    request(fdserial,5,station,NULL,aktdata,0,0);    // Reset
    sleep(2);
    return(result);
}

// Abfrage der letzten Statusinformation
// (Rückmeldung zum letzten Befehl)
// Aktuelle Daten richtig Rückgabe >= 0
// Fehler Rückgabe = -1
int getStatus(int fdserial,struct devdaten *station,struct livedata *aktdata)
{
    int result = 0;
    request(fdserial,6,station,NULL,aktdata,0,0);    // Statusabfrage
    return(result);
}

// Abfrage der letzten Fehlerinformation
// (Rückmeldung zum letzten Befehl)
// Aktuelle Daten richtig Rückgabe >= 0
// Fehler Rückgabe = -1
int getError(int fdserial,struct devdaten *station,struct livedata *aktdata)
{
    int result = 0;
    request(fdserial,7,station,NULL,aktdata,0,0);    // Fehlerabfrage
    return(result);
}
