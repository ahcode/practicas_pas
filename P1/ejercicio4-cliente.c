#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"

//Prototipo de funciones
void funcionLog(char *);
void salir();
void m_salir(int signal);
// Colas
mqd_t mq_to_server;
mqd_t mq_to_client;
// Apuntador al fichero de log (se utilizará en el ejercicio resumen)
FILE *fLog = NULL;

int main(int argc, char **argv)
{
	// Cadena de salida
	char out[300];
	// Nombre cola
	char *usuario;
	char cola[60];
	// Buffer para intercambiar mensajes
	char buffer[MAX_SIZE];
  char buffer2[MAX_SIZE+1];
	// Manejadores de señales
	if (signal(SIGINT, m_salir) == SIG_ERR){
		sprintf(out,"No puedo asociar la señal SIGINT al manejador!");
		printf("%s\n",out);
		funcionLog(out);
		exit(-1);
	}if (signal(SIGHUP, m_salir) == SIG_ERR){
		sprintf(out,"No puedo asociar la señal SIGHUP al manejador!");
		printf("%s\n",out);
		funcionLog(out);
		exit(-1);
	}if (signal(SIGTERM, m_salir) == SIG_ERR){
		sprintf(out,"No puedo asociar la señal SIGTERM al manejador!");
		printf("%s\n",out);
		funcionLog(out);
		exit(-1);
	}
	// Abrir las colas
	usuario = getlogin();
	if (usuario == NULL){
		sprintf(out,"Error al recuperar nombre de usuario");
		printf("%s\n",out);
		funcionLog(out);
		exit(-1);
	}
	sprintf(cola,"%s-%s",SERVER_QUEUE,usuario);
	mq_to_server = mq_open(cola, O_WRONLY);
	if(mq_to_server == (mqd_t)-1 ){
        	sprintf(out,"Error al abrir la cola del servidor: %s", strerror(errno));
					printf("%s\n",out);
					funcionLog(out);
       		exit(-1);
	}
	sprintf(cola,"%s-%s",CLIENT_QUEUE,usuario);
  mq_to_client = mq_open(cola, O_RDONLY);
	if(mq_to_client == (mqd_t)-1 ){
        	sprintf(out,"Error al abrir la cola del cliente");
       		exit(-1);
	}

	sprintf(out,"Mandando mensajes al servidor (escribir \"%s\" para parar):", MSG_STOP);
	printf("%s\n",out);
	funcionLog(out);
	while(1) {
		printf("> ");
		fflush(stdout);                  // Limpiar buffer de salida
		memset(buffer, 0, MAX_SIZE);     // Poner a 0 el buffer
		fgets(buffer, MAX_SIZE, stdin);  // Leer por teclado
		buffer[strlen(buffer)-1] = '\0'; // Descartar el salto de línea
		sprintf(out,"> %s",buffer);
		funcionLog(out);

		// Enviar y comprobar si el mensaje se manda
		if(mq_send(mq_to_server, buffer, MAX_SIZE, 0) != 0){
			sprintf(out,"Error al enviar el mensaje: %s", strerror(errno));
			printf("%s\n",out);
			funcionLog(out);
			salir();
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
			sprintf(out,"Error al recibir el mensaje: %s", strerror(errno));
			printf("%s\n",out);
			funcionLog(out);
			salir();
		}
    // Cerrar la cadena
		buffer2[bytes_read] = '\0';
		// Comprobar salida
		if (strncmp(buffer2, MSG_STOP, strlen(MSG_STOP))==0)
			salir();
    // Imprimir respuesta
    sprintf(out,"%s",buffer2);
		printf("%s\n",out);
		funcionLog(out);
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

	//El cierre del fichero se gestiona en la función salir()
	//fclose(fLog);
	//fLog=NULL;
}

void salir(){
	char out[300];

	// Enviar salida al servidor
	if(mq_send(mq_to_server, MSG_STOP, MAX_SIZE, 0) != 0){
		sprintf(out,"Error al enviar el mensaje: %s", strerror(errno));
		printf("%s\n",out);
		funcionLog(out);
	}
	// Cerrar la cola del servidor
	if(mq_close(mq_to_server) == (mqd_t)-1){
		sprintf(out,"Error al cerrar la cola del servidor: %s", strerror(errno));
		printf("%s\n",out);
		funcionLog(out);
	}
  if(mq_close(mq_to_client) == (mqd_t)-1){
		sprintf(out,"Error al cerrar la cola del cliente: %s", strerror(errno));
		printf("%s\n",out);
		funcionLog(out);
	}

	fclose(fLog);

	exit(EXIT_SUCCESS);
}

void m_salir(int signal){
	char msg[50];
	sprintf(msg,"Saliendo del programa tras recibir una señal: %d",signal);
	funcionLog(msg);
  salir();
}
