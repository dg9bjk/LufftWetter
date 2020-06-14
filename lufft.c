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

// Sende Abfrage Versionsnummer
int encode(char arrayTX[SerialArray])
{
    int count = 14;
    
    unsigned short result;
    
    arrayTX[0] = 0x01; //SOH - fix
    arrayTX[1] = 0x10; // Version - fix
    arrayTX[2] = 0x01; // Low-Adresse Station
    arrayTX[3] = 0x70; // High-Adresse Station
    arrayTX[4] = 0x01; // Low-Adresse Master, PC
    arrayTX[5] = 0xF0; // High-Adresse Master, PC
    arrayTX[6] = 0x02; // Laenge des Komando [8] - [n]
    arrayTX[7] = 0x02; // STX - fix
    arrayTX[8] = 0x20; // Kommando
    arrayTX[9] = 0x10; // Parameter
    arrayTX[10]= 0x03; // ETX - fix
    arrayTX[11]= 0x00; // Checksumme Low
    arrayTX[12]= 0x00; // Checksumme High
    arrayTX[13]= 0x04; // EOT - fix
    arrayTX[14]= 0x00;
    
    result = crc_sum(arrayTX,count);
    arrayTX[11]= result & 0x00FF;
    arrayTX[12]= ((result = result >> 8) & 0x00FF);
    
    return(count);
}

int decode(char arrayRX[SerialArray],int count)
{
    ;
}
