/*
 * Para testear el daemon usamos: ps -ef | grep serveld (o ps -aux en Linux)
 * Para testear archivo log: tail -f /tmp/daemon.log
 * Para testear se~nal: kill -HUP ‘cat /tmp/daemon.lock‘
 * Para finalizar la ejecuci´on del proceso: kill ‘cat /tmp/daemon.lock‘
 */

#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#define RUNNING_DIR "/tmp" /* Se puede usar un directorio diferente */
#define LOCK_FILE "daemon.lock" /* escribir´a aqu´ı su PID */
#define LOG_FILE "daemon.log" /* mensajes del proceso demonio */

/*
 *
 * Como el proceso demonio no puede imprimir mensajes por pantalla
 * entonces lo har´a en el archivo daemon.log
 *
 */
void log_message(char *mensaje)
{
        FILE *logfile;
        logfile = fopen(LOG_FILE,"a"); /*Lo abre para escritura, pero adhiere al final y
                                          despu´es de lo ya escrito (modo append)*/
        if( !logfile ) { exit(0); /* error al abrir el archivo*/
                         pperror("open file error");
                         exit(-1); }
        fprintf(logfile,"%s\n",mensaje); /*escribe mensaje en el archivo*/
        fclose(logfile);
}
/* El handler o rutina de interrupciones */
void signal_handler(int senial)
{
        switch( senial ) {
        case SIGHUP: /*Para reiniciar el servidor*/
                log_message("Se~nal HUNGUP capturada");
                break;
        case SIGTERM: /*Para finalizar la ejecucion del servidor*/
                log_message("se~nal terminate capturada");
                exit(0);
        }
}
/* funci´on que daemoniza un proceso */
void daemonize()
{
        int i, fd, pid, lfp;
        char str[10];
        pid = fork();
        if ( pid<0 ) exit(1);  /* error en fork */
        if ( pid>0 ) exit(0);  /* padre termina */
/* El hijo (el daemon) continua con su ejecuci´on y pasa a ser hijo de init */
        setsid(); /* se convierte en el l´ıder de una sesi´on o grupo de procesos */
        for ( i = getdtablesize(); i >= 0; --i )
                close(i);  /* cierra todos los descriptores de la tabla de archivos */
        fd = open("/dev/null",O_RDWR);
        dup(fd); /* entrada est´andar */
        dup(fd); /* salida est´andar */
        dup(fd); /* manejo de errores est´andar.*/
/* Asi nos aseguramos que el demonio no vuelva a adquirir un terminal */
/* Para manejar IO est´andar en un dispositivo inofensivo como /dev/null */
        umask(027); /* inicia nuevamente los permisos del archivo creado con permiso 750 */
        chdir(RUNNING_DIR); /* cambia el directorio de trabajo. Aqu´ı se encontraran
                               almacenados los archivos daemon.lock y daemon.log.
                               Este directorio de trabajo lo pueden cambiar */
        lfp = open(LOCK_FILE,O_RDWR|O_CREAT,0640); /*Define un cerrojo para el archivo*/
        if ( lfp < 0 ) exit(1);  /* no pudo abrir el archivo */
        if ( lockf(lfp,F_TLOCK,0) < 0 ) exit(0);  /*no pudo lockear */
/* Para comenzar a imprimir el PID en el archivo lock */
        sprintf(str,"%d\n",getpid());
        write(lfp,str,strlen(str)); /* escribe pid en el archivo lock */
        signal(SIGCHLD,SIG_IGN); /* ignora se~nales de un proceso hijo */
        signal(SIGTSTP,SIG_IGN); /* ignora se~nales desde el tty */
        signal(SIGTTOU,SIG_IGN); /* ignora se~nales de salida est´andar */
        signal(SIGTTIN,SIG_IGN); /* ignora se~nales de entrada est´andar */
        signal(SIGHUP,signal_handler); /* Para atrapar se~nal de hangup o reinicio */
        signal(SIGTERM,signal_handler); /* Para atrapar se~nal kill, de termino de ejecucion */
}
