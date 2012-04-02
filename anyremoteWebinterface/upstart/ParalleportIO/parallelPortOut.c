#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <unistd.h>

#include <sys/types.h> //setuid

#define port 0x378

int main(int argc, char *argv[])
{
	setuid(0); //alle User koennen dieses Programm mit den Rechten des Root ausfuehren
  
	if(argc!=2)
	{
	    fprintf(stderr,"falsche Parameteranzahl\n");
	    exit(0);
	}

	int wert = atoi(argv[1]);

	if(!(wert>=0 && wert<=255))
	{
	    fprintf(stderr,"falscher Parameterwert\n");
	    exit(0);
	}

	if (ioperm(port,3,1)!=0)
	{
	    fprintf(stderr,"kein Zugriff. Muss als root laufen !\n");
	    exit(1);
	}

	outb(wert,port);

	ioperm(port,3,0);


	/*printf(
	        "Real     UID = %d\n"
	        "Effektiv UID = %d\n"
	        "Real     GID = %d\n"
	        "Effectiv GID = %d\n",
	        getuid (),
	        geteuid(),
	        getgid (),
	        getegid() );*/

	return 0;
}
