#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

void infoUsuario(struct passwd *pw, int g, char i, int mostrar);

int main (int argc, char **argv) {
	int gflag = 0, eflag = 0, sflag = 0, mostrarUsuario = 0;
	char idioma;
	char *nvalue = NULL, *uvalue = NULL;
	int c;
	struct passwd *pw;
	opterr = 0;

	//Buscar opciones introducidas
	while ((c = getopt (argc, argv, "n:u:ges")) != -1){
		switch (c)
		{
			case 'n':
				nvalue = optarg;
				break;
			case 'u':
				uvalue = optarg;
				break;
			case 'g':
				gflag = 1;
				break;
			case 'e':
				eflag = 1;
				idioma='e';
				break;
			case 's':
				sflag = 1;
				idioma='s';
				break;
			case '?':
				if (optopt == 'n' || optopt == 'u')
					fprintf (stderr, "La opción %c requiere un argumento.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Opción desconocida '-%c'.\n", optopt);
				else
					fprintf (stderr, "Caracter `\\x%x'.\n", optopt);
				exit(-1);
			default:
				abort ();
		}
	}

	//Asignar idioma
	if (!eflag && !sflag){
		if (strstr(getenv("LANG"),"ES"))
			idioma='s';
		else
			idioma='e';
	}

	//Errores en las opciones
	if (nvalue!=NULL && uvalue!=NULL){
		printf("No se puede utilizar la opción -u y -n a la vez.\n");
		exit(-1);
	}
	if (eflag && sflag){
		printf("No se puede utilizar la opción -e y -s a la vez.\n");
		exit(-1);
	}

	//Asignar nombre de usuario
	if (nvalue==NULL && uvalue==NULL){
		mostrarUsuario=1;
		if ((nvalue = getlogin()) == NULL){
			printf("Error al recuperar nombre de usuario.\n");
			exit(-1);
		}
	}

	//Crear estructura
	if (nvalue!=NULL){
		if ((pw = getpwnam(nvalue)) == NULL){
			fprintf(stderr, "Nombre de usuario no encontrado.\n");
			exit(-1);
		}
	}else{
		if ((pw = getpwuid(atoi(uvalue))) == NULL){
			fprintf(stderr, "ID de usuario no encontrado.\n");
			exit(-1);
		}
		mostrarUsuario = 1;
	}

	//Imprimir resultados
	infoUsuario(pw,gflag,idioma,mostrarUsuario);

	return 0;
}

void infoUsuario(struct passwd *pw, int g, char i, int mostrar){
	struct group *gr;
	if(i=='e'){ //Inglés
		if(mostrar){
			printf("Username: %s\n", pw->pw_name);
		}
		printf("Name: %s\n", pw->pw_gecos);
		printf("Password: %s\n", pw->pw_passwd);
		printf("User ID: %d\n", pw->pw_uid);
		printf("Home folder: %s\n", pw->pw_dir);
		printf("Shell: %s\n", pw->pw_shell);
		if(g){
			gr=getgrgid(pw->pw_gid);
			printf("Group number: %d\n", pw->pw_gid);
			printf("Group name: %s\n", gr->gr_name);
		}
	}else{ //Español
		if(mostrar){
			printf("Usuario: %s\n", pw->pw_name);
		}
		printf("Nombre: %s\n", pw->pw_gecos);
		printf("Contraseña: %s\n", pw->pw_passwd);
		printf("ID de Usuario: %d\n", pw->pw_uid);
		printf("Carpeta personal: %s\n", pw->pw_dir);
		printf("Interprete de comandos: %s\n", pw->pw_shell);
		if(g){
			gr=getgrgid(pw->pw_gid);
			printf("Número de grupo: %d\n", pw->pw_gid);
			printf("Nombre de grupo: %s\n", gr->gr_name);
		}
	}
}
