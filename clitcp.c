/*  DIEGO FERNANDEZ C.
    DAVID QUIJÓN F.*/

//Programa fuente CLIENTE


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

int main (int argc, char *argv[]) {
    int sock;
    struct sockaddr_in name;
    struct hostent *hp;
    char mensaje_uno[200];
    char mensaje_dos[200];
    char mensaje_tres[200];
    char confirmacion1[200] = "Persona NO habilitada para censar\n";
    char confirmacion2[200] = "Tipo de usuario incorrecto\n";
    char tipo = 's';

    int region = 0;
    int casas = 0;
    int residentes = 0;
    int hombres = 0;
    int mujeres = 0;

    if(argc<5){ // validar los argumentos de entrada de cliente NOMBRE EXE, IP, PUERTO
        printf("\nSe debe utilizar: %s <ip-servidor> <puerto> <rut> <s/c>\n\n", argv[0]);
        exit(0);
    }
	  sock = socket(AF_INET,SOCK_STREAM,0);
    hp = gethostbyname(argv[1]);
    name.sin_family = AF_INET; // Para protocolo de Internet IPv4.
    bcopy ((char *)hp->h_addr,(char *)&name.sin_addr,hp->h_length);
    name.sin_port = htons(atoi(argv[2])); //que convierte un short int de host byte order a network byte order.
    connect(sock, (struct sockaddr *)&name, sizeof name);
     char msg[200];

    while(1){
      int nbytes;
      nbytes=strlen(argv[3])+1;
      send(sock,argv[3],nbytes,0);
      nbytes=recv(sock,mensaje_dos,200,0);

      if(strcmp(mensaje_dos,confirmacion1) == 0){
      printf("\n Respuesta del servidor: %s",mensaje_dos);
      return 0; // Ya que no esta habilitado para censar, se termina el programa
     }

     else {
       do{
       printf("\n Ingrese la region donde va a censar:     ");
       fgets(mensaje_uno,200,stdin);
       region = atoi(mensaje_uno);
       if ( region == 13 || region <= 0 || region > 15)
           printf("  Region ingresada no es válida\n");

     } while( region == 13 || region <= 0 || region > 15);


       do{
       printf("\n Ingrese la cantidad de casas que va a censar:      ");
       fgets(mensaje_uno,200,stdin);
       casas = atoi(mensaje_uno);
       if(casas <= 0)
         printf("  Ha ingresado una cantidad no válida\n");

       } while( casas  <= 0 );

       int l;
       for(l=0; l< casas; l++){

         do{
         printf("\n Ingrese la cantidad de habitantes de la casa Numero %d:      ",l+1);
         fgets(mensaje_uno,200,stdin);
         residentes = residentes + atoi(mensaje_uno);

         if(atoi(mensaje_uno) < 0){
           printf("  Ha ingresado una cantidad no válida\n");
           residentes = residentes - atoi(mensaje_uno);
         }

           } while( atoi(mensaje_uno) < 0 );

         do{
         printf("\n Ingrese la cantidad de hombres de la casa Numero %d:     ",l+1);
         fgets(mensaje_uno,200,stdin);
         hombres = hombres + atoi(mensaje_uno);

         if(atoi(mensaje_uno) < 0){
           printf("  Ha ingresado una cantidad no válida\n");
           hombres = hombres - atoi(mensaje_uno);
         }
          } while (atoi(mensaje_uno) < 0);

         do {
         printf("\n Ingrese la cantidad de mujeres de la casa Numero %d:     ",l+1);
         fgets(mensaje_uno,200,stdin);
         mujeres = mujeres + atoi(mensaje_uno);

         if(atoi(mensaje_uno) < 0){
           printf("  Ha ingresado una cantidad no válida\n");
           mujeres = mujeres - atoi(mensaje_uno); // Reestablece el valor anterior ya que al ingresar un valor negativo
         }                                        // este se resta a la suma acumulada.

          } while (atoi(mensaje_uno) < 0);

       }


          char cad[20];
          sprintf(cad,"%d",region);
          send(sock,cad,20,0);
          nbytes=recv(sock,mensaje_tres,200,0);
          //printf("Recibido: %s\n",mensaje_tres);

         sprintf(cad,"%d",casas);
         send(sock,cad,20,0);
         nbytes=recv(sock,mensaje_tres,200,0);
         //printf("Recibido: %s\n",mensaje_tres);

         sprintf(cad,"%d",residentes);
         send(sock,cad,20,0);
         nbytes=recv(sock,mensaje_tres,200,0);
         //printf("Recibido: %s\n",mensaje_tres);

         sprintf(cad,"%d",hombres);
         send(sock,cad,20,0);
         nbytes=recv(sock,mensaje_tres,200,0);
         //printf("Recibido: %s\n",mensaje_tres);

         sprintf(cad,"%d",mujeres);
         send(sock,cad,20,0);
         nbytes=recv(sock,mensaje_tres,200,0);
         //printf("Recibido: %s\n",mensaje_tres);

         printf("\n\n*********GRACIAS POR COLABORAR CON EL CENSO 2017*********\n\n");

         return 0;
       }
     }


        return 0;
        }
