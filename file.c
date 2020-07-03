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
