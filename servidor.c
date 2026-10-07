#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

#define longnombre 50

static void handler(int signum)
{
	int wstate;
	int pid = wait(&wstate);
}


int main(int argc, char ** argv)
{
	char cadena[longnombre], cadena2[longnombre], cadena3[longnombre];
	int fde, fds, leidos, pid, fdProxy;
	struct sigaction sa;
	
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	if(argc != 2)
	{
		printf("Servidor: Formato: servidor <nombre_fifo>\n");
		exit(-1);
	}
	
	// Pongo como nombre de fifo <nombre_fifo>s y <nombre_fifo>e
	sprintf(cadena, "%se", argv[1]);
	sprintf(cadena2, "%ss", argv[1]);
	
	// Creo los fifos correspondientes
	mkfifo(cadena, 0660);
	mkfifo(cadena2, 0660);
	
	// Si hubo error
	if ( (fde = open(cadena, O_RDWR)) < 0)
	{
		printf("Error al abrir el fifo %s\n", cadena);
		exit(-1);
	}
	
	if( (fds = open(cadena2, O_RDWR)) < 0)
	{
		printf("Error al abrir el fifo %s\n", cadena2);
		exit(-1);
	}
	int fdBloq  = open("bloqueo", O_CREAT|O_RDWR, 0660);
	
	if(sigaction(SIGCHLD, &sa, NULL) < 0)
	{
		perror("Error al intentar establecer el manejador de señal para SIGCHLD\n");
		exit(-1);
	}
	
	// Cuando sigue habiendo petición de cliente
	while( (leidos = read(fde, &pid, sizeof(int))) != -1)
	{
		int retVal = fork();
		
		if(retVal == 0) // Hijo
		{
			int mipid = getpid();
			// Establecer nombre del fifo
			sprintf(cadena3, "fifo.%d", mipid);
			// Crear el fifo
			if (mkfifo(cadena3, 0660) < 0) 
			{
			    perror("Error al crear el FIFO privado");
			    exit(-1);
			}
			// Pasar el pid al cliente
			write(fds, &mipid, sizeof(int));
			// Abrir el fifo privado
			if ((fdProxy = open(cadena3, O_RDONLY)) < 0) 
			{ 
				perror("Error al abrir el FIFO privado"); 
				exit(-1); 
			}
			// dup2(STDIN_FILENO, fdProxy);
			close(STDIN_FILENO);
			dup(fdProxy);
			execl("./proxy", "proxy", NULL);
		}
	}
	close(fde);
	close(fds);
	unlink(cadena);
	unlink(cadena2);
	unlink("bloqueo");
	exit(0);
}
