#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <unistd.h>

#include <sys/types.h> //setuid

int main(int argc, char *argv[])
{
	if (argc!=2)
	{
	    fprintf(stderr,"falsche Parameteranzahl\n");
	    exit(0);
	}

	int wert = atoi(argv[1]);


	setuid(0); //alle User koennen dieses Programm mit den Rechten des Root ausfuehren

	if (wert==0)
	{
		system("echo 4-2 > /sys/bus/usb/drivers/usb/unbind");
	}

	if (wert==1)
	{
		system("echo 4-2 > /sys/bus/usb/drivers/usb/bind");
	}


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
