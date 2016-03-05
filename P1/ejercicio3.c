#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h> //Control de errores
#include <string.h> //Para la funcion strerror()

#define MAX_SIZE 5

void funcionLog(char *mensaje);
FILE *fLog = NULL;

int main(){
	int fildes[2], i, flag, status;
	pid_t pid;
	char buf[MAX_SIZE], out[300];

	if (pipe(fildes)==-1){
		sprintf(out,"Error al crear la tubería");
		printf("%s\n",out);
		funcionLog(out);
		return 1;
	}

	switch(pid=fork()){
		case -1:
			sprintf(out,"Error en la creación del proceso hijo");
			printf("%s\n",out);
			funcionLog(out);
			return 1;

		case 0:
			sprintf(out,"[HIJO] mi PID es %d y mi PPID es %d",getpid(),getppid());
			printf("%s\n",out);
			funcionLog(out);
			if (close(fildes[1])==-1)
					perror("Error al cerrar la tubería");

			for(i=0;i<5;i++){
				if(read(fildes[0],buf,MAX_SIZE)==-1)
					perror("Error en la escritura en la tubería");
				sprintf(out,"[HIJO] leemos el número aleatorio %s de la tubería",buf);
				printf("%s\n",out);
				funcionLog(out);
			}

			if (close(fildes[0])==-1)
					perror("Error al cerrar la tubería");
			sprintf(out,"[HIJO] tubería cerrada. Salgo...");
			printf("%s\n",out);
			funcionLog(out);
			return 0;

		default:
			sprintf(out,"[PADRE] mi PID es %d y el PID de mi hijo es %d",getpid(),pid);
			printf("%s\n",out);
			funcionLog(out);
			srand(time(NULL));
			if (close(fildes[0])==-1)
				perror("Error al cerrar la tubería");

			for(i=0;i<5;i++){
				sprintf(buf,"%d",rand()%5000);
				sprintf(out,"[PADRE] escribimos el número aleatorio %s en la tubería",buf);
				printf("%s\n",out);
				funcionLog(out);
				if(write(fildes[1],buf,MAX_SIZE)==-1)
					perror("Error en la lectura de la tubería");
				usleep(rand()%1000000);
			}

			if (close(fildes[1])==-1)
					perror("Error al cerrar la tubería");
			sprintf(out,"[PADRE] tubería cerrada");
			printf("%s\n",out);
			funcionLog(out);
			break;
	}

	while ((flag = wait(&status)) > 0)
	{
		if (WIFEXITED(status))
		{
			sprintf(out,"Hijo PID:%d finalizado, estado=%d", flag, WEXITSTATUS(status));
			printf("%s\n",out);
			funcionLog(out);
		} else if (WIFSIGNALED(status)) {  //Para seniales como las de finalizar o matar
			sprintf(out,"Hijo  PID:%d finalizado al recibir la señal %d", flag, WTERMSIG(status));
			printf("%s\n",out);
			funcionLog(out);
		} else if (WIFSTOPPED(status)) { //Para cuando se para un proceso. Al usar wait() en vez de waitpid() no nos sirve.
			sprintf(out,"Hijo PID:%d parado al recibir la señal %d", flag,WSTOPSIG(status));
			printf("%s\n",out);
			funcionLog(out);
		} else if (WIFCONTINUED(status)){ //Para cuando se reanuda un proceso parado. Al usar wait() en vez de waitpid() no nos sirve.
			sprintf(out,"Hijo reanudado");
			printf("%s\n",out);
			funcionLog(out);
		}
	}
	if (flag==-1 && errno==ECHILD)
	{
		sprintf(out,"No hay más hijos que esperar");
		printf("%s\n",out);
		funcionLog(out);
		sprintf(out,"status de errno=%d, definido como %s", errno, strerror(errno));
		printf("%s\n",out);
		funcionLog(out);
	}
	else
	{
		sprintf(out,"Error en la invocacion de wait o la llamada ha sido interrumpida por una señal.");
		printf("%s\n",out);
		funcionLog(out);
		exit(EXIT_FAILURE);
	}

	return 0;
}

void funcionLog(char *mensaje){
	int resultado;
	char nombreFichero[100];
	char mensajeAEscribir[300];
	time_t t;

	// Abrir el fichero
	sprintf(nombreFichero,"ejercicio3.txt");
	if(fLog==NULL){
		fLog = fopen(nombreFichero,"at");
		if(fLog==NULL){
			perror("Error abriendo el fichero de log");
			exit(1);
		}
	}

	// Obtener la hora actual
	t = time(NULL);
	struct tm * p = localtime(&t);
	strftime(mensajeAEscribir, 1000, "[%Y-%m-%d, %H:%M:%S]", p);

	// Vamos a incluir la hora y el mensaje que nos pasan
	sprintf(mensajeAEscribir, "%s ==> %s\n", mensajeAEscribir, mensaje);

	// Escribir finalmente en el fichero
	resultado = fputs(mensajeAEscribir,fLog);
	if ( resultado < 0)
		perror("Error escribiendo en el fichero de log");

	fclose(fLog);
	fLog=NULL;
}
