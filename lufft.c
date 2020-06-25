#include "serial.c"
#define Befehllaenge 41

// Datenarray
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

struct kanal
{
    int lfdnr;
    int block;
    int maxnummer;
    int nummer;
    unsigned char groesse[21];
    unsigned char einheit[16];
    int mwtyp;
    int datetyp;
    union messdatenmix Min;
    union messdatenmix Max;
    union messdatenmix lastvalue;
    struct kanal *next;
};

struct devdaten
{
    int StationAddr;
    int PCAddr;
    int Hardwareversion;
    int Softwareversion;
    int EEPROMSize;
    int AnzahlKanaele;
    int AnzahlBloecke;
    int aktcntchannels;
    int lastStatus;
    int lastError;
    int Command;
    unsigned char Befehl[Befehllaenge];
    unsigned char Geraetebezeichnung[Befehllaenge];
    unsigned char Geraetebeschreibung[Befehllaenge];
    unsigned char serialnummer[Befehllaenge];
    unsigned char NummernKanaele[Befehllaenge];
    unsigned char Kanalinfo[Befehllaenge];
    unsigned char Fehlermeldung[Befehllaenge];
    struct ErweiterteInfo Devinfo;
    struct kanal *channels;
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
int request(int fdserial,int cmd,struct devdaten *station,struct kanal *channels,int i,int j);
// Sende Abfrage Versionsnummer
int encode(char arrayTX[SerialArray],int command,int fdserial,struct devdaten *daten,struct kanal *channels,int opt1,int opt2)
{
    unsigned short result;	// CRC-Berechnung
    int runstop = 0;
    int count;
    int i;
    struct kanal *ptr;

    switch(command)
    {
        case 1:			// Version
                                count = 14;
                                arrayTX[6] = 0x02; // Lauflänge
                                arrayTX[8] = 0x20; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                break;

        case 2:			// Geraeteinfo
                                request(fdserial,21,daten,channels,0,0); // Gerätebezeichnung
                                request(fdserial,22,daten,channels,0,0); // Gerätebeschreibung
                                request(fdserial,23,daten,channels,0,0); // Hard- und Softwareversion
                                request(fdserial,24,daten,channels,0,0); // erweiterte Info
                                request(fdserial,25,daten,channels,0,0); // Groesse EEPROM
                                request(fdserial,26,daten,channels,0,0); // Anzahl der Kanaele

                                for(i=0;i<daten->AnzahlBloecke;i++)
                                    request(fdserial,27,daten,channels,i,0); // Nummern der Kanaele
                                    
                                ptr = daten->channels;
                                for(i=0;i<daten->AnzahlKanaele;i++)
				{
                                    request(fdserial,28,daten,daten->channels,ptr->nummer,0); // Kanalinfo
				    ptr = ptr->next;
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
                                count = 16;
                                arrayTX[6] = 0x04;
                                arrayTX[8] = 0x23; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                arrayTX[10] = opt1 & 0x00ff; // Kanal low
                                arrayTX[11] = (opt1 >> 8) & 0x00ff; // Kanal high
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

int decode(char arrayRX[SerialArray],int count,struct devdaten *daten,struct kanal *channels)
{
    int i,j;
    char buffer[10];
    struct kanal *ptr;
    int chnummer;

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
                                                    i=0;
                                                    ptr = daten->channels;
                                                    
                                                    do
                                                    {
                                                        if(ptr == NULL)
                                                        {
                                                            ptr = malloc(sizeof(struct kanal));
                                                            daten->channels = ptr;
                                                            daten->aktcntchannels = 1;
                                                            ptr->next = NULL;
                                                        }
                                                        else
                                                        {
                                                            while(ptr->next != NULL)
                                                                ptr = ptr->next;   
                                                            ptr->next = malloc(sizeof(struct kanal));
                                                            ptr = ptr->next;
                                                            ptr->next = NULL;
                                                        }
                                                        ptr->block = arrayRX[12];
                                                        ptr->maxnummer = arrayRX[13];
                                                        ptr->nummer = (arrayRX[15+(i*2)] * 256) + arrayRX[14+(i*2)];
                                                        ptr->lfdnr = daten->aktcntchannels;
                                                        if(DEBUG)
                                                            printf("Debug: %s %d Block: %d Anzahl: %d Kanal: %d\n",daten->Befehl,i,ptr->block,ptr->maxnummer,ptr->nummer);
                                                        i++;
                                                        daten->aktcntchannels++;
                                                    }while(i < ptr->maxnummer);
                                                }
                                            else if(arrayRX[11] == 0x30)
                                                {
                                                    ptr = channels;
                                                    chnummer = (arrayRX[13] * 256) + arrayRX[12];
                                                    while(ptr->nummer != chnummer)
                                                        ptr = ptr->next;
                                                    memset(ptr->groesse,0x0,21);
                                                    for(i=0;i<arrayRX[6] & i<20;i++)
                                                        ptr->groesse[i] = arrayRX[14 + i];
                                                    memset(ptr->einheit,0x0,16);
                                                    for(i=0;i<arrayRX[6] & i<15;i++)
                                                        ptr->einheit[i] = arrayRX[34 + i];
                                                    ptr->mwtyp  = arrayRX[49];
                                                    ptr->datetyp = arrayRX[50];

                                                    if(ptr->datetyp == 0x10 || ptr->datetyp == 0x11) 
                                                    {
                                                        ptr->Min.z[0] = arrayRX[51];
                                                        ptr->Max.z[0] = arrayRX[52];
                                                    }
                                                    else if(ptr->datetyp == 0x12 || ptr->datetyp == 0x13)
                                                    {
                                                        ptr->Min.z[0] = arrayRX[51];
                                                        ptr->Min.z[1] = arrayRX[52];
                                                        ptr->Max.z[0] = arrayRX[53];
                                                        ptr->Max.z[1] = arrayRX[54];
                                                    }
                                                    else if(ptr->datetyp == 0x14 || ptr->datetyp == 0x15 || ptr->datetyp == 0x16)
                                                    {
                                                        ptr->Min.z[0] = arrayRX[51];
                                                        ptr->Min.z[1] = arrayRX[52];
                                                        ptr->Min.z[2] = arrayRX[53];
                                                        ptr->Min.z[3] = arrayRX[54];
                                                        ptr->Max.z[0] = arrayRX[55];
                                                        ptr->Max.z[1] = arrayRX[56];
                                                        ptr->Max.z[2] = arrayRX[57];
                                                        ptr->Max.z[3] = arrayRX[58];
                                                    }
                                                    else if(ptr->datetyp == 0x17)
                                                    {
                                                        ptr->Min.z[0] = arrayRX[51];
                                                        ptr->Min.z[1] = arrayRX[52];
                                                        ptr->Min.z[2] = arrayRX[53];
                                                        ptr->Min.z[3] = arrayRX[54];
                                                        ptr->Min.z[4] = arrayRX[55];
                                                        ptr->Min.z[5] = arrayRX[56];
                                                        ptr->Min.z[6] = arrayRX[57];
                                                        ptr->Min.z[7] = arrayRX[58];
                                                        ptr->Max.z[0] = arrayRX[59];
                                                        ptr->Max.z[1] = arrayRX[60];
                                                        ptr->Max.z[2] = arrayRX[61];
                                                        ptr->Max.z[3] = arrayRX[62];
                                                        ptr->Max.z[4] = arrayRX[63];
                                                        ptr->Max.z[5] = arrayRX[64];
                                                        ptr->Max.z[6] = arrayRX[65];
                                                        ptr->Max.z[7] = arrayRX[66];
                                                    }
                                                    else
                                                    {
                                                        ptr->Min.z[0] = 0x00;
                                                        ptr->Min.z[1] = 0x00;
                                                        ptr->Min.z[2] = 0x00;
                                                        ptr->Min.z[3] = 0x00;
                                                        ptr->Min.z[4] = 0x00;
                                                        ptr->Min.z[5] = 0x00;
                                                        ptr->Min.z[6] = 0x00;
                                                        ptr->Min.z[7] = 0x00;
                                                        ptr->Max.z[0] = 0x00;
                                                        ptr->Max.z[1] = 0x00;
                                                        ptr->Max.z[2] = 0x00;
                                                        ptr->Max.z[3] = 0x00;
                                                        ptr->Max.z[4] = 0x00;
                                                        ptr->Max.z[5] = 0x00;
                                                        ptr->Max.z[6] = 0x00;
                                                        ptr->Max.z[7] = 0x00;
                                                    }
                                                    
                                                    if(DEBUG)
                                                    {
                                                        printf("Debug: %s %d %s %s ",daten->Befehl,chnummer,ptr->groesse,ptr->einheit);
                                                        if(ptr->mwtyp == 0x10)
                                                            printf("%x -> Current - ",ptr->mwtyp);
                                                        else if(ptr->mwtyp == 0x11)
                                                            printf("%x -> Min - ",ptr->mwtyp);
                                                        else if(ptr->mwtyp == 0x12)
                                                            printf("%x -> Max - ",ptr->mwtyp);
                                                        else if(ptr->mwtyp == 0x13)
                                                            printf("%x -> Avg - ",ptr->mwtyp);
                                                        else if(ptr->mwtyp == 0x14)
                                                            printf("%x -> Sum - ",ptr->mwtyp);
                                                        else if(ptr->mwtyp == 0x15)
                                                            printf("%x -> Vct - ",ptr->mwtyp); 
                                                        else
                                                            printf("%x -> Unknown - ",ptr->mwtyp);
                                                            
                                                        if(ptr->datetyp == 0x10)
                                                        {
                                                            printf("%x -> U8 - ",ptr->datetyp);
                                                            printf("%d %d",ptr->Min.a,ptr->Max.a);
                                                        }
                                                        else if(ptr->datetyp == 0x11)
                                                        {
                                                            printf("%x -> S8 - ",ptr->datetyp);
                                                            printf("%d %d",ptr->Min.b,ptr->Max.b);
                                                        }
                                                        else if(ptr->datetyp == 0x12)
                                                        {
                                                            printf("%x -> U16 - ",ptr->datetyp);
                                                            printf("%d %d",ptr->Min.c,ptr->Max.c);
                                                        }
                                                        else if(ptr->datetyp == 0x13)
                                                        {
                                                            printf("%x -> S16 - ",ptr->datetyp);
                                                            printf("%d %d",ptr->Min.d,ptr->Max.d);
                                                        }
                                                        else if(ptr->datetyp == 0x14)
                                                        {
                                                            printf("%x -> U32 - ",ptr->datetyp);
                                                            printf("%ld %ld",ptr->Min.e,ptr->Max.e);
                                                        }
                                                        else if(ptr->datetyp == 0x15)
                                                        {
                                                            printf("%x -> S32 - ",ptr->datetyp);
                                                            printf("%ld %ld",ptr->Min.f,ptr->Max.f);
                                                        }
                                                        else if(ptr->datetyp == 0x16)
                                                        {
                                                            printf("%x -> Float - ",ptr->datetyp);
                                                            printf("%.3f %.3f",ptr->Min.g,ptr->Max.g);
                                                        }
                                                        else if(ptr->datetyp == 0x17)
                                                        {
                                                            printf("%x -> Double - ",ptr->datetyp);
                                                            printf("%.3f %.3f",ptr->Min.h,ptr->Max.h);
                                                        }
                                                        else
                                                        {
                                                            printf("%x -> Unknown - ",ptr->datetyp);
                                                            printf("??? ???");
                                                        }
                                                            
                                                        printf("\n");
                                                    }
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

                                            chnummer = (arrayRX[12]*256) + arrayRX[11];
                                            
                                            if(daten->channels != NULL)
                                            {
                                                ptr = daten->channels;
                                                while((ptr->next != NULL) & (chnummer != ptr->nummer))
                                                    ptr = ptr->next;   
                                            }
                                            else
                                            {	// Temporärer Kanal
                                                ptr = channels;
                                                ptr->datetyp = arrayRX[13];
                                                ptr->nummer = chnummer;
                                                ptr->next = NULL;

                                                ptr->lfdnr = 0;
                                                ptr->block = 0;
                                                ptr->maxnummer = 1;
                                                memset(ptr->groesse,0x0,21);
                                                strncpy(ptr->groesse,"TempValue",9);
                                                memset(ptr->einheit,0x0,16);
                                                strncpy(ptr->einheit,"TempValue",9);
                                                ptr->mwtyp = 0;
                                                ptr->Min.a = 0;
                                                ptr->Max.a = 0;
                                            }

                                            if((arrayRX[10] == 0x00) & (chnummer == ptr->nummer))
                                            {
                                                if(arrayRX[13] == ptr->datetyp)
                                                {
                                                    if(ptr->datetyp == 0x10 || ptr->datetyp == 0x11) 
                                                    {
                                                        ptr->lastvalue.z[0] = arrayRX[14];
                                                    }
                                                    else if(ptr->datetyp == 0x12 || ptr->datetyp == 0x13)
                                                    {
                                                        ptr->lastvalue.z[0] = arrayRX[14];
                                                        ptr->lastvalue.z[1] = arrayRX[15];
                                                    }
                                                    else if(ptr->datetyp == 0x14 || ptr->datetyp == 0x15 || ptr->datetyp == 0x16)
                                                    {
                                                        ptr->lastvalue.z[0] = arrayRX[14];
                                                        ptr->lastvalue.z[1] = arrayRX[15];
                                                        ptr->lastvalue.z[2] = arrayRX[16];
                                                        ptr->lastvalue.z[3] = arrayRX[17];
                                                    }
                                                    else if(ptr->datetyp == 0x17)
                                                    {
                                                        ptr->lastvalue.z[0] = arrayRX[14];
                                                        ptr->lastvalue.z[1] = arrayRX[15];
                                                        ptr->lastvalue.z[2] = arrayRX[16];
                                                        ptr->lastvalue.z[3] = arrayRX[17];
                                                        ptr->lastvalue.z[4] = arrayRX[18];
                                                        ptr->lastvalue.z[5] = arrayRX[19];
                                                        ptr->lastvalue.z[6] = arrayRX[20];
                                                        ptr->lastvalue.z[7] = arrayRX[21];
                                                    }
                                                    else
                                                    {
                                                        ptr->lastvalue.z[0] = 0x00;
                                                        ptr->lastvalue.z[1] = 0x00;
                                                        ptr->lastvalue.z[2] = 0x00;
                                                        ptr->lastvalue.z[3] = 0x00;
                                                        ptr->lastvalue.z[4] = 0x00;
                                                        ptr->lastvalue.z[5] = 0x00;
                                                        ptr->lastvalue.z[6] = 0x00;
                                                        ptr->lastvalue.z[7] = 0x00;
                                                    }
                                                }
                                                else
                                                    printf("Error 112: Falscher Datentyp\n");
                                            }
                                            
                                            if(DEBUG)
                                            {
                                                printf("Debug: %s %d ",daten->Befehl,chnummer);
                                                if(daten->channels != NULL)
                                                {
                                                    printf("%s ",ptr->groesse);
                                                    if(ptr->mwtyp == 0x10)
                                                        printf("%x -> Current - ",ptr->mwtyp);
                                                    else if(ptr->mwtyp == 0x11)
                                                        printf("%x -> Min - ",ptr->mwtyp);
                                                    else if(ptr->mwtyp == 0x12)
                                                        printf("%x -> Max - ",ptr->mwtyp);
                                                    else if(ptr->mwtyp == 0x13)
                                                        printf("%x -> Avg - ",ptr->mwtyp);
                                                    else if(ptr->mwtyp == 0x14)
                                                        printf("%x -> Sum - ",ptr->mwtyp);
                                                    else if(ptr->mwtyp == 0x15)
                                                        printf("%x -> Vct - ",ptr->mwtyp); 
                                                    else
                                                        printf("%x -> Unknown - ",ptr->mwtyp);
                                                }
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
                                                            
                                                if(daten->channels != NULL)
                                                    printf(" %s",ptr->einheit);
                                                        printf("\n");
                                            }
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

                                            if(arrayRX[10] == 0x00)
                                                if(DEBUG)
                                                    printf("Reset\n");
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

                                            if(arrayRX[10] == 0x00)
                                            {
                                                daten->lastStatus = arrayRX[11];
                                                if(DEBUG)
                                                    printf("Status: %d\n",daten->lastStatus);
                                            }
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

                                            if(arrayRX[10] == 0x00)
                                            {
                                                daten->lastError = arrayRX[11];
                                                if(DEBUG)
                                                    printf("Fehler: %d\n",daten->lastError);
                                            }
                                        }
                                        else
                                            printf("Error 109: Falsche Befehlsversion\n");
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
int request(int fdserial,int cmd,struct devdaten *station,struct kanal *channels,int i,int j)
{
    int count;
    char arrayTX[SerialArray];
    char arrayRX[SerialArray];

    memset(arrayTX,0,SerialArray);
    memset(arrayRX,0,SerialArray);
    
    count = encode(arrayTX,cmd,fdserial,station,channels,i,j);
    if(count > 0)
    {
        if(DEBUG)
            debugdisplay(arrayTX,count);

            send(fdserial,arrayTX,count);
        sleep(1);
        count = recv(fdserial,arrayRX);
        if(DEBUG)
            debugdisplay(arrayRX,count);

        decode(arrayRX,count,station,channels);
    }
    usleep(100000);
    return(0);
}
