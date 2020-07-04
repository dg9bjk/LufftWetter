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
union messdatenmix								// Zum Einlesen der Daten
{
    unsigned char  a;							// Benutzt [0]
    signed char    b;							// Benutzt [0]
    unsigned short c;							// Benutzt [0][1]
    signed short   d;							// Benutzt [0][1]
    unsigned long  e;							// Benutzt [0][1][2][3]
    signed long    f;							// Benutzt [0][1][2][3]
    float          g;							// Benutzt [0][1][2][3]
    double         h;							// Benutzt [0][1][2][3][4][5][6][7]
    unsigned char z[8];							// Eingabe aus dem Datenarray der Wetterstation
};

struct livedata									// Aktuelle Livedaten
{
    int aktcntchannels;							// Aktuell Eingelesene Datenpunktemenge
    int lastStatus;								// Letzter Statuscode
    int lastError;								// Letzter Fehlercode
    int Statuscode;
    int Cmdnr;									// Kommandonummer (Interne Verarbeiterung)
    char Befehl[Befehllaenge];					// Befehlname (Interne Verarbeiterung)
    char Fehlermeldung[Befehllaenge];			// Fehlermeldung (Interne Verarbeiterung)
};

struct kanal									// Kanalinformation eines Messkanales
{
    int lfdnr;									// Laufende Nummer
    int block;									// Blocknummer
    int maxnummer;								// Maximale Kanäle
    int nummer;									// Kanalnummer
    int timestamp;								// Zeitstempel letzte Abfrage
    int zyklusSek;								// Abfrage-Zyklus
    char groesse[21];							// Name des Kaneles  - Ausgelesen aus der Station
    char einheit[16];							// Einheit  - Ausgelesen aus der Station
    int mwtyp;									// Messwert-Typ
    int datetyp;								// Datentyp
    union messdatenmix Min;						// Minimumwert (Siehe Datentyp)
    union messdatenmix Max;						// Maximumwert (Siehe Datentyp)
    union messdatenmix value;					// Aktueller Messwert (Siehe Datentyp)
    struct kanal *next;							// Nächster Kanal -> Kette
};

struct ExtInfo									// Zusätzliche Stationsdaten
{
    int lfddevnr;								// Produktionsnummer
    int proddate;								// Produktzeitpunkt (MMJJ)
    int project;								// Projektnummer
    int stueli;									// Stückliste
    int splan;									// Schaltplan
    int hardware;								// Hardware-Revision
    int software;								// Software-Revision (Auslieferung)
    int e2version;								// ???
    int deviceversion;							// Device-Revision
};

struct devdaten									// Stationsdaten (Allgemein)
{
	int lfdnr;									// Laufende Nummer
    int StationAdr;								// Stationsaddresse
    int PCAdr;									// Temporär -> Lokale PC-Addresse
    int aktHardware;							// Aktuelle Hardware-Addresse
    int aktSoftware;							// Aktuelle Software-Addresse
    int EEPROMSize;								// Grösse EEPROM
    int AnzahlKanaele;							// Anzahl der Kanäle gesammt
    int AnzahlBloecke;							// Anzahl der Blöcke (100er Kanalraster)
    int fdserial;								// File-Zeiger für Zugriff auf Seriale Kommunikation
	char *serialport;							// Name des Serialports
    char Geraetebezeichnung[Befehllaenge];		// Gerätebezeichnung - Ausgelesen aus der Station
    char Geraetebeschreibung[Befehllaenge];		// Gerätebeschreibung  - Ausgelesen aus der Station
    char serialnummer[Befehllaenge];			// Serialnummer  - Ausgelesen aus der Station über ExtInfo
    struct ExtInfo Devinfo;						// Produktionsdaten  - Ausgelesen aus der Station
    struct livedata live;						// Aktuelle Live-Daten auf der Serialschnittstelle aus der Station
    struct kanal *channels;						// Kanalliste der verfügbaren Messkanäle
    struct devdaten *next;						// Nächste Station -> Kette
};

struct serport
{
    int fdserial;								// File-Zeiger für Zugriff auf Seriale Kommunikation
	char *serialport;							// Name des Serialports
	struct serport *next;						// Nächster Serial-Port
};
struct master									// Globale Struktur für die Programmkonfiguration / Laufvariablen
{
	int PCAdr;									// Lokale PC-Adresse (Lufft-Protokoll)
	char *konfigpath;							// Lage der Konfigurationsdatei
	struct serport *serial;						// Liste der verfügbaren Serialports
	struct devdaten *station;					// Liste der Wetterstationen
};

// Muster der Funktionen für den Aufruf (public)
// serial.c
int SerialPortInit(char *serialport);
int searchport(struct master *globalkonfig);
int send(int fdserial,unsigned char array[SerialArray],int count);
int recv(int fdserial,unsigned char array[SerialArray]);
int sim(int fdserial,unsigned char arrayTX[SerialArray],unsigned char arrayRX[SerialArray],int count);

//file.c
int readKonfig(char *konfig,struct master *globalkonfig);
int searchkonfig(struct master *globalkonfig);

// lufft.c
int request(int cmd,struct devdaten *station,struct kanal *channels,int opt1,int list[]);
void printMWtyp(int mwtyp);
void printValueByDatatyp(int datatyp,union messdatenmix value);

// APIlufft.c
int getStationList(struct master *globalkonfig,struct serport serial,int typ,int start,int stop);
int getVersion(struct devdaten *station);
int getDeviceinfo(struct devdaten *station);
int getChanList(struct devdaten *station);
int getChanInfo(struct devdaten *station,int chan);
int getSingleData(struct devdaten *station, struct kanal *dp, int kanalnr);
int getMultiData(struct devdaten *station, struct kanal *dp, int chanlist[]);
int doReset(struct devdaten *station);
int getStatus(struct devdaten *station);
int getError(struct devdaten *station);
