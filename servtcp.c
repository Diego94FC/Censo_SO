/* El Servidor */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <time.h>
#include <sys/time.h>
#include <sys/un.h>
#include<unistd.h>
#include <stdbool.h> //librerias para usar booleanos
#include<pthread.h>
#include "daemon.h"
#define MAXBUF 512   // constantes a usar en el programa
#define MAXNAME 10
#define MAXSZ 200
#define NUMREGIONES 16

typedef struct reg {
								int numhogares;
								int numresidentes;
								int numhombres;
								int nummujeres;
}Region;


typedef struct {
								char Rut[12];
								char Nombre[40];
								char tipo[40];
}ficha;  //estructura ficha que almacena los datos persona

ficha vector[50]; //hasta 50 personas
FILE *f;  //para la lectura del texto
int persona_registrada = 0;
char validacion[]="si";
char supervisor[1] = "s";
char censista[1] = "c";
Region regiones[NUMREGIONES];
void *connection_handler();
int sin_size, lenght;

void RegistraPersona(){ //funcion que almacena los censistas o supervisores

		if ((f=fopen("censista.txt","r"))==NULL) { //valida la apertura del archivo
										printf("El fichero no se pudo abrir \n ");
		}
		else{
										int i=0;
										while(!feof(f)) {
																		fscanf(f,"%s %s %s",vector[i].Rut,vector[i].Nombre,vector[i].tipo);
																		printf("%s %s %s\n",vector[i].Rut,vector[i].Nombre,vector[i].tipo);
																		i++;
																		persona_registrada = i;

										}

										fclose(f);
		}
}

bool Persona(char msg[MAXSZ]){ //valida que el votante esté dentro del archivo indicado

		printf("%s\n",msg);
		int j=0;
		for(j=0; j<persona_registrada; j++) {
										if(strcmp(vector[j].Rut,msg)==0) {
																		return true;
										}
		}
		return false;
}


int main () {


				RegistraPersona();
        void ctrlc(); //ejecuta la funcion para cerrar con control+C
        int sock, s, lenght, n, i, nbytes, *new_sock;; //inicializacion de socket y otros
        struct sockaddr_in name; //direccion del socket
        char msg[MAXBUF];  //buffer
				daemonize();



				int k;
				for (k=0; k<NUMREGIONES; k++) {
												regiones[k].numhogares = 0;
												regiones[k].numresidentes = 0;
												regiones[k].numhombres = 0;
												regiones[k].nummujeres = 0;
				}



        signal (SIGINT,ctrlc);
        sock = socket(AF_INET,SOCK_STREAM,0);  //conexion con el socket
        name.sin_family = AF_INET;
        name.sin_addr.s_addr = INADDR_ANY;
        name.sin_port = 0;
        bind(sock,(struct sockaddr *)&name, sizeof name);
        lenght = sizeof name;

        printf ("%d\n", getsockname(sock,(struct sockaddr *)&name, &lenght));
        printf ("El puerto asociada al socket es %d\n", ntohs(name.sin_port));


	char puerto[10];
	sprintf(puerto, "%d", ntohs(name.sin_port));
	log_message("El puerto asociada al socket es \n");
	log_message(puerto);
	log_message("El servidor esta  OK");
	printf ("El servidor esta  OK, Para bajarlo  Presione Control C\n\n");
        listen (sock, 10); //espera al cliente



	while(1){

	s=accept(sock,0,0); //espera y acepta la conexion

	printf("Nuevo cliente conectado\n");
	log_message("Nuevo cliente conectado\n");
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = s;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0){
            perror(" no se pudo crear el  thread");
	    log_message("no se puedo crear el thread");
            return 1;
        	}
					pthread_join(sniffer_thread, NULL);

	}//cierre while
 	return 0;

	}//cierre main

void *connection_handler(void *socket_desc)
{

	    int sock = *(int*)socket_desc;
	    int s,salida=0,numero=0,n;
	    char msg[MAXSZ];
			char respuesta[MAXSZ];


				char mRut[MAXSZ];
				n=recv(sock,mRut,MAXSZ,0);
				if(n==0){
					close(sock);
				////	break;
					}
					log_message("Rut recibido: ");
					log_message(mRut);

					if(Persona(mRut)) {

						sprintf(msg,"Persona registrada\n");
						n=strlen(msg)+1;
						send(sock,msg,MAXSZ,0);
						log_message(msg);

					}

					else{
						sprintf(msg,"Persona NO habilitada para censar\n");
						n=strlen(msg)+1;
						send(sock,msg,MAXSZ,0);
						log_message(msg);
						close(sock);
																							 pthread_exit(0);
					}


					n=recv(sock,msg,MAXSZ,0); // Recibe la región
						 if(n==0){
							 close(s);
							 //break;
							 }
							 printf("Recibido: %s\n",msg);
							 sprintf(respuesta,"Region recibida: %s\n",msg);
							 log_message("Region recibida: ");
							 log_message(msg);
							 send(sock,respuesta,MAXSZ,0);
							 int region = atoi( msg );
									 //write(regionpipe[1],&region,sizeof(region)); // Escribe la región recibida en el pipe correspondiente

							 n=recv(sock,msg,MAXSZ,0); // Recibe la cantidad de casas
							 if(n==0){
									 close(s);
									 //break;
									 }
							 printf("Recibido: %s\n",msg);
							 sprintf(respuesta,"Cantidad de casas recibida: %s\n",msg);
							 log_message("Casas recibidas: ");
							 log_message(msg);
							 send(sock,respuesta,MAXSZ,0);
							 int casas = atoi(msg);
															 //write(mypipefd[1],&casas,sizeof(casas));  // Escribe en el pipe


								 n=recv(sock,msg,MAXSZ,0); // Recibe el numero de habitantes total de las casas a censar en esta sesión.
								 if(n==0){
									 close(s);
									 //break;
									 }
									 printf("Recibido: %s\n",msg);
									 sprintf(respuesta,"Numero de habitantes recibido: %s\n",msg);
									 log_message("Habitantes recibida: ");
									 log_message(msg);
									 send(sock,respuesta,MAXSZ,0);
									 int residentes = atoi( msg );
																	 //write(habitantesPipe[1],&residentes,sizeof(residentes));

								 n=recv(sock,msg,MAXSZ,0); // Recibe el numero total de hombres en los hogares a censar en esta sesión
								 if(n==0){
									 close(s);
									 //break;
									 }
									 printf("Recibido: %s\n",msg);
									 sprintf(respuesta,"Numero de hombres recibido: %s\n",msg);
									 log_message("Hombres recibida: ");
									 log_message(msg);
									 send(sock,respuesta,MAXSZ,0);
									 int hombres = atoi( msg );
																	 //write(hombresPipe[1],&hombres,sizeof(hombres));


								 n=recv(sock,msg,MAXSZ,0); // Recibe el numero total de mujeres en los hogares a censar en esta sesión
								 if(n==0){
									 close(s);
									 //break;
									 }
									 printf("Recibido: %s\n",msg);
									 sprintf(respuesta,"Numero de mujeres recibido: %s\n",msg);
									 log_message("Mujeres recibida: ");
									 log_message(msg);
									 send(sock,respuesta,MAXSZ,0);
									 int mujeres = atoi( msg );
																	 //write(mujeresPipe[1],&mujeres,sizeof(mujeres));

											 regiones[region].numhogares = regiones[region].numhogares + casas;
											 regiones[region].numresidentes = regiones[region].numresidentes + residentes;
											 regiones[region].numhombres = regiones[region].numhombres + hombres;
											 regiones[region].nummujeres = regiones[region].nummujeres + mujeres;

											 log_message("....................RESULTADOS DEL CENSO..................\n" );

															 int x;
															 for(x=0;x< NUMREGIONES;x++){
																	 if( x == 13){
																			 x++;
																	 }
																	 log_message("Region: ");
																	 sprintf(respuesta,"%d", x);
																	 log_message(respuesta);

																	 log_message("Casas: ");
																	 sprintf(respuesta,"%d", regiones[x].numhogares);
																	 log_message(respuesta);

																	 log_message("Habitantes: ");
																	 sprintf(respuesta,"%d", regiones[x].numresidentes);
																	 log_message(respuesta);

																	 log_message("Hombres: ");
																	 sprintf(respuesta,"%d", regiones[x].numhombres);
																	 log_message(respuesta);

											 					 	 log_message("Mujeres: ");
																	 sprintf(respuesta,"%d", regiones[x].nummujeres);
																	 log_message(respuesta);
																	 log_message("\n");
															 }

					close(sock);
  				pthread_exit(0);
    return 0;
}

void ctrlc() {
        printf("\nServidor is OUT\n");
        exit(0);
	}
