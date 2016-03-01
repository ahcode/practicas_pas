#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h> //Control de errores
#include <string.h> //Para la funcion strerror()

#define MAX_SIZE 5

int addLog(char * cad);

int main(){
	int fildes[2], i, flag, status;
	pid_t pid;
	char buf[MAX_SIZE];

	if (pipe(fildes)==-1){
		printf("Error al crear la tubería\n");
		return 1;
	}

	switch(pid=fork()){
		case -1:
			printf("Error en la creación del proceso hijo\n");
			return 1;

		case 0:
			printf("[HIJO] mi PID es %d y mi PPID es %d\n",getpid(),getppid());
			if (close(fildes[1])==-1)
					perror("Error al cerrar la tubería");

			for(i=0;i<5;i++){
				if(read(fildes[0],buf,MAX_SIZE)==-1)
					perror("Error en la escritura en la tubería");
				printf("[HIJO]: leemos el número aleatorio %s de la tubería\n",buf);
			}

			if (close(fildes[0])==-1)
					perror("Error al cerrar la tubería");
			printf("[HIJO]: tubería cerrada. Salgo...\n");
			return 0;

		default:
			printf("[PADRE] mi PID es %d y el PID de mi hijo es %d\n",getpid(),pid);
			srand(time(NULL));
			if (close(fildes[0])==-1)
				perror("Error al cerrar la tubería");

			for(i=0;i<5;i++){
				sprintf(buf,"%d",rand()%5000);
				if(write(fildes[1],buf,MAX_SIZE)==-1)
					perror("Error en la lectura de la tubería");
				printf("[PADRE] escribimos el número aleatorio %s en la tubería\n",buf);
				usleep(rand()%1000000);
			}

			if (close(fildes[1])==-1)
					perror("Error al cerrar la tubería");
			printf("[PADRE]: tubería cerrada\n");
			break;
	}

	while ((flag = wait(&status)) > 0)
	{
		if (WIFEXITED(status))
		{
			printf("Hijo PID:%d finalizado, estado=%d\n", flag, WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {  //Para seniales como las de finalizar o matar
			printf("Hijo  PID:%d finalizado al recibir la señal %d\n", flag, WTERMSIG(status));
		} else if (WIFSTOPPED(status)) { //Para cuando se para un proceso. Al usar wait() en vez de waitpid() no nos sirve.
			printf("Hijo PID:%d parado al recibir la señal %d\n", flag,WSTOPSIG(status));
		} else if (WIFCONTINUED(status)){ //Para cuando se reanuda un proceso parado. Al usar wait() en vez de waitpid() no nos sirve.
			printf("Hijo reanudado\n");
		}
	}
	if (flag==-1 && errno==ECHILD)
	{
		printf("No hay más hijos que esperar\n");
		printf("status de errno=%d, definido como %s\n", errno, strerror(errno));
	}
	else
	{
		printf("Error en la invocacion de wait o la llamada ha sido interrumpida por una señal.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
