//###################################################################################################################################
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define SerialArray        260    // Zeichenpuffer

// Initialisierung Serial-Port
int SerialPortInit()
{
        int fd;
        struct termios SerialPortSettings;

        // Seriale-Schnittstelle
        fd=open("/dev/lufftwetter",O_RDWR | O_NOCTTY | O_NDELAY);
        if(fd == -1)
        {
                printf("\n  Error! in Opening Serial-Port ");
                return(-1);
        }
        else
        {
                printf("\n  Serial-Port Opened Successfully ");

                // Lesen der Portparameter
                tcgetattr(fd, &SerialPortSettings);

                // Setze Geschwindigkeit Senden/Empfangen 19200Bd
                cfsetspeed(&SerialPortSettings,B19200);

                /* 8N1 Mode */
                SerialPortSettings.c_cflag &= ~PARENB;                          // Disables the Parity Enable bit(PARENB),So No Parity
                SerialPortSettings.c_cflag &= ~CSTOPB;                          // CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit
                SerialPortSettings.c_cflag &= ~CSIZE;                           // Clears the mask for setting the data size
                SerialPortSettings.c_cflag |=  CS8;                             // Set the data bits = 8
                SerialPortSettings.c_iflag |=  IGNPAR;
                SerialPortSettings.c_iflag |=  IGNBRK;
                SerialPortSettings.c_iflag |=  IGNCR;
                SerialPortSettings.c_cflag &= ~CRTSCTS;                         // No Hardware flow Control
                SerialPortSettings.c_cflag |= CREAD | CLOCAL;                   // Enable receiver,Ignore Modem Control lines
                SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          // Disable XON/XOFF flow control both i/p and o/p
                SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Non Cannonical mode
                SerialPortSettings.c_cflag &= ~OPOST;                           // No Output Processing
                SerialPortSettings.c_cc[VMIN] = 0;
                SerialPortSettings.c_cc[VTIME] = 10;

                // Schreiben der Parameter
                if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0)
                {
                    printf("\n  ERROR ! Serial-Parameter");
                    close(fd); // Zeiger zurückgeben
                    return(-1);
                }
                else
                {
                    printf("\n  19200 Bd - 8n1 \n");
                    tcflush(fd, TCIFLUSH);  // Löschen von Müll im Speicher
                }
        }
        return(fd);
}

// Schreibe Commando
int send(int fdserial,char array[SerialArray],int count)
{
    int serreturn;
    
    serreturn=write(fdserial,array,count);
    
    return(serreturn);    
}


// Empfange Daten
int recv(int fdserial,char array[SerialArray])
{
    int bytes_read_in; // Länge der Empfangskette
    int bytes_read_out; // Länge der Rückgabe
    int count;
    int count2;
    unsigned char puffer;
/*    
    bytes_read_in = 0;
    bytes_read_out = 0;
    count2 = 0;
    printf("RX: ");
    do{
        count = read(fdserial,&puffer,1);
        if(count > 0)
        {
            printf("%x - ",puffer);
            array[bytes_read_in] = puffer;
            bytes_read_in++;
            if(bytes_read_in > 16)
            {
                bytes_read_out = bytes_read_in;
            }
        }
        count2++;
    }while((bytes_read_out == 0) & (count2 < 1000000));
    printf("\n");
*/    
    bytes_read_out = read(fdserial,array,SerialArray);

    return(bytes_read_out);    
}
