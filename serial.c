#include "wetterstation.h"

// Initialisierung Serial-Port
int SerialPortInit()
{
    int fd;
    struct termios SerialPortSettings;

    if(SIMULATION == 0)
    {
    	// Seriale-Schnittstelle
    	fd=open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY |O_NONBLOCK);
    	if(fd == -1)
    	{
    		printf("\n  Error! in Opening Serial-Port\n");
    		return(-1);
    	}
    	else
    	{
    		printf("\n  Serial-Port Opened Successfully\n");

    		// Lesen der Portparameter
    		tcgetattr(fd, &SerialPortSettings);

    		// Setze Geschwindigkeit Senden/Empfangen 19200Bd
    		cfsetspeed(&SerialPortSettings,B19200);

    		/* Raw-Mode */
    		SerialPortSettings.c_cflag &= ~PARENB;                          // No Parity
    		SerialPortSettings.c_cflag &= ~CSTOPB;                          // 1 Stop bit
    		SerialPortSettings.c_cflag |= CS8;                              // data bits = 8
    		SerialPortSettings.c_cflag &= ~CRTSCTS;                         // No Hardware flow Control
    		SerialPortSettings.c_cflag |= CREAD | CLOCAL;                   // Enable receiver,Ignore Modem Control lines
    		SerialPortSettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Non Cannonical mode / ECHO / Interpret Siganls
    		SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          // Disable XON/XOFF flow control
    		SerialPortSettings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Ignore Spezial-Charaters
    		SerialPortSettings.c_oflag &= ~(OPOST | ONLCR);                 // No Output Processing
    		SerialPortSettings.c_cc[VMIN] = 0;
    		SerialPortSettings.c_cc[VTIME] = 10;

    		// Schreiben der Parameter
    		if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0)
    		{
    			printf("\n  ERROR ! Serial-Parameter\n\n");
    			close(fd); // Zeiger zurückgeben
    			return(-1);
    		}
    		else
    		{
    			printf("\n  19200 Bd - 8n1\n\n");
    			tcflush(fd, TCIFLUSH);  // Löschen von Müll im Speicher
    		}
    	}
    }
    else
    	fd=-2207;
    return(fd);
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
