#include "wetterstation.h"

int readKonfig(char *konfig,struct master *globalkonfig)
{
	int fd;
	char line[255];

	fd = open(konfig,O_RDONLY);
	if(fd > 0)
	{
		printf("File: %s\n",konfig);

		while(read(fd,&line,255))
		{
			printf("Line: %s",line);
		};

		close(fd);

		return(1);
	}
	else
		return(-1);
}

int searchkonfig(struct master *globalkonfig)
{
		int i;
		int result = 0;

		// Vorschläge der Konfigurationspfade - werden der Reihe nach durchprobiert. Der erste Treffer wird benutzt.
		static char konfigpath[9][30] = {
				{"./wetter.conf"},
				{"~/wetter.conf"},
				{"~/lufft/wetter.conf"},
				{"/etc/wetter.conf"},
				{"/etc/lufft/wetter.conf"},
				{"/usr/lufft/wetter.conf"},
				{"/var/lufft/wetter.conf"},
				{"/usr/share/lufft/wetter.conf"},
				{"/var/share/lufft/wetter.conf"}
				};

		globalkonfig->konfigpath    = NULL;						// Initialisierung auf NULL
		for(i=0;i < 9;i++)										// Suche das konfigpath-Array durch
			if((readKonfig(konfigpath[i],globalkonfig)) > 0)		// Wenn eine gültige Dateigefunden wird
			{
				globalkonfig->konfigpath    = (char*)&konfigpath[i];	// Wird diese gespeichert
				break;												// Der Suchlauf sofort bendet.
			}
		return(result);
}

