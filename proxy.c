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

#define tamano 1024

int main(void)
{
	char buffer[tamano];
	FILE * tmp;
	tmp = tmpfile();
	int leidos;
	// Crear cerrojo
	struct flock cerrojo;
	
	
	// Ir leyendo de 1024 en 1024
	while((leidos = read(STDIN_FILENO, buffer, 1024)) != 0) // Mientras haya caracter
		fwrite(buffer, sizeof(char), leidos, tmp);
		
	// Obtener cerrojo
	int fdBloq  = open("bloqueo", O_RDWR);
	
	// Configurar el cerrojo
	cerrojo.l_type = F_WRLCK;
	cerrojo.l_whence =SEEK_SET;
	cerrojo.l_start =0;
	cerrojo.l_len =0;
	
	// Configurar el cerrojo
	fcntl(fdBloq, F_SETLKW, &cerrojo);
	
	// Configurar el puntero del fichero de tmp para que lea desde el inicio
	fseek(tmp, 0L, SEEK_SET);
	
	while((leidos = fread(buffer, sizeof(char), 1024, tmp)) != 0)
		write(STDOUT_FILENO, buffer, leidos);

	// Liberar el cerrojo
	cerrojo.l_type = F_UNLCK;
	cerrojo.l_whence =SEEK_SET;
	cerrojo.l_start =0;
	cerrojo.l_len =0;
	fcntl(fdBloq, F_SETLKW, &cerrojo);
	
	sprintf(buffer, "fifo.%d", getpid());
	unlink(buffer);
	
	exit(0);
}
