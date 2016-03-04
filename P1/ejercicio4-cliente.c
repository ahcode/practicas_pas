#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>
#include "common.h"

//Prototipo de funcion
void funcionLog(char *);
// Apuntador al fichero de log (se utilizará en el ejercicio resumen)
FILE *fLog = NULL;

int main(int argc, char **argv)
{
	// Cola del servidor
	mqd_t mq_to_server;
  mqd_t mq_to_client;
	// Buffer para intercambiar mensajes
	char buffer[MAX_SIZE];
  char buffer2[MAX_SIZE+1];
	// Abrir la cola del servidor
	mq_to_server = mq_open(SERVER_QUEUE, O_WRONLY);
	if(mq_to_server == (mqd_t)-1 ){
        	perror("Error al abrir la cola del servidor");
       		exit(-1);
	}
  mq_to_client = mq_open(CLIENT_QUEUE, O_RDONLY);
	if(mq_to_client == (mqd_t)-1 ){
        	perror("Error al abrir la cola del cliente");
       		exit(-1);
	}

	printf("Mandando mensajes al servidor (escribir \"%s\" para parar):\n", MSG_STOP);
	while(1) {
		printf("> ");
		fflush(stdout);                  // Limpiar buffer de salida
		memset(buffer, 0, MAX_SIZE);     // Poner a 0 el buffer
		fgets(buffer, MAX_SIZE, stdin);  // Leer por teclado
		buffer[strlen(buffer)-1] = '\0'; // Descartar el salto de línea

		// Enviar y comprobar si el mensaje se manda
		if(mq_send(mq_to_server, buffer, MAX_SIZE, 0) != 0){
			perror("Error al enviar el mensaje");
			exit(-1);
		}
    // Comprobar salida
    if(!strncmp(buffer, MSG_STOP, strlen(MSG_STOP)))
      break;
    // Número de bytes leidos
		ssize_t bytes_read;
    // Recibir el mensaje
    bytes_read = mq_receive(mq_to_client, buffer2, MAX_SIZE, NULL);
    // Comprobar que la recepción es correcta (bytes leidos no son negativos)
		if(bytes_read < 0){
			perror("Error al recibir el mensaje");
			exit(-1);
		}
    // Cerrar la cadena
		buffer2[bytes_read] = '\0';
    // Imprimir respuesta
    printf("%s\n",buffer2);
	}

	// Cerrar la cola del servidor
	if(mq_close(mq_to_server) == (mqd_t)-1){
		perror("Error al cerrar la cola del servidor");
		exit(-1);
	}
  if(mq_close(mq_to_client) == (mqd_t)-1){
		perror("Error al cerrar la cola del cliente");
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
	sprintf(nombreFichero,"log-cliente.txt");
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
