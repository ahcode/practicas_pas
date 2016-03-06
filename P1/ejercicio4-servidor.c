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
#include <unistd.h>
#include <signal.h>
#include "common.h"

//Prototipo de funciones
void funcionLog(char *);
void salir();
void m_salir(int signal);

// Apuntador al fichero de log (se utilizará en el ejercicio resumen)
FILE *fLog = NULL;
// Declaración de colas
mqd_t mq_to_server;
mqd_t mq_to_client;
// Nombre de colas
char cola_server[60];
char cola_client[60];
// Regex
regex_t re;

int main(int argc, char **argv)
{
	// Cadena de salida
	char out[300];
	// Nombre de usuario
	char *usuario;
	// Atributos de las colas
	struct mq_attr attr;
	// Buffer para intercambiar mensajes
	char buffer[MAX_SIZE + 1];
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
      sprintf(out,"Error al inicializar la expresión regular");
			printf("%s\n",out);
			funcionLog(out);
      exit(-1);
  }

	// Crear las colas de mensajes
	usuario = getlogin();
	if (usuario == NULL){
		sprintf(out,"Error al recuperar nombre de usuario");
		printf("%s\n",out);
		funcionLog(out);
		exit(-1);
	}
	sprintf(cola_server,"%s-%s",SERVER_QUEUE,usuario);
	mq_to_server = mq_open(cola_server, O_CREAT | O_RDONLY, 0644, &attr);
  if(mq_to_server == (mqd_t)-1 ){
		sprintf(out,"Error al abrir la cola del servidor: %s", strerror(errno));
		printf("%s\n",out);
		funcionLog(out);
    exit(-1);
	}
	sprintf(cola_client,"%s-%s",CLIENT_QUEUE,usuario);
  mq_to_client = mq_open(cola_client, O_CREAT | O_WRONLY, 0644, &attr);
  if(mq_to_client == (mqd_t)-1 ){
   	sprintf(out,"Error al abrir la cola del cliente: %s", strerror(errno));
		printf("%s\n",out);
		funcionLog(out);
    exit(-1);
	}

	while(1) {
		// Número de bytes leidos
		ssize_t bytes_read;

		// Recibir el mensaje
		bytes_read = mq_receive(mq_to_server, buffer, MAX_SIZE, NULL);
		// Comprobar que la recepción es correcta (bytes leidos no son negativos)
		if(bytes_read < 0){
			sprintf(out,"Error al recibir el mensaje: %s", strerror(errno));
			printf("%s\n",out);
			funcionLog(out);
			salir();
		}
		// Cerrar la cadena
		buffer[bytes_read] = '\0';

		// Comprobar el fin del bucle
		if (strncmp(buffer, MSG_STOP, strlen(MSG_STOP))==0)
			break;
		else{
			sprintf(out,"Recibido el mensaje: %s", buffer);
			printf("%s\n",out);
			funcionLog(out);
      if(regexec(&re, buffer, 0, NULL, 0)==0){
        //Empareja
        if(mq_send(mq_to_client, "Empareja", MAX_SIZE, 0) != 0){
    			sprintf(out,"Error al enviar el mensaje: %s", strerror(errno));
					printf("%s\n",out);
					funcionLog(out);
    			salir();
    		}
      }else{
        //No Empareja
        if(mq_send(mq_to_client, "No Empareja", MAX_SIZE, 0) != 0){
    			sprintf(out,"Error al enviar el mensaje: %s", strerror(errno));
					printf("%s\n",out);
					funcionLog(out);
    			salir();
    		}
      }
    }
	// Iterar hasta que llegue el código de salida
	}

	salir();

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

	//El cierre del fichero se gestiona en la función salir()
	//fclose(fLog);
	//fLog=NULL;
}

void salir(){
	char out[300];

	// Liberar regex
	regfree(&re);

	// Enviar salida al cliente
	if(mq_send(mq_to_client, MSG_STOP, MAX_SIZE, 0) != 0){
		sprintf(out,"Error al enviar el mensaje: %s", strerror(errno));
		printf("%s\n",out);
		funcionLog(out);
	}

	// Cerrar las colas
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
	// Eliminar las colas
	if(mq_unlink(cola_server) == (mqd_t)-1){
		sprintf(out,"Error al eliminar la cola del servidor: %s", strerror(errno));
		printf("%s\n",out);
		funcionLog(out);
	}
	if(mq_unlink(cola_client) == (mqd_t)-1){
		sprintf(out,"Error al eliminar la cola del cliente: %s", strerror(errno));
		printf("%s\n",out);
		funcionLog(out);
	}

	//Cierre del fichero de Log
	fclose(fLog);

	exit(EXIT_SUCCESS);
}

void m_salir(int signal){
	char msg[50];
	sprintf(msg,"Saliendo del programa tras recibir una señal: %d",signal);
	funcionLog(msg);
  salir();
}
