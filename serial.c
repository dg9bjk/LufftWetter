#include "wetterstation.h"

// Initialisierung Serial-Port
int SerialPortInit(char *serialport)
{
    int fd;			// File-Zeiger
    struct termios SerialPortSettings;	// Konfigurationsstruktur zum

    if(SIMULATION == 0)	// Nur die reale Struktur verändern, wenn keine Simulation erfolgt.
    {
    	// Seriale-Schnittstelle
    	fd=open(serialport,O_RDWR | O_NOCTTY | O_NDELAY |O_NONBLOCK);	// File-Zeiger öffnen
    	if(fd == -1)													// Fehler bei Öffnen des File-Zeiger
    	{
    		printf(" Error! in Opening Serial-Port: %s\n",serialport);
    		return(-1);
    	}
    	else															// File-Zeiger ist vorhanden und ist lesbar
    	{
    		printf(" Serial-Port Opened Successfully: %s\n",serialport);

    		tcgetattr(fd, &SerialPortSettings);							// Lesen der Portparameter

    		cfsetspeed(&SerialPortSettings,B19200);						// Setze Geschwindigkeit Senden/Empfangen 19200Bd
    		// Kommunikationsparameter
    		SerialPortSettings.c_cflag &= ~PARENB;                          // Kein Paritybit
    		SerialPortSettings.c_cflag &= ~CSTOPB;                          // 1 Stop bit
    		SerialPortSettings.c_cflag |= CS8;                              // data bits = 8
    		SerialPortSettings.c_cflag &= ~CRTSCTS;                         // Keine RTS/CTS DTR/DSR
    		SerialPortSettings.c_cflag |= CREAD | CLOCAL;                   // Aktiviere Empfänger,Ignoriere DCD / RI Eingänge
    		SerialPortSettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Kein Zeilenmode / ECHO aus/ Keine Unterbrechungsignale auswerten
    		SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          // Kein XON/XOFF-Control
    		SerialPortSettings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Ignoriere Spezial-Zeichen
    		SerialPortSettings.c_oflag &= ~(OPOST | ONLCR);                 // Keine manipulation des Ausganges
    		SerialPortSettings.c_cc[VMIN] = 0;								// Kein warten auf minimale Zeichenzahl
    		SerialPortSettings.c_cc[VTIME] = 10;							// Wartezeit n * 100ms;

    		if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0)		// Schreiben der Parameter
    		{
    			printf(" ERROR ! Serial-Parameter\n");					// Fehler beim Schreiben der Parameter
    			close(fd); 												// Zeiger auflösen
    			fd = -1;
    			return(-1);
    		}
    		else
    		{
    			printf(" 19200 Bd - 8n1\n");
    			tcflush(fd, TCIFLUSH);									// Löschen von Müll im Speicher
    		}
    	}
    }
    else
    	fd=-2207;														// File-Zeiger-ID im Simulationsmode
    return(fd);
}

//Suche nach den serialen Schnittstellen mit Wetterstation
int searchport(struct master *globalkonfig)
{
	int i;
	int result = 0;
    struct serport *serptr;			// Liste der Serialen Schnittstellen
    struct serport *testserport;
    int fdserial = 0;


    static char serialport[10][14] = {
		    		{"/dev/ttyS0"},
				{"/dev/ttyS1"},
				{"/dev/ttyS2"},
				{"/dev/ttyS3"},
				{"/dev/ttyUSB0"},
				{"/dev/ttyUSB1"},
				{"/dev/ttyUSB2"},
				{"/dev/ttyUSB3"},
				{"/dev/ttyACM0"},
				{"/dev/ttyAMA0"}
				};

	    // Vorschläge der Serialen Schnittstellen
	    serptr = malloc(sizeof(struct serport));
	    serptr->next = NULL;
	    globalkonfig->serial = NULL;

	    for(i=0;i < 10;i++)
	    {
	    	if((fdserial = SerialPortInit((char*)&serialport[i])) > 0)
	    	{
				serptr->fdserial   = fdserial;
				serptr->serialport = (char*)&serialport[i];
	    		if((getStationList(globalkonfig,*serptr,WSxTyp,STARTSCANSTATION,STOPSCANSTATION)) > 0)
	    		{
				if(globalkonfig->serial == NULL)
				{
					globalkonfig->serial = serptr;
					serptr = malloc(sizeof(struct serport));
					serptr->next = NULL;
				}
				else
				{
					testserport=globalkonfig->serial;
					while(testserport == NULL)
					{
						if(testserport->next == NULL)
						{
							testserport->next = serptr;
							serptr = malloc(sizeof(struct serport));
							serptr->next = NULL;
						}
						else
						{
							testserport = testserport->next;
						}
					}
				}
	    		}
	    		else
    			close(fdserial);
	    	}
	    }
	    free(serptr);
	    return(result);
}

// Schreibe Commando
int send(int fdserial,unsigned char array[SerialArray],int count)
{
    int writecnt;
    writecnt=write(fdserial,array,count);
    return(writecnt);    
}

// Empfange Daten
int recv(int fdserial,unsigned char array[SerialArray])
{
    int readcnt; // Länge der Rückgabe
    readcnt = read(fdserial,array,SerialArray);
    return(readcnt);    
}
