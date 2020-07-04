#include "wetterstation.h"

// Display der Serialdaten
void debugserial(unsigned char array[SerialArray],int count)
{
    int i;

    printf(" - Debug Länge %d: - ",count);
    for(i=0;i<count;i++)
        printf("%02x - ",(unsigned char)array[i]);
    printf("\n");
}

// Ausgabe des Messwerttypes
void printMWtyp(int mwtyp)
{
	if(mwtyp == 0x10)
		printf("- (%x) Current - ",mwtyp);
	else if(mwtyp == 0x11)
		printf("- (%x) Min - ",mwtyp);
	else if(mwtyp == 0x12)
		printf("- (%x) Max - ",mwtyp);
	else if(mwtyp == 0x13)
		printf("- (%x) Avg - ",mwtyp);
	else if(mwtyp == 0x14)
		printf("- (%x) Sum - ",mwtyp);
	else if(mwtyp == 0x15)
		printf("- (%x) Vct - ",mwtyp);
	else
		printf("- (%x) Unknown - ",mwtyp);
}

// Ausgabe des Datenpunktes, nach Datentyp
void printValueByDatatyp(int datatyp,union messdatenmix value)
{
	if(datatyp == 0x10)
	   printf("%d",value.a);
	else if(datatyp == 0x11)
	   printf("%d",value.b);
	else if(datatyp == 0x12)
	   printf("%d",value.c);
	else if(datatyp == 0x13)
	   printf("%d",value.d);
	else if(datatyp == 0x14)
	   printf("%ld",value.e);
	else if(datatyp == 0x15)
	   printf("%ld",value.f);
	else if(datatyp == 0x16)
	   printf("%.3f",value.g);
	else if(datatyp == 0x17)
	   printf("%.3f",value.h);
	else
	   printf("Kein gültiger Wert\n");
}

// Ausgabe Status bei Rückmeldung in Text
void printerrorcode(int code)
{
	switch(code)
	{
		case 0x0:	printf(" OK "); // OK - Keine Ausgabe
					break;
		case 0x10:	printf(" unbekanntes Kommando "); // unbekanntes Kommando
					break;
		case 0x11:	printf(" ungültige Parameter "); // ungültige Parameter
					break;
		case 0x12:	printf(" ungültige Header-Version "); // ungültige Header-Version
					break;
		case 0x13:	printf(" ungültige Version des Befehls "); // ungültige Version des Befehls
					break;
		case 0x14:	printf(" Passwort für Kommando falsch "); // Passwort für Kommando falsch
					break;
		case 0x20:	printf(" Lesefehler "); // Lesefehler
					break;
		case 0x21:	printf(" Schreibfehler "); // Schreibfehler
					break;
		case 0x22:	printf(" Länge zu groß "); // Länge zu groß; max. zulässige Länge wird in <maxlength> angegeben
					break;
		case 0x23:	printf(" ungültige Adresse "); // ungültige Adresse
					break;
		case 0x24:	printf(" ungültiger Kanal "); // ungültiger Kanal
					break;
		case 0x25:	printf(" Kommando in diesem Modus nicht möglich "); // Kommando in diesem Modus nicht möglich
					break;
		case 0x26:	printf(" unbekanntes Test-/Abgleich-Kommando "); // unbekanntes Test-/Abgleich-Kommando
					break;
		case 0x27:	printf(" Fehler bei der Kalibrierung "); // Fehler bei der Kalibrierung
					break;
		case 0x28:	printf(" Gerät nicht bereit "); // Gerät nicht bereit; z.B. Initialisierung / Kalibrierung läuft <channel>
					break;
		case 0x29:	printf(" Unterspannung "); // Unterspannung
					break;
		case 0x2A:	printf(" Hardwarefehler "); // Hardwarefehler
					break;
		case 0x2B:	printf(" Fehler in der Messung "); // Fehler in der Messung
					break;
		case 0x2C:	printf(" Fehler bei der Geräteinitialisierung "); // Fehler bei der Geräteinitialisierung
					break;
		case 0x2D:	printf(" Fehler im Betriebssystem "); // Fehler im Betriebssystem
					break;
		case 0x30:	printf(" Fehler in der Konfiguration "); // Fehler in der Konfiguration, Default-Konfiguration wurde geladen
					break;
		case 0x31:	printf(" Fehler im Abgleich "); //Fehler im Abgleich / der Abgleich ist ungültig, Messung nicht möglich
					break;
		case 0x32:	printf(" CRC-Fehler beim Laden der Konfiguration "); // CRC-Fehler beim Laden der Konfiguration; Default-Konfiguration wurde geladen
					break;
		case 0x33:	printf(" CRC-Fehler beim Laden der Abgleich-Daten "); // CRC-Fehler beim Laden der Abgleich-Daten; Messung nicht möglich
					break;
		case 0x34:	printf(" Abgleich Step 1 "); // Abgleich Step 1
					break;
		case 0x35:	printf(" Abgleich OK "); // Abgleich OK
					break;
		case 0x36:	printf(" Kanal deaktiviert "); // Kanal deaktiviert
					break;
		case 0x50:	printf(" Messgröße liegt oberhalb des eingestellten Darstellungsbereichs "); // Messgröße liegt oberhalb des eingestellten Darstellungsbereichs
					break;
		case 0x51:	printf(" Messgröße liegt unterhalb des eingestellten Darstellungsbereichs "); // Messgröße liegt unterhalb des eingestellten Darstellungsbereichs
					break;
		case 0x52:	printf(" Messwert liegt oberhalb des Messbereichs "); // Messwert liegt oberhalb des Messbereichs
					break;
		case 0x53:	printf(" Messwert liegt unterhalb des Messbereichs "); // Messwert liegt unterhalb des Messbereichs
					break;
		case 0x54:	printf(" Datenfehler in den Messdaten oder keine gültigen Daten vorhanden "); // Datenfehler in den Messdaten oder keine gültigen Daten vorhanden
					break;
		case 0x55:	printf(" Gerät / Sensor kann auf Grund der Umgebungsbedingungen keine gültige Messung durchführen "); // Gerät / Sensor kann auf Grund der Umgebungsbedingungen keine gültige Messung durchführen
					break;
		case 0x60:	printf(" CRC-Fehler in den Flash-Daten "); // CRC-Fehler in den Flash-Daten
					break;
		case 0x61:	printf(" Fehler beim Schreiben ins Flash "); // Fehler beim Schreiben ins Flash; z.B. Speicherstelle nicht gelöscht
					break;
		case 0x62:	printf(" Flash enthält ungültige Float-Werte "); // Flash enthält ungültige Float-Werte
					break;
		case 0xFF:	printf(" unbekannter Fehler "); // unbekannter Fehler
					break;
		default:	printf(" unbekannter Code "); // unbekannter Fehlercode
					break;
	}
}

// Berechnung der Checksumme
unsigned short crc_sum(unsigned char array[SerialArray],int count) // DatenArray von SOH bis ETX.
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

// Sende Abfrage Versionsnummer
int encode(unsigned char arrayTX[SerialArray],int command,struct devdaten *daten,struct kanal *channels,int opt1,int opt2[])
{
    unsigned short crcresult;	// CRC-Berechnung
    int runstop = 0;
    int count;
    int i,j;

    switch(command)
    {
        case 1:			// Version
                                count = 14;
                                arrayTX[6] = 0x02; // Lauflänge
                                arrayTX[8] = 0x20; // Kommando
                                arrayTX[9] = 0x10; // Parameter
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
                                count = 15;
                                arrayTX[6] = 0x03;
                                arrayTX[8] = 0x2F; // Kommando
                                arrayTX[9] = 0x10; // Parameter

                                for(i=0,j=0;(i < 20);i++)
                                	if(opt2[i] != 0)
                                		j++;
                                arrayTX[10] = j; // Anzahl

                                for(i=0;i<j;i++)
                                {
                                	arrayTX[11+(i*2)] = opt2[i] & 0x00ff; // Kanal low
                                	arrayTX[12+(i*2)] = (opt2[i] >> 8) & 0x00ff; // Kanal high
                                }
                                arrayTX[6] = arrayTX[6] + (i*2);
                                count = count + (i*2);
                                break;

        case 5:			// Reset
                                count = 15;
                                arrayTX[6] = 0x03;
                                arrayTX[8] = 0x25; // Kommando
                                arrayTX[9] = 0x10; // Parameter
                                arrayTX[10] = 0x10; // Softwarereset
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
        arrayTX[2] = (daten->StationAdr & 0x00FF); // Low-Adresse Station
        arrayTX[3] = (daten->StationAdr >> 8) & 0x00FF; // High-Adresse Station
        arrayTX[4] = (daten->PCAdr & 0x00FF); // Low-Adresse Master, PC
        arrayTX[5] = (daten->PCAdr >> 8) & 0x00FF; // High-Adresse Master, PC
        arrayTX[7] = 0x02; // STX - fix
        arrayTX[8 + arrayTX[6]]= 0x03; // ETX - fix
        arrayTX[11 + arrayTX[6]]= 0x04; // EOT - fix

        // CRC-Summe
        crcresult = crc_sum(arrayTX,count);
        arrayTX[9 + arrayTX[6]]= crcresult & 0x00FF;
        arrayTX[10 + arrayTX[6]]= ((crcresult = crcresult >> 8) & 0x00FF);
    }
    
    return(count);
}

int LufftAddr(unsigned char klasse,unsigned char addresse)
{
    int mainaddr;
    
    mainaddr = (klasse & 0x00FF) << 8;
    mainaddr += (addresse & 0x00FF);
    
    return(mainaddr);
}

int decode(unsigned char arrayRX[SerialArray],int count,struct devdaten *daten,struct kanal *channels,struct livedata *aktdata)
{
    int i,j;
    struct kanal *ptr;
    int chnummer;

    if((arrayRX[0] == 0x01) & (arrayRX[7] == 0x02) & (arrayRX[8 + arrayRX[6]] == 0x03) & (arrayRX[11 + arrayRX[6]] == 0x04)) //SOH + STX + ETX + EOT
    {
        if(arrayRX[1] == 0x10) // Protokoll-Version 1.0
        {
            if((LufftAddr(arrayRX[5],arrayRX[4]) == daten->StationAdr) & (LufftAddr(arrayRX[3],arrayRX[2]) == daten->PCAdr))
            {
                switch(arrayRX[8]) // Befehl
                {
                    case 0x20:		// Hard- und Software
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(aktdata->Befehl,0x0,Befehllaenge);
                                            strncpy(aktdata->Befehl,"Version",7);
                                            aktdata->Cmdnr = 1;
                                            
                                            daten->aktHardware = arrayRX[11];
                                            daten->aktSoftware = arrayRX[12];
                                            if(DEBUG > 1)
                                                printf("Debug: %s Hardware: %.1f Software: %.1f\n",aktdata->Befehl,(float)daten->aktHardware/10,(float)daten->aktSoftware/10);
                                        }
                                        else
                                            printf("Error 103: Falsche Befehlsversion\n");
                                        break;

                    case 0x2D:		// Geräteinformationen
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(aktdata->Befehl,0x0,Befehllaenge);
                                            strncpy(aktdata->Befehl,"Geraeteinfo",11);
                                            aktdata->Cmdnr = 2;
                                            
                                            if(arrayRX[11] == 0x10)
                                                {
                                                    memset(daten->Geraetebezeichnung,0x0,Befehllaenge);
                                                    for(i=0;(i < arrayRX[6]-4) & (i < Befehllaenge-1);i++)
                                                        daten->Geraetebezeichnung[i] = arrayRX[12 + i];
                                                    if(DEBUG > 1)
                                                        printf("Debug: %s Gerätebezeichnung: %s\n",aktdata->Befehl,daten->Geraetebezeichnung);

                                                }
                                            else if(arrayRX[11] == 0x11)
                                                {
                                                    memset(daten->Geraetebeschreibung,0x0,Befehllaenge);
                                                    for(i=0;(i < arrayRX[6]-4) & (i < Befehllaenge-1);i++)
                                                        daten->Geraetebeschreibung[i] = arrayRX[12 + i];
                                                    if(DEBUG > 1)
                                                        printf("Debug: %s Gerätebeschreibung: %s\n",aktdata->Befehl,daten->Geraetebeschreibung);
                                                }
                                            else if(arrayRX[11] == 0x12)
                                                {
                                                    daten->aktHardware = arrayRX[12];
                                                    daten->aktSoftware = arrayRX[13];
                                                    if(DEBUG > 1)
                                                        printf("Debug: %s Hardware: %.1f Software: %.1f\n",aktdata->Befehl,(float)daten->aktHardware/10,(float)daten->aktSoftware/10);
                                                }
                                            else if(arrayRX[11] == 0x13)
                                                {
                                                    daten->Devinfo.lfddevnr = (arrayRX[13] * 256) + arrayRX[12];
                                                    daten->Devinfo.proddate = (arrayRX[15] * 256) + arrayRX[14];
                                                    daten->Devinfo.project = (arrayRX[17] * 256) + arrayRX[16];
                                                    daten->Devinfo.stueli = arrayRX[18];
                                                    daten->Devinfo.splan = arrayRX[19];
                                                    daten->Devinfo.hardware = arrayRX[20];
                                                    daten->Devinfo.software = arrayRX[21];
                                                    daten->Devinfo.e2version = arrayRX[22];
                                                    daten->Devinfo.deviceversion = (arrayRX[24] * 256) + arrayRX[23];
                                                    
                                                    memset(daten->serialnummer,0x0,Befehllaenge);
                                                    sprintf(daten->serialnummer,"%03d.%04d.%04d.%03d",daten->Devinfo.lfddevnr,daten->Devinfo.proddate,daten->Devinfo.project,daten->Devinfo.deviceversion);

                                                    if(DEBUG > 1)
                                                    {
                                                        printf("Debug: %s Erweiterte Projektdaten: %d %04d %d %d %d %d %d %d %d\n",aktdata->Befehl,daten->Devinfo.lfddevnr,daten->Devinfo.proddate,daten->Devinfo.project,daten->Devinfo.stueli, daten->Devinfo.splan,daten->Devinfo.hardware,daten->Devinfo.software,daten->Devinfo.e2version,daten->Devinfo.deviceversion);
                                                        printf("Debug: %s Seriennummer:%s\n",aktdata->Befehl,daten->serialnummer);
                                                    }
                                                }
                                            else if(arrayRX[11] == 0x14)
                                                {
                                                    daten->EEPROMSize = (arrayRX[13] * 256) + arrayRX[12];
                                                    if(DEBUG > 1)
                                                        printf("Debug: %s EEPROM: %d Byte\n",aktdata->Befehl,daten->EEPROMSize);
                                                }
                                            else if(arrayRX[11] == 0x15)
                                                {
                                                    daten->AnzahlKanaele = (arrayRX[13] * 256) + arrayRX[12];
                                                    daten->AnzahlBloecke = arrayRX[14];
                                                    if(DEBUG > 1)
                                                        printf("Debug: %s %d Kanäle in %d Blöcken\n",aktdata->Befehl,daten->AnzahlKanaele,daten->AnzahlBloecke);
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
                                                            aktdata->aktcntchannels = 1;
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
                                                        ptr->lfdnr = aktdata->aktcntchannels;
                                                        if(DEBUG > 1)
                                                            printf("Debug: %s %d Block: %d Anzahl: %d Kanal: %d\n",aktdata->Befehl,i,ptr->block,ptr->maxnummer,ptr->nummer);
                                                        i++;
                                                        aktdata->aktcntchannels++;
                                                    }while(i < ptr->maxnummer);
                                                }
                                            else if(arrayRX[11] == 0x30)
                                                {
                                                    ptr = channels;
                                                    chnummer = (arrayRX[13] * 256) + arrayRX[12];
                                                    while(ptr->nummer != chnummer)
                                                        ptr = ptr->next;
                                                    memset(ptr->groesse,0x0,21);
                                                    for(i=0;(i < arrayRX[6]) & (i < 20);i++)
                                                        ptr->groesse[i] = arrayRX[14 + i];
                                                    memset(ptr->einheit,0x0,16);
                                                    for(i=0;(i < arrayRX[6]) & (i < 15);i++)
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
                                                    
                                                    if(DEBUG > 1)
                                                    {
                                                        printf("Debug: %s %d %s %s ",aktdata->Befehl,chnummer,ptr->groesse,ptr->einheit);
                                                        printMWtyp(ptr->mwtyp);
                                                            
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
                                                memset(aktdata->Fehlermeldung,0x0,Befehllaenge);
                                                strncpy(aktdata->Fehlermeldung,"Keine Daten lesbar!",19);
                                                if(DEBUG > 1)
                                                    printf("Debug: %s %s\n",aktdata->Befehl,aktdata->Fehlermeldung);
                                            }
                                        }
                                        else
                                            printf("Error 110: Falsche Befehlsversion\n");
                                        break;

                    case 0x23:		// Onlinedaten Einzelabfrage
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(aktdata->Befehl,0x0,Befehllaenge);
                                            strncpy(aktdata->Befehl,"OnlinedatenE",12);
                                            aktdata->Cmdnr = 3;

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
                                                        ptr->value.z[0] = arrayRX[14];
                                                    }
                                                    else if(ptr->datetyp == 0x12 || ptr->datetyp == 0x13)
                                                    {
                                                        ptr->value.z[0] = arrayRX[14];
                                                        ptr->value.z[1] = arrayRX[15];
                                                    }
                                                    else if(ptr->datetyp == 0x14 || ptr->datetyp == 0x15 || ptr->datetyp == 0x16)
                                                    {
                                                        ptr->value.z[0] = arrayRX[14];
                                                        ptr->value.z[1] = arrayRX[15];
                                                        ptr->value.z[2] = arrayRX[16];
                                                        ptr->value.z[3] = arrayRX[17];
                                                    }
                                                    else if(ptr->datetyp == 0x17)
                                                    {
                                                        ptr->value.z[0] = arrayRX[14];
                                                        ptr->value.z[1] = arrayRX[15];
                                                        ptr->value.z[2] = arrayRX[16];
                                                        ptr->value.z[3] = arrayRX[17];
                                                        ptr->value.z[4] = arrayRX[18];
                                                        ptr->value.z[5] = arrayRX[19];
                                                        ptr->value.z[6] = arrayRX[20];
                                                        ptr->value.z[7] = arrayRX[21];
                                                    }
                                                    else
                                                    {
                                                        ptr->value.z[0] = 0x00;
                                                        ptr->value.z[1] = 0x00;
                                                        ptr->value.z[2] = 0x00;
                                                        ptr->value.z[3] = 0x00;
                                                        ptr->value.z[4] = 0x00;
                                                        ptr->value.z[5] = 0x00;
                                                        ptr->value.z[6] = 0x00;
                                                        ptr->value.z[7] = 0x00;
                                                    }
                                                }
                                                else
                                                    printf("Error 112: Falscher Datentyp\n");
                                            }
                                            
                                            if(DEBUG > 1)
                                            {
                                                printf("Debug: %s %d ",aktdata->Befehl,chnummer);
                                                if(daten->channels != NULL)
                                                {
                                                    printf("%s ",ptr->groesse);
                                                    printMWtyp(ptr->mwtyp);
                                                }
                                                printValueByDatatyp(ptr->datetyp,ptr->value);

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
                                            memset(aktdata->Befehl,0x0,Befehllaenge);
                                            strncpy(aktdata->Befehl,"OnlinedatenM",12);
                                            aktdata->Cmdnr = 4;

                                            if(daten->channels != NULL)
                                            {
                                                ptr = daten->channels;
                                                while(ptr->next != NULL)
                                                    ptr = ptr->next;
                                            }
                                            else
                                            {	// Temporärer Kanal
                                                ptr = channels;
                                            }

                                            for(i=0,j=12;i < arrayRX[11];i++)
                                            {
                                            	/*
                                            	arrayRX[j];   // 12 -länge Substring
                                            	arrayRX[j+1]; // 13 -Status Substring
                                            	arrayRX[j+2]; // 14 -Kanal Low
												arrayRX[j+3]; // 15 -Kanal High
												arrayRX[j+4]; // 16 -Datentyp

                                            	arrayRX[j+5]; // 17 -Daten bis zu 8*
                                            	arrayRX[j+6]; // 18
                                            	arrayRX[j+7]; // 19
                                            	arrayRX[j+8]; // 20
                                            	arrayRX[j+9]; // 21
                                            	arrayRX[j+10]; // 22
                                            	arrayRX[j+11]; // 23
                                            	arrayRX[j+12]; // 24
												*/
                                            	chnummer = (arrayRX[j+3]*256) + arrayRX[j+2];

                                                ptr->datetyp = arrayRX[j+4];
                                                ptr->nummer = chnummer;
                                                ptr->next = NULL;
                                                ptr->lfdnr = i+1;
                                                ptr->block = 0;
                                                ptr->maxnummer = arrayRX[11];
                                                memset(ptr->groesse,0x0,21);
                                                strncpy(ptr->groesse,"TempValue",9);
                                                memset(ptr->einheit,0x0,16);
                                                strncpy(ptr->einheit,"TempValue",9);
                                                ptr->mwtyp = 0;
                                                ptr->Min.a = 0;
                                                ptr->Max.a = 0;

                                                if((arrayRX[10] == 0x00) & (arrayRX[j+1] == 0x00) & (chnummer == ptr->nummer))
                                                {
                                                    if(arrayRX[j+4] == ptr->datetyp)
                                                    {
                                                        if(ptr->datetyp == 0x10 || ptr->datetyp == 0x11)
                                                        {
                                                            ptr->value.z[0] = arrayRX[j+5];
                                                        }
                                                        else if(ptr->datetyp == 0x12 || ptr->datetyp == 0x13)
                                                        {
                                                            ptr->value.z[0] = arrayRX[j+5];
                                                            ptr->value.z[1] = arrayRX[j+6];
                                                        }
                                                        else if(ptr->datetyp == 0x14 || ptr->datetyp == 0x15 || ptr->datetyp == 0x16)
                                                        {
                                                            ptr->value.z[0] = arrayRX[j+5];
                                                            ptr->value.z[1] = arrayRX[j+6];
                                                            ptr->value.z[2] = arrayRX[j+7];
                                                            ptr->value.z[3] = arrayRX[j+8];
                                                        }
                                                        else if(ptr->datetyp == 0x17)
                                                        {
                                                            ptr->value.z[0] = arrayRX[j+5];
                                                            ptr->value.z[1] = arrayRX[j+6];
                                                            ptr->value.z[2] = arrayRX[j+7];
                                                            ptr->value.z[3] = arrayRX[j+8];
                                                            ptr->value.z[4] = arrayRX[j+9];
                                                            ptr->value.z[5] = arrayRX[j+10];
                                                            ptr->value.z[6] = arrayRX[j+11];
                                                            ptr->value.z[7] = arrayRX[j+12];
                                                        }
                                                        else
                                                        {
                                                            ptr->value.z[0] = 0x00;
                                                            ptr->value.z[1] = 0x00;
                                                            ptr->value.z[2] = 0x00;
                                                            ptr->value.z[3] = 0x00;
                                                            ptr->value.z[4] = 0x00;
                                                            ptr->value.z[5] = 0x00;
                                                            ptr->value.z[6] = 0x00;
                                                            ptr->value.z[7] = 0x00;
                                                        }
                                                    }
                                                    else
                                                        printf("Error 112: Falscher Datentyp\n");
                                                }

                                                if(DEBUG > 1)
                                                {
                                                    printf("Debug: %s %d ",aktdata->Befehl,chnummer);
                                                    if(daten->channels != NULL)
                                                    {
                                                        printf("%s ",ptr->groesse);
                                                        printMWtyp(ptr->mwtyp);
                                                    }

													printValueByDatatyp(ptr->datetyp,ptr->value);

													if(daten->channels != NULL)
                                                        printf(" %s",ptr->einheit);
                                                            printf("\n");
                                                }

                                            	j = j+ arrayRX[j];
                                            	ptr->next = malloc(sizeof(struct kanal));
                                            	ptr = ptr->next;
                                            	ptr->next=NULL;
                                                ptr->nummer = 0;
                                            }
                                        }
                                        else
                                            printf("Error 111: Falsche Befehlsversion\n");
                                        break;

                    case 0x25:		// Reset / Default
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(aktdata->Befehl,0x0,Befehllaenge);
                                            strncpy(aktdata->Befehl,"Reset",5);
                                            aktdata->Cmdnr = 5;

                                            if(arrayRX[10] == 0x00)
                                                if(DEBUG > 1)
                                                    printf("Reset\n");
                                        }
                                        else
                                            printf("Error 105: Falsche Befehlsversion\n");
                                        break;

                    case 0x26:		// Statusabfrage
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(aktdata->Befehl,0x0,Befehllaenge);
                                            strncpy(aktdata->Befehl,"Status",6);
                                            aktdata->Cmdnr = 6;

                                            if(arrayRX[10] == 0x00)
                                            {
                                            	aktdata->lastStatus = arrayRX[11];
                                                if(DEBUG > 1)
                                                    printf("Status: %d\n",aktdata->lastStatus);
                                            }
                                        }
                                        else
                                            printf("Error 106: Falsche Befehlsversion\n");
                                        break;

                    case 0x2C:		// letzte Fehlermeldung
                                        if(arrayRX[9] == 0x10)
                                        {
                                            memset(aktdata->Befehl,0x0,Befehllaenge);
                                            strncpy(aktdata->Befehl,"Fehlermeldung",13);
                                            aktdata->Cmdnr = 7;

                                            if(arrayRX[10] == 0x00)
                                            {
                                            	aktdata->lastError = arrayRX[11];
                                                if(DEBUG > 1)
                                                    printf("Fehler: %d\n",aktdata->lastError);
                                            }
                                        }
                                        else
                                            printf("Error 109: Falsche Befehlsversion\n");
                                        break;

                    default:		printf("Error 102: Unbekannter Befehl");
                                        memset(aktdata->Befehl,0x0,Befehllaenge);
                                        strncpy(aktdata->Befehl,"Error 102",9);
                                        aktdata->Cmdnr = 0;
                                        break;

                }
            }
            else
                if(DEBUG > 1)
                    printf("Debug: Falsche Adressierung.\n");
        }
        else
            printf("Error 101: Protokoll-Version nicht unterstützt.\n");
    }
    else
        printf("Error 100: Protokoll-Rahmen fehlerhaft.\n");

    return(0);
}

// Abfrage der Daten 
int request(int cmd,struct devdaten *station,struct kanal *channels,int i,int list[])
{
    int count;
    unsigned char arrayTX[SerialArray];
    unsigned char arrayRX[SerialArray];

    memset(arrayTX,0,SerialArray);
    memset(arrayRX,0,SerialArray);
    
    count = encode(arrayTX,cmd,station,channels,i,list);
    if(count > 0)
    {
        if(DEBUG > 2)
            debugserial(arrayTX,count);

        if(SIMULATION > 0)
        {
        	count=sim(station->fdserial,arrayTX,arrayRX,count);
        	sleep(1);
        }
        else
        {
        	send(station->fdserial,arrayTX,count);
        sleep(1);
        	count = recv(station->fdserial,arrayRX);
        }
        if(DEBUG > 2)
            debugserial(arrayRX,count);

        decode(arrayRX,count,station,channels,&station->live);
    }
    usleep(100000);
    return(0);
}
