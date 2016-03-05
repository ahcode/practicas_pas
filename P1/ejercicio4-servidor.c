#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>
#include <getopt.h>
#include <regex.h>
#include "common.h"

//Prototipo de funcionn
void funcionLog(char *);
// Apuntador al fichero de log (se utilizará en el ejercicio resumen)
FILE *fLog = NULL;


int main(int argc, char **argv)
{
	// Cadena de salida
	char out[300];
	// Declaración de colas
	mqd_t mq_to_server;
  mqd_t mq_to_client;
	// Atributos de las colas
	struct mq_attr attr;
	// Buffer para intercambiar mensajes
	char buffer[MAX_SIZE + 1];
  // Regex
  regex_t re;
	// Inicializar los atributos de la cola
	attr.mq_maxmsg = 10;        // Maximo número de mensajes
	attr.mq_msgsize = MAX_SIZE; // Maximo tamaño de un mensaje
	// Opciones
	int c, hflag=0, option_index = 0;
	char *rvalue=NULL;
	static struct option opciones[] =
	{
		{"regex", required_argument, 0, 'r'},
		{"help", no_argument, 0, 'h'},
		/* Necesario para indicar el final de las opciones */
		{0, 0, 0, 0}
	};

	while ((c = getopt_long (argc, argv, "r:h", opciones, &option_index))!=-1){
		switch (c){
			case 'r':
				rvalue=optarg;
				break;
			case 'h':
				hflag=1;
				break;
			case '?':
				break;
			default:
				abort ();
		}
	}

	if (hflag==1){
		sprintf(out,"Ayuda");
		printf("%s\n",out);
		funcionLog(out);
		return 0;
	}

	if (rvalue==NULL){
		sprintf(out,"Debes indicar un valor para la expresión regular con -r");
		printf("%s\n",out);
		funcionLog(out);
		exit(-1);
	}

  // Inicializar regex
  if (regcomp(&re,rvalue,0)!=0){
      printf("Error al inicializar la expresión regular");
      exit(-1);
  }
	// Crear las colas de mensajes
	mq_to_server = mq_open(SERVER_QUEUE, O_CREAT | O_RDONLY, 0644, &attr);
  if(mq_to_server == (mqd_t)-1 ){
   	perror("Error al abrir la cola del servidor");
    exit(-1);
	}
  mq_to_client = mq_open(CLIENT_QUEUE, O_CREAT | O_WRONLY, 0644, &attr);
  if(mq_to_client == (mqd_t)-1 ){
   	perror("Error al abrir la cola del cliente");
    exit(-1);
	}

	while(1) {
		// Número de bytes leidos
		ssize_t bytes_read;

		// Recibir el mensaje
		bytes_read = mq_receive(mq_to_server, buffer, MAX_SIZE, NULL);
		// Comprobar que la recepción es correcta (bytes leidos no son negativos)
		if(bytes_read < 0){
			perror("Error al recibir el mensaje");
			exit(-1);
		}
		// Cerrar la cadena
		buffer[bytes_read] = '\0';

		// Comprobar el fin del bucle
		if (strncmp(buffer, MSG_STOP, strlen(MSG_STOP))==0)
			break;
		else{
			printf("Recibido el mensaje: %s\n", buffer);
      if(regexec(&re, buffer, 0, NULL, 0)==0){
        //Empareja
        if(mq_send(mq_to_client, "Empareja", MAX_SIZE, 0) != 0){
    			perror("Error al enviar el mensaje");
    			exit(-1);
    		}
      }else{
        //No Empareja
        if(mq_send(mq_to_client, "No Empareja", MAX_SIZE, 0) != 0){
    			perror("Error al enviar el mensaje");
    			exit(-1);
    		}
      }
    }
	// Iterar hasta que llegue el código de salida
	}

  // Liberar regex
  regfree(&re);

	// Cerrar la cola del servidor
	if(mq_close(mq_to_server) == (mqd_t)-1){
		perror("Error al cerrar la cola del servidor");
		exit(-1);
	}
  if(mq_close(mq_to_client) == (mqd_t)-1){
    perror("Error al cerrar la cola del cliente");
    exit(-1);
  }
	// Eliminar la cola del servidor
	if(mq_unlink(SERVER_QUEUE) == (mqd_t)-1){
		perror("Error al eliminar la cola del servidor");
		exit(-1);
	}
  if(mq_unlink(CLIENT_QUEUE) == (mqd_t)-1){
		perror("Error al eliminar la cola del cliente");
		exit(-1);
	}

	return 0;
}


// Función auxiliar, escritura de un log
void funcionLog(char *mensaje)
{
	int resultado;
	char nombreFichero[100];
	char mensajeAEscribir[300];
	time_t t;

	// Abrir el fichero
	sprintf(nombreFichero,"log-servidor.txt");
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
