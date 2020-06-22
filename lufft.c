#include "serial.c"
#define Befehllaenge 41

// Datenarray
struct ErweiterteInfo
{
    int Lfdnr;
    int Proddate;
    int Project;
    int stueli;
    int splan;
    int hardware;
    int software;
    int e2version;
    int diveversion;
};

struct Kanal
{
    int block;
    int maxnummer;
    int nummer;
    char groesse[21];
    char einheit[16];
    int mwtyp;
    int datetyp;
    char min[21];
    char max[21];
    struct Kanal *next;
};

struct lufftdaten
{
    int StationAddr;
    int PCAddr;
    int Hardwareversion;
    int Softwareversion;
    int EEPROMSize;
    int AnzahlKanaele;
    int AnzahlBloecke;
    int Command;
    char Befehl[Befehllaenge];
    char Geraetebezeichnung[Befehllaenge];
    char Geraetebeschreibung[Befehllaenge];
    char serialnummer[Befehllaenge];
    char NummernKanaele[Befehllaenge];
    char Kanalinfo[Befehllaenge];
    char Fehlermeldung[Befehllaenge];
    struct ErweiterteInfo Devinfo;
    struct Kanal *channel;
};

// Display der Serialdaten
void debugdisplay(char array[SerialArray],int count)
{
    int i;

    printf(" - Debug Länge %d: - ",count);
    for(i=0;i<count;i++)
        printf("%02x - ",array[i]);
    printf("\n");
}


// Berechnung der Checksumme
unsigned short crc_sum(char array[SerialArray],int count) // DatenArray von SOH bis ETX.
{
    int n;
    unsigned char i;
    unsigned short x16;
    unsigned short crc_buff;
    char puffer;
    
    crc_buff = 0xffff;	// Startwert
    
    for(n=0;n<count-3;n++) // Die letzten Positionen sind Checksumme und EOT - nicht in der Checksumme
    {
        puffer = array[n];
        for(i=0;i<8;i++)
        {
            if((crc_buff & 0x0001) ^ (puffer & 0x01))
                x16 = 0x8408;
            else
                x16 = 0x0000;
        
            crc_buff = crc_buff >> 1;
            crc_buff ^= x16;
            puffer = puffer >> 1;
        }
    }
    return(crc_buff);
}
//Musterfunktion -> am Ende zu finden
int request(int fdserial,int cmd,struct lufftdaten *station,int i,int j);
// Sende Abfrage Versionsnummer
int encode(char arrayTX[SerialArray],int command,int fdserial,struct lufftdaten *daten,int opt1,int opt2)
{
    unsigned short result;	// CRC-Berechnung
    int runstop = 0;
    int count;
    int i;
    int *ptr;

    switch(command)
    {
        case 1:			// Version
                                count = 14;
                                arrayTX[6] = 0x02; // Lauflänge
                                arrayTX[8] = 0x20; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                break;

        case 2:			// Geraeteinfo
                                request(fdserial,21,daten,0,0); // Gerätebezeichnung
                                request(fdserial,22,daten,0,0); // Gerätebeschreibung
                                request(fdserial,23,daten,0,0); // Hard- und Softwareversion
                                request(fdserial,24,daten,0,0); // erweiterte Info
                                request(fdserial,25,daten,0,0); // Groesse EEPROM
                                request(fdserial,26,daten,0,0); // Anzahl der Kanaele
                                for(i=0;i<daten->AnzahlBloecke;i++)
                                    request(fdserial,27,daten,i,0); // Nummern der Kanaele
                                    
                                ptr = &daten->channel;
                                for(i=0;i<daten->AnzahlKanaele;i++)
				{
                                    request(fdserial,28,daten,ptr->nummer,0); // Kanalinfo
				    ptr= daten->channel->next;
				}
                                count=0;
                                runstop = 1;
                                break;

        case 21:		// Gerätebezeichnung 10hex
                                count = 15;
                                arrayTX[6] = 0x03;
                                arrayTX[8] = 0x2D; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                arrayTX[10] = 0x10; // Gerätebezeichnung
                                break;

        case 22:		// Gerätebeschreibung 11hex
                                count = 15;
                                arrayTX[6] = 0x03;
                                arrayTX[8] = 0x2D; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                arrayTX[10] = 0x11; // Gerätebeschreibung
                                break;

        case 23:		// Hard- und Softwareversion 12hex
                                count = 15;
                                arrayTX[6] = 0x03;
                                arrayTX[8] = 0x2D; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                arrayTX[10] = 0x12; // Hard- und Softwareversion
                                break;

        case 24:		// erweiterte Vesionsinfo 13hex
                                count = 15;
                                arrayTX[6] = 0x03;
                                arrayTX[8] = 0x2D; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                arrayTX[10] = 0x13; // erweiterte Versionsinfo
                                break;

        case 25:		// Grösse EEPROM 14hex
                                count = 15;
                                arrayTX[6] = 0x03;
                                arrayTX[8] = 0x2D; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                arrayTX[10] = 0x14; // Grösse EEPROM
                                break;

        case 26:		// Anzahl verfügbare Kanäle 15hex
                                count = 15;
                                arrayTX[6] = 0x03;
                                arrayTX[8] = 0x2D; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                arrayTX[10] = 0x15; // Anzahl verfügbare Kanäle
                                break;

        case 27:		// Nummern der Kanäle 16hex
                                count = 16;
                                arrayTX[6] = 0x04;
                                arrayTX[8] = 0x2D; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                arrayTX[10] = 0x16; // Nummern der Kanäle
                                arrayTX[11] = opt1; // Blocknummer
                                break;

        case 28:		// komplette Kanalinfo 30hex
                                count = 17;
                                arrayTX[6] = 0x05;
                                arrayTX[8] = 0x2D; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                arrayTX[10] = 0x30; // Messwerttyp
                                arrayTX[11] = opt1 & 0x00ff; // Kanal low
                                arrayTX[12] = (opt1 >> 8) & 0x00ff; // Kanal high
                                break;

        case 3:			// Onlinedaten Einzeln
                                count = 14;
                                arrayTX[6] = 0x02;
                                arrayTX[8] = 0x23; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                break;

        case 4:			// Onlinedaten Mehrfach
                                count = 14;
                                arrayTX[6] = 0x02;
                                arrayTX[8] = 0x2F; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                break;

        case 5:			// Reset
                                count = 15;
                                arrayTX[6] = 0x03;
                                arrayTX[8] = 0x25; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                arrayTX[10] = 0x10; // Parameter
                                break;

        case 6:			// Status
                                count = 14;
                                arrayTX[6] = 0x02;
                                arrayTX[8] = 0x26; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                break;

        case 7:			// Fehlermeldung
                                count = 14;
                                arrayTX[6] = 0x02;
                                arrayTX[8] = 0x2C; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                break;

        case 8:			// UhrzeitSet
                                count = 18;
                                arrayTX[6] = 0x06;
                                arrayTX[8] = 0x27; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                arrayTX[10] = 0x00; // Unixzeit  0- 8
                                arrayTX[11] = 0x00; // Unixzeit  9-16
                                arrayTX[12] = 0x00; // Unixzeit 17-24
                                arrayTX[13] = 0x00; // Unixzeit 25-32
                                break;

        case 9:			// UhrzeitGet
                                count = 14;
                                arrayTX[6] = 0x02;
                                arrayTX[8] = 0x28; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                break;

        default:		count=0;
                                runstop=1;
                                break;
    }

    if(!runstop)
    {
        arrayTX[0] = 0x01; //SOH - fix
        arrayTX[1] = 0x10; // Version - fix
        arrayTX[2] = 0x01; // Low-Adresse Station
        arrayTX[3] = 0x70; // High-Adresse Station
        arrayTX[4] = 0x01; // Low-Adresse Master, PC
        arrayTX[5] = 0xF0; // High-Adresse Master, PC
        arrayTX[7] = 0x02; // STX - fix
        arrayTX[8 + arrayTX[6]]= 0x03; // ETX - fix
        arrayTX[11 + arrayTX[6]]= 0x04; // EOT - fix

        // CRC-Summe
        result = crc_sum(arrayTX,count);
        arrayTX[9 + arrayTX[6]]= result & 0x00FF;
        arrayTX[10 + arrayTX[6]]= ((result = result >> 8) & 0x00FF);
    }
    
    return(count);
}

int LufftAddr(unsigned char klasse,unsigned char addresse)
{
    int mainaddr;
    
    mainaddr = (klasse & 0x00ff) << 8;
    mainaddr += (addresse & 0x00FF);
    
    return(mainaddr);
}

int decode(char arrayRX[SerialArray],int count,struct lufftdaten *daten)
{
    int i;
    char buffer[10];

    if((arrayRX[0] == 0x01) & (arrayRX[7] == 0x02) & (arrayRX[8 + arrayRX[6]] == 0x03) & (arrayRX[11 + arrayRX[6]] == 0x04)) //SOH + STX + ETX + EOT
    {
        if(arrayRX[1] == 0x10) // Protokoll-Version 1.0
        {
            if((LufftAddr(arrayRX[5],arrayRX[4]) == daten->StationAddr) & (LufftAddr(arrayRX[3],arrayRX[2]) == daten->PCAddr))
            {
                switch(arrayRX[8]) // Befehl
                {
                    case 0x20:		// Hard- und Software
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(daten->Befehl,0x0,Befehllaenge);
                                            strncpy(daten->Befehl,"Version",7);
                                            daten->Command = 1;
                                            daten->Hardwareversion = arrayRX[11];
                                            daten->Softwareversion = arrayRX[12];
                                            if(DEBUG)
                                                printf("Debug: %s Hardware: %.1f Software: %.1f\n",daten->Befehl,(float)daten->Hardwareversion/10,(float)daten->Softwareversion/10);
                                        }
                                        else
                                            printf("Error 103: Falsche Befehlsversion\n");
                                        break;

                    case 0x2D:		// Geräteinformationen
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(daten->Befehl,0x0,Befehllaenge);
                                            strncpy(daten->Befehl,"Geraeteinfo",11);
                                            daten->Command = 2;
                                            if(arrayRX[11] == 0x10)
                                                {
                                                    memset(daten->Geraetebezeichnung,0x0,Befehllaenge);
                                                    for(i=0;i<arrayRX[6] & i<Befehllaenge-1;i++)
                                                        daten->Geraetebezeichnung[i] = arrayRX[12 + i];
                                                    if(DEBUG)
                                                        printf("Debug: %s Gerätebezeichnung: %s\n",daten->Befehl,daten->Geraetebezeichnung);

                                                }
                                            else if(arrayRX[11] == 0x11)
                                                {
                                                    memset(daten->Geraetebeschreibung,0x0,Befehllaenge);
                                                    for(i=0;i<arrayRX[6] & i<Befehllaenge-1;i++)
                                                        daten->Geraetebeschreibung[i] = arrayRX[12 + i];
                                                    if(DEBUG)
                                                        printf("Debug: %s Gerätebeschreibung: %s\n",daten->Befehl,daten->Geraetebeschreibung);
                                                }
                                            else if(arrayRX[11] == 0x12)
                                                {
                                                    daten->Hardwareversion = arrayRX[12];
                                                    daten->Softwareversion = arrayRX[13];
                                                    if(DEBUG)
                                                        printf("Debug: %s Hardware: %.1f Software: %.1f\n",daten->Befehl,(float)daten->Hardwareversion/10,(float)daten->Softwareversion/10);
                                                }
                                            else if(arrayRX[11] == 0x13)
                                                {
                                                    daten->Devinfo.Lfdnr = (arrayRX[13] * 256) + arrayRX[12];
                                                    daten->Devinfo.Proddate = (arrayRX[15] * 256) + arrayRX[14];
                                                    daten->Devinfo.Project = (arrayRX[17] * 256) + arrayRX[16];
                                                    daten->Devinfo.stueli = arrayRX[18];
                                                    daten->Devinfo.splan = arrayRX[19];
                                                    daten->Devinfo.hardware = arrayRX[20];
                                                    daten->Devinfo.software = arrayRX[21];
                                                    daten->Devinfo.e2version = arrayRX[22];
                                                    daten->Devinfo.diveversion = (arrayRX[24] * 256) + arrayRX[23];
                                                    
                                                    memset(daten->serialnummer,0x0,Befehllaenge);
                                                    sprintf(daten->serialnummer,"%d.%04d.%d.%d\0",daten->Devinfo.Lfdnr,daten->Devinfo.Proddate,daten->Devinfo.Project,daten->Devinfo.diveversion);

                                                    if(DEBUG)
                                                    {
                                                        printf("Debug: %s Erweiterte Projektdaten: %d %04d %d %d %d %d %d %d %d\n",daten->Befehl,daten->Devinfo.Lfdnr,daten->Devinfo.Proddate,daten->Devinfo.Project,daten->Devinfo.stueli, daten->Devinfo.splan,daten->Devinfo.hardware,daten->Devinfo.software,daten->Devinfo.e2version,daten->Devinfo.diveversion);
                                                        printf("Debug: %s Seriennummer:%s\n",daten->Befehl,daten->serialnummer);
                                                    }
                                                }
                                            else if(arrayRX[11] == 0x14)
                                                {
                                                    daten->EEPROMSize = (arrayRX[13] * 256) + arrayRX[12];
                                                    if(DEBUG)
                                                        printf("Debug: %s EEPROM: %d Byte\n",daten->Befehl,daten->EEPROMSize);
                                                }
                                            else if(arrayRX[11] == 0x15)
                                                {
                                                    daten->AnzahlKanaele = (arrayRX[13] * 256) + arrayRX[12];
                                                    daten->AnzahlBloecke = arrayRX[14];
                                                    if(DEBUG)
                                                        printf("Debug: %s %d Kanäle in %d Blöcken\n",daten->Befehl,daten->AnzahlKanaele,daten->AnzahlBloecke);
                                                }
                                            else if(arrayRX[11] == 0x16)
                                                {
                                                    daten->channel->block = arrayRX[12];
                                                    daten->channel->maxnummer = arrayRX[13];
                                                    for(i=0;i<daten->channel->maxnummer;i++)
                                                    {
                                                        daten->channel->nummer = (arrayRX[15+(i*2)] * 256) + arrayRX[14+(i*2)];
                                                        daten->channel->next = malloc(sizeof(struct Kanal));
                                                    if(DEBUG)
                                                        printf("Debug: %s Block: %d Anzahl: %d Kanal: %d\n",daten->Befehl,daten->channel->block,daten->channel->maxnummer,daten->channel->nummer);
                                                    }
                                                }
                                            else if(arrayRX[11] == 0x30)
                                                {
                                                    memset(daten->Kanalinfo,0x0,Befehllaenge);
                                                    for(i=0;i<arrayRX[6] & i<Befehllaenge-1;i++)
                                                        daten->Kanalinfo[i] = arrayRX[12 + i];
                                                    if(DEBUG)
                                                        printf("Debug: %s %s\n",daten->Befehl,daten->Kanalinfo);
                                                }
                                            else
                                                {
                                                memset(daten->Fehlermeldung,0x0,Befehllaenge);
                                                strncpy(daten->Fehlermeldung,"Keine Daten lesbar!",19);
                                                if(DEBUG)
                                                    printf("Debug: %s %s\n",daten->Befehl,daten->Fehlermeldung);
                                                }
                                        }
                                        else
                                            printf("Error 110: Falsche Befehlsversion\n");
                                        break;

                    case 0x23:		// Onlinedaten Einzelabfrage
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(daten->Befehl,0x0,Befehllaenge);
                                            strncpy(daten->Befehl,"OnlinedatenE",12);
                                            daten->Command = 3;
                                        }
                                        else
                                            printf("Error 104: Falsche Befehlsversion\n");
                                        break;

                    case 0x2F:		// Onlinedaten Mehrfachabfrage
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(daten->Befehl,0x0,Befehllaenge);
                                            strncpy(daten->Befehl,"OnlinedatenM",12);
                                            daten->Command = 4;
                                        }
                                        else
                                            printf("Error 111: Falsche Befehlsversion\n");
                                        break;

                    case 0x25:		// Reset / Default
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(daten->Befehl,0x0,Befehllaenge);
                                            strncpy(daten->Befehl,"Reset",5);
                                            daten->Command = 5;
                                        }
                                        else
                                            printf("Error 105: Falsche Befehlsversion\n");
                                        break;

                    case 0x26:		// Statusabfrage
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(daten->Befehl,0x0,Befehllaenge);
                                            strncpy(daten->Befehl,"Status",6);
                                            daten->Command = 6;
                                        }
                                        else
                                            printf("Error 106: Falsche Befehlsversion\n");
                                        break;

                    case 0x2C:		// letzte Fehlermeldung
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(daten->Befehl,0x0,Befehllaenge);
                                            strncpy(daten->Befehl,"Fehlermeldung",13);
                                            daten->Command = 7;
                                        }
                                        else
                                            printf("Error 109: Falsche Befehlsversion\n");
                                        break;

                    case 0x27:		// Uhrzeit setzen
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(daten->Befehl,0x0,Befehllaenge);
                                            strncpy(daten->Befehl,"UhrzeitSet",10);
                                            daten->Command = 8;
                                        }
                                        else
                                            printf("Error 107: Falsche Befehlsversion\n");
                                        break;
                    
                    case 0x28:		// Uhrzeit lesen
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(daten->Befehl,0x0,Befehllaenge);
                                            strncpy(daten->Befehl,"UhrzeitGet",10);
                                            daten->Command = 9;
                                        }
                                        else
                                            printf("Error 108: Falsche Befehlsversion\n");
                                        break;

                    default:		printf("Error 102: Unbekannter Befehl");
                                        memset(daten->Befehl,0x0,Befehllaenge);
                                        strncpy(daten->Befehl,"Error 102",9);
                                        daten->Command = 0;
                                        break;

                }
            }
            else
                if(DEBUG)
                    printf("Debug: Falsche Adressierung.\n");
        }
        else
            printf("Error 101: Protokoll-Version nicht unterstützt.\n");
    }
    else
        printf("Error 100: Protokoll-Rahmen fehlerhaft.\n");
}

// Abfrage der Daten 
int request(int fdserial,int cmd,struct lufftdaten *station,int i,int j)
{
    int count;
    char arrayTX[SerialArray];
    char arrayRX[SerialArray];

    memset(arrayTX,0,SerialArray);
    memset(arrayRX,0,SerialArray);
    
    count = encode(arrayTX,cmd,fdserial,station,i,j);
    if(count > 0)
    {
        if(DEBUG)
            debugdisplay(arrayTX,count);

            send(fdserial,arrayTX,count);
        sleep(1);
        count = recv(fdserial,arrayRX);
        if(DEBUG)
            debugdisplay(arrayRX,count);

        decode(arrayRX,count,station);
    }

    return(0);
}
