#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <getopt.h>

char idioma;

void imprimirGrupo(struct group *gr);
int imprimirTodo();
void noExiste();
void ayuda();

int main (int argc, char **argv) {
	int aflag = 0, hflag = 0, eflag = 0, sflag = 0;
	char *gvalue = NULL;
	int c;
	int option_index = 0;
	struct group *gr;

	static struct option opciones[] =
	{
		{"group", required_argument, 0, 'g'},
		{"english", no_argument, 0, 'e'},
		{"spanish", no_argument, 0, 's'},
		{"help", no_argument, 0, 'h'},
		{"all", no_argument, 0, 'a'},
		/* Necesario para indicar el final de las opciones */
		{0, 0, 0, 0}
	};


	//Buscar opciones introducidas
	while ((c = getopt_long (argc, argv, "g:esha", opciones, &option_index))!=-1){
		switch (c){
			case 'g':
				gvalue=optarg;
				break;
			case 'e':
				eflag=1;
				idioma='e';
				break;
			case 's':
				sflag=1;
				idioma='s';
				break;
			case 'h':
				hflag=1;
				break;
			case 'a':
				aflag=1;
				break;
			case '?':
				break;
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
	if (eflag && sflag){
		printf("No se puede utilizar la opción -e y -s a la vez.\n");
		exit(-1);
	}
	if ((gvalue!=NULL && hflag==1)||(gvalue!=NULL && aflag==1)||(aflag==1 && hflag==1)){
		printf("No se pueden usar las opciones indicadas a la vez.\n");
		exit(-1);
	}

	if (hflag==1){
		ayuda();
		return 0;
	}
	if (aflag==1){
		if (imprimirTodo()==1){
			exit(-1);
		}
		return 0;
	}else if(gvalue!=NULL){
		if((gr=getgrnam(gvalue))==NULL){
			noExiste();
			exit(-1);
		}
	}else{
		gr=getgrgid(getpwnam(getlogin())->pw_gid);
	}
	imprimirGrupo(gr);
	return 0;
}

void imprimirGrupo(struct group *gr){
	if (idioma=='s'){
		printf("Identificador del grupo: %d\n", gr->gr_gid);
		printf("Nombre de grupo: %s\n", gr->gr_name);
	}else{
		printf("Group ID: %d\n", gr->gr_gid);
		printf("Groupre Name: %s\n", gr->gr_name);
	}
}

int imprimirTodo(){
	FILE * f=fopen("/etc/group","r");
	char cad[50];
	if (f==NULL){
		if (idioma=='s'){
			printf("No se puede abrir el fichero de grupos\n");
		}else{
			printf("Group file can not be opened\n");
		}
		fclose(f);
		exit(-1);
	}
	while (fgets(cad,51,f)!=NULL){
		*(strchr(cad,':'))='\0';
		imprimirGrupo(getgrnam(cad));
		printf("\n");
	}
	fclose(f);
	return 0;
}

void noExiste(){
	if (idioma=='s'){
		printf("El grupo especificado no existe\n");
	}else{
		printf("The group does not exist\n");
	}
}

void ayuda(){
	if (idioma=='s'){
		printf("\nModo de Empleo: ./ejercicio2 [opciones] [grupo]\n");
		printf("\nMuestra información sobre uno o todos los grupos de usuarios existentes.\n");
		printf("\nOpciones:");
		printf("\n\t-g\t--group [grupo]\tMuestra información sobre un determinado grupo.");
		printf("\n\t-s\t--spanish\tSelecciona el idioma español");
		printf("\n\t-e\t--english\tSelecciona el idioma inglés");
		printf("\n\t-h\t--help\t\tMuestra la ayuda");
		printf("\n\t-a\t--all\t\tMuestra información sobre todos los grupos");
		printf("\n\nSi no se especifican opciones se muestra información del grupo del usuario actual.\n\n");
	}else{
		printf("\nHow to use: ./ejercicio2 [options] [group]\n");
		printf("\nIt shows information about the user groups.\n");
		printf("\nOptions:");
		printf("\n\t-g\t--group [grupo]\tIt shows information about a group.");
		printf("\n\t-s\t--spanish\tChange the language to spanish");
		printf("\n\t-e\t--english\tChange the language to english");
		printf("\n\t-h\t--help\t\tIt shows the help");
		printf("\n\t-a\t--all\t\tIt shows information about all groups");
		printf("\n\nIf there is not options, it shows information about the group of the current user.\n\n");
	}
}
