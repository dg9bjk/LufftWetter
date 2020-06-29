#include "wetterstation.h"

// Simiulation der Datenübertragung

// Berechnung der Checksumme für Simulation -> Achtung ! Kleine Fehlersimulation wird noch integriert.
unsigned short crc_sum_sim(unsigned char array[SerialArray],int count) // DatenArray von SOH bis ETX.
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

// WS800 Simulation
int sim(int fdserial,unsigned char arrayTX[SerialArray],unsigned char arrayRX[SerialArray],int count)
{
	int i,j;
	int offset;
	int dp;
	char geraeteinfo[]         = "Sim WSx-UMB";
	char geraetebeschreibung[] = "Sim Compact Weatherstation WS-800";
	char messgroesse[]         = "Sim Kanal";
	char messtyp[]             = "Sim Einheit";

    unsigned short result;	// CRC-Berechnung
    unsigned short result1;
    unsigned short result2;

    int Ecount = 0;			// Lauflängenkennung für Encoder

	if(fdserial == -2207)
	{
		if((arrayTX[0] == 0x01) & (arrayTX[7] == 0x02) & (arrayTX[8 + arrayTX[6]] == 0x03) & (arrayTX[11 + arrayTX[6]] == 0x04)) //SOH + STX + ETX + EOT)
		{
			if(arrayTX[1] == 0x10) // Protokoll-Version 1.0
			{
				// CRC-Summe
		        result = crc_sum_sim(arrayTX,count);
		        result1 = result & 0x00FF;
		        result2 = (result >> 8) & 0x00FF;

		        if((arrayTX[9 + arrayTX[6]] == result1) & (arrayTX[10 + arrayTX[6]] == result2))
		        {
					arrayRX[0] = 0x01;	//SOH - fix
			        arrayRX[1] = 0x10;	// Version - fix
			        arrayRX[7] = 0x02;	// STX - fix
			        arrayRX[2] = arrayTX[4]; // Low-Adresse Station
			        arrayRX[3] = arrayTX[5]; // High-Adresse Station
			        arrayRX[4] = arrayTX[2]; // Low-Adresse Master, PC
			        arrayRX[5] = arrayTX[3]; // High-Adresse Master, PC

					switch (arrayTX[8])
					{
						case 0x20:		printf("Sim: Version");
										Ecount = 17;
		                                arrayRX[6] = 0x05;
										arrayRX[8] = 0x20;
										arrayRX[9] = 0x10;
										arrayRX[10]= 0x00;
										arrayRX[11]= 10;
										arrayRX[12]= 11;
										break;

						case 0x2D:		printf("Sim: Geräteinformation\n");
										arrayRX[8] = 0x2D;
										arrayRX[9] = 0x10;
										if(arrayTX[10]== 0x10) // Geräteinfo - Kurzform
										{
											Ecount = 16;
			                                arrayRX[6] = 0x04;
											arrayRX[10] = 0x00;
											arrayRX[11] = 0x10;
											for(i=0,j=12;i < strlen(geraeteinfo);i++,j++)
												arrayRX[j] = geraeteinfo[i];
											Ecount     = Ecount + strlen(geraeteinfo);
											arrayRX[6] = arrayRX[6] + strlen(geraeteinfo);

										}
										else if(arrayTX[10]== 0x11) // Gerätebeschreibung - ausgeschrieben
										{
											Ecount = 16;
			                                arrayRX[6] = 0x04;
											arrayRX[10] = 0x00;
											arrayRX[11] = 0x11;
											for(i=0,j=12;i < strlen(geraetebeschreibung);i++,j++)
												arrayRX[j] = geraetebeschreibung[i];
											Ecount     = Ecount + strlen(geraetebeschreibung);
											arrayRX[6] = arrayRX[6] + strlen(geraetebeschreibung);

										}
										else if(arrayTX[10]== 0x12) // Hardwareversion + Softwareversion
										{
											Ecount = 18;
			                                arrayRX[6] = 0x06;
											arrayRX[10] = 0x00;
											arrayRX[11] = 0x12;
											arrayRX[12]= 10;
											arrayRX[13]= 11;
										}
										else if(arrayTX[10]== 0x13) // Serialnummer - Erweiterte Daten
										{
											Ecount = 29;
			                                arrayRX[6] = 0x11;
											arrayRX[10] = 0x00;
											arrayRX[11] = 0x13;
											arrayRX[12]= 0x00;
											arrayRX[13]= 0x00;
											arrayRX[14]= 0x00;
											arrayRX[15]= 0x00;
											arrayRX[16]= 0x00;
											arrayRX[17]= 0x00;
											arrayRX[18]= 0x00;
											arrayRX[19]= 0x00;
											arrayRX[20]= 0x00;
											arrayRX[21]= 0x00;
											arrayRX[22]= 0x00;
											arrayRX[23]= 0x00;
											arrayRX[24]= 0x00;
										}
										else if(arrayTX[10]== 0x14) // EEPROM = 599 Byte
										{
											Ecount = 18;
			                                arrayRX[6] = 0x06;
											arrayRX[10] = 0x00;
											arrayRX[11] = 0x14;
											arrayRX[12] = 0x57;
											arrayRX[13] = 0x02;
										}
										else if(arrayTX[10]== 0x15) // Anzahl Messwerte = 122 auf 2 Blöcke
										{
											Ecount = 19;
			                                arrayRX[6] = 0x07;
											arrayRX[10] = 0x00;
											arrayRX[11] = 0x15;
											arrayRX[12]= 0x7a; // Anzahl Datenpunkte
											arrayRX[13]= 0x00;
											arrayRX[14]= 0x02; // Blöcke
										}
										else if(arrayTX[10]== 0x16) // Messpunkteliste
										{
											Ecount = 18;
			                                arrayRX[6] = 0x06;
											arrayRX[10] = 0x00;
											arrayRX[11] = 0x16;
											arrayRX[12] = arrayTX[11]; // Blocknummer
											if(arrayTX[11] == 0) // 1. Block 0-100
											{
												arrayRX[13] = 0x64;
												for(i=0,j=14;i<100;i++,j=j+2)
												{
													dp = (i*5)+100;
													arrayRX[j] = dp % 256;
													arrayRX[j+1] = dp / 256;
												}
												offset = i*2;
											}
											else if(arrayTX[11] == 1) // 2. Block 101-122
											{
												arrayRX[13] = 0x16;
												for(i=0,j=14;i<22;i++,j=j+2)
												{
													dp = ((i*5)+600);
													arrayRX[j] = dp % 256;
													arrayRX[j+1] = dp / 256;
												}
												offset = i*2;
											}
											Ecount     = Ecount + offset;
											arrayRX[6] = arrayRX[6] + offset;
										}
										else if(arrayTX[10] == 0x30)
										{
											Ecount = 63;
			                                arrayRX[6] = 0x33;
											arrayRX[10] = 0x00;
											arrayRX[11] = 0x30;
											arrayRX[12] = arrayTX[11]; // Kanalnummer
											arrayRX[13] = arrayTX[12]; // Kanalnummer

											for(i=0,j=14;i < strlen(messgroesse);i++,j++)
												arrayRX[j] = messgroesse[i];
											for(;i<20;i++,j++)
												arrayRX[j] = 0x00;

											for(i=0;i < strlen(messtyp);i++,j++)
												arrayRX[j] = messtyp[i];
											for(;i<15;i++,j++)
												arrayRX[j] = 0x00;

											arrayRX[j++] = 0x10;	// MW_Typ
											arrayRX[j++] = 0x16; // Datentyp

											arrayRX[j++] = 0x0; // Min
											arrayRX[j++] = 0x0;
											arrayRX[j++] = 0x7a;
											arrayRX[j++] = 0xc4;

											arrayRX[j++] = 0x0; // Max
											arrayRX[j++] = 0x0;
											arrayRX[j++] = 0x7a;
											arrayRX[j++] = 0x44;

										}
										else
											;
										break;

						case 0x23:		printf("Sim: Onlinedaten Einzel\n");
										Ecount = 22;
		                                arrayRX[6] = 0x0a;
										arrayRX[8] = 0x23;
										arrayRX[9] = 0x10;
										arrayRX[10]= 0x00;
										arrayRX[11]= arrayTX[10];
										arrayRX[12]= arrayTX[11];
										arrayRX[13]= 0x16; // Datentyp
										arrayRX[14]= 0x00; // Messwert 4*
										arrayRX[15]= 0x00;
										arrayRX[16]= 0xb8;
										arrayRX[17]= 0x41;
										break;

						case 0x2F:		printf("Sim: Onlinedaten Multi\n");
										Ecount = 16;
		                                arrayRX[6] = 0x05;
										arrayRX[8] = 0x2F;
										arrayRX[9] = 0x10;
										break;

						case 0x25:		printf("Sim: Reset\n");
										Ecount = 15;
		                                arrayRX[6] = 0x03;
										arrayRX[8] = 0x25;
										arrayRX[9] = 0x10;
										arrayRX[10]= 0x00;
										break;

						case 0x26:		printf("Sim: Statusabfrage\n");
										Ecount = 16;
		                                arrayRX[6] = 0x04;
										arrayRX[8] = 0x26;
										arrayRX[9] = 0x10;
										arrayRX[10]= 0x00;
										arrayRX[11]= 0x00; // Statuscode
										break;

						case 0x2C:		printf("Sim: Fehlerabfrage\n");
										Ecount = 16;
		                                arrayRX[6] = 0x04;
										arrayRX[8] = 0x2C;
										arrayRX[9] = 0x10;
										arrayRX[10]= 0x00;
										arrayRX[11]= 0x00; // Fehlercode
										break;
					};

					arrayRX[8 + arrayRX[6]]= 0x03; // ETX - fix
			        arrayRX[11 + arrayRX[6]]= 0x04; // EOT - fix
			        //CRC-Checksumme
			        result = crc_sum_sim(arrayRX,count);
			        arrayRX[9 + arrayRX[6]]= result & 0x00FF;
			        arrayRX[10 + arrayRX[6]]= ((result = result >> 8) & 0x00FF);

		        }
		        else
		        	printf("CRC-Checksumme_Simulation fehler\n");

			}
		}
	}
	else
		return(-1);

	return(Ecount);
}
