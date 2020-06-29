// Alle Includes, die benötigt werden.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

// Globale Definitionen
#define DEBUG 3				// Ausgabe von Zusatzinformationen
							// 0 = Aus
							// 1 = frei
							// 2 = Nur Programminformationen
							// 3 = Mit Protokoll der Serial-Daten

#define SIMULATION	1		// 0 = Aus
							// 1 = Simulation korrekte Daten
							// 2 = Simulation Fehler

#define Befehllaenge 41 	// Arraylänge für die Datenstrings der Station.
#define TIMEOUT 10			// Sekunden Verzögerung für den nächsten Zyklus
#define SerialArray 260     // Zeichenpuffer der Serial-Schnittstelle (max. 256 Byte möglich + 4 Reserve

// Datenstrukturen
union messdatenmix
{
    unsigned char a;
    signed char b;
    unsigned short c;
    signed short d;
    unsigned long e;
    signed long f;
    float g;
    double h;
    unsigned char z[8];
};

struct ExtInfo
{
    int lfddevnr;
    int proddate;
    int project;
    int stueli;
    int splan;
    int hardware;
    int software;
    int e2version;
    int deviceversion;
};

struct kanal
{
    int lfdnr;
    int block;
    int maxnummer;
    int nummer;
    char groesse[21];
    char einheit[16];
    int mwtyp;
    int datetyp;
    union messdatenmix Min;
    union messdatenmix Max;
    union messdatenmix lastvalue;
    struct kanal *next;
};

struct livedata
{
    int aktcntchannels;
    int lastStatus;
    int lastError;
    int Cmdnr;
    char Befehl[Befehllaenge];
    char Fehlermeldung[Befehllaenge];
};

struct devdaten
{
    int StationAdr;
    int PCAddr;
    int aktHardware;
    int aktSoftware;
    int EEPROMSize;
    int AnzahlKanaele;
    int AnzahlBloecke;
    char Geraetebezeichnung[Befehllaenge];
    char Geraetebeschreibung[Befehllaenge];
    char serialnummer[Befehllaenge];
    struct ExtInfo Devinfo;
    struct kanal *channels;
};

// Muster der Funktionen für den Aufruf (public)
// serial.c
int SerialPortInit();
int send(int fdserial,unsigned char array[SerialArray],int count);
int recv(int fdserial,unsigned char array[SerialArray]);
int sim(int fdserial,unsigned char arrayTX[SerialArray],unsigned char arrayRX[SerialArray],int count);

// lufft.c
int request(int fdserial,int cmd,struct devdaten *station,struct kanal *channels,struct livedata *aktdata,int i,int j);

// APIlufft.c
int getVersion(int fdserial,struct devdaten *station,struct livedata *aktdata);
int getDeviceinfo(int fdserial,struct devdaten *station,struct livedata *aktdata);
int getChanList(int fdserial,struct devdaten *station,struct livedata *aktdata);
int getSingleData(int fdserial, struct devdaten *station, struct kanal *dp,struct livedata *aktdata,int opt1);
int getMultiData(int fdserial,struct devdaten *station,struct kanal *dp,struct livedata *aktdata,int opt1);
int doReset(int fdserial,struct devdaten *station,struct livedata *aktdata);
int getStatus(int fdserial,struct devdaten *station,struct livedata *aktdata);
int getError(int fdserial,struct devdaten *station,struct livedata *aktdata);

