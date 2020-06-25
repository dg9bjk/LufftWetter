#include "lufft.c"

// Abfrage-API für LUFFT Wetterstation.

// Die "Gefährlichen Funktionen" wurden nicht aufgenommen, weil diese die Station zerstören können.
// Konfiguration mit der Originalsoftware "ConfigTool.NET" ist pflicht, um die Gewährleistung nicht
// zu verlieren.

// Abfrage der aktuellen Versionnummern (Hardware / Software)
// Aktuelle Daten richtig Rückgabe = 0
// Fehler Rückgabe = -1
int getVersion(int stationadr,int *hardware,int *software)
{
    int result = 0;

    return(restult);
}

// Abfrage der Stationsdaten
// Aktuelle Daten richtig Rückgabe = 0
// Fehler Rückgabe = -1
int getDeviceinfo(struct devdaten *station)
{
    int result = 0;

    return(restult);
}

// Abfrage der Kanalkonfiguration (Scan kann mehrere Minuten dauern)
// Aktuelle Anzahl der Kanäle Rückgabe > 0
// Fehler Rückgabe = -1
int getChanList(struct devdaten *station,struct kanal *liste)
{
    int result = 0;

    return(restult);
}

// Abfrage eines Datenpunktes
// Anzahl der Datenpunkte Rückgabe > 0
// Fehler Rückgabe = -1
int getSingleData(struct devdaten *station,struct kanal *dp)
{
    int result = 0;

    return(restult);
}

// Abfrage von bis zu 20 Datenpunkten
// Anzahl der  Datenpunkte Rückgabe > 0
// Fehler Rückgabe = -1
int getMultiData(struct devdaten *station,struct kanal *dp[])
{
    int result = 0;

    return(restult);
}

// Löst einen Softwarereset in der Station aus.
// die station ist dann 2 Sekunden nicht erreichbar.
// Aktuelle Daten richtig Rückgabe = 0
// Fehler Rückgabe = -1
int doReset(int stationadr)
{
    int result = 0;

    return(restult);
}

// Abfrage der letzten Statusinformation
// (Rückmeldung zum letzten Befehl)
// Aktuelle Daten richtig Rückgabe >= 0
// Fehler Rückgabe = -1
int getStatus(int stationadr)
{
    int result = 0;

    return(restult);
}

// Abfrage der letzten Fehlerinformation
// (Rückmeldung zum letzten Befehl)
// Aktuelle Daten richtig Rückgabe >= 0
// Fehler Rückgabe = -1
int getError(int stationadr)
{
    int result = 0;

    return(restult);
}
