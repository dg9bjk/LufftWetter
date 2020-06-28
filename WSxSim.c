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
										Ecount = 15;
		                                arrayRX[6] = 0x03;
										arrayRX[8] = 0x2D;
										arrayRX[9] = 0x10;
										break;

						case 0x23:		printf("Sim: Onlinedaten Einzel\n");
										Ecount = 16;
		                                arrayRX[6] = 0x04;
										arrayRX[8] = 0x23;
										arrayRX[9] = 0x10;
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
										break;

						case 0x26:		printf("Sim: Statusabfrage\n");
										Ecount = 14;
		                                arrayRX[6] = 0x02;
										arrayRX[8] = 0x26;
										arrayRX[9] = 0x10;
										break;

						case 0x2C:		printf("Sim: Fehlerabfrage\n");
										Ecount = 14;
		                                arrayRX[6] = 0x02;
										arrayRX[8] = 0x2C;
										arrayRX[9] = 0x10;
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
