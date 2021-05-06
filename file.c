#include "wetterstation.h"

int readKonfig(char *konfig,struct master *globalkonfig)
{
	int i;
	int fd;
	int filesize;
	int length;
	int aktpos,startpos,stoppos,stpos;
	char *inputline;
	struct stat attribut;

	struct segm
	{
		char digit[20];
		struct segm *next;
	};

	struct kanelparameter
	{
		char nummer[5];
		char anzname[50];
		char anzeinheit[15];
		char abtast[5];
		struct segm *anzdigit;
		struct kanalparameter *next;
	};

	struct parameter
	{
		char name[20];
		char wert[20];
		struct parameter *next;
	};

	struct sector
	{
		char name[20];
		char nummer[5];
		struct parameter *param;
		struct kanalparameter *kanal;
		struct sector *next;
	};


	fd = open(konfig,O_RDONLY);
	if(fd > 0)
	{
		printf("File: %s\n",konfig);

		stat(konfig, &attribut);			// Laden der Attribute
		filesize = attribut.st_size + 2;	// Zwei Byte zusätzlich, wegen NULL-Zeichen;
		inputline = malloc(filesize);
		memset(inputline,0x0,filesize);

		length = read(fd,inputline,filesize);		// Auslesen der Datei
		if(DEBUG > 1)
			printf("Line: %s",inputline);

		for(i=0;i < length;i++)
		{

		}

		free(inputline);
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

