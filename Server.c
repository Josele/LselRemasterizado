#include <stdio.h>          
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "LibSock/Socket_Servidor.h"
#include "LibSock/Socket.h"
#include "Server.h"
#define MAXDATASIZE 150
#define PORT 45000
static char buff[MAXDATASIZE];  
static char respuesta[MAXDATASIZE];
volatile int tam;
volatile int flag_leer;
volatile int fenviar=0;
 RT_MUTEX fe_mutex;
volatile static int Des_Ser;
volatile static int Des_Clit;
enum serv_state {
  SERV_APT,
  SERV_RECVINST,
  SERV_ANSW,
  SERV_CLOSE,
};

static int conexion_start (fsm_t* this)
{
 if(-1==(Des_Clit=Acepta_Conexion_Cliente (Des_Ser)))
 return 0;

printf("C: conexion start\n");

return 1;
}

static int sckt_instruc (fsm_t* this)
{
memset(buff,'\0',MAXDATASIZE);
if(-1==Lee_Socket(Des_Clit,buff,MAXDATASIZE)){
printf("C: error de lectura\n");

//this->tt->dest_state=SERV_CLOSE;
close(Des_Clit); /* cierra fd2*/
if(-1==(Des_Clit=Acepta_Conexion_Cliente (Des_Ser)))

return 0;
}

return 1;
}

static int answer_prepare (fsm_t* this)
{
rt_mutex_acquire(&fe_mutex,TM_INFINITE);
if(fenviar=0){
rt_mutex_release(&fe_mutex);

return 0;
}
fenviar=0;
rt_mutex_release(&fe_mutex);
return 1;
}

static int sckt_close (fsm_t* this)
{

close(Des_Clit); /* cierra fd2*/ 

return 1;
}

static void something (fsm_t* this)
{
flag_leer=0;
}

static void  execute (fsm_t* this)
{
//printf ("Soy servidor, He recibido : %s\n", buff);
	//ejecutamos y respondemos con el OK o error
//send(Des_Clit,respuesta,MAXDATASIZE,0);
//printf("envio LO\n");

memset(respuesta,'\0',MAXDATASIZE);
sprintf(respuesta,"This command don't respond\0");
flag_leer=1;

}

static void  answer_response (fsm_t* this)
{
//printf ("Soy servidor, He recibido : %s\n", buff);
        //ejecutamos y respondemos con el OK o error
send(Des_Clit,respuesta,MAXDATASIZE,0);
printf("C: envio %s\n", respuesta);
flag_leer=0;

}
static void finish (fsm_t* this)
{
printf("C: finish\n");
// podemos añadir un flag con un flag con un break etc
}
// Explicit FSM description
static fsm_trans_t serv[] = {
  { SERV_APT, conexion_start, SERV_RECVINST,  something},
  { SERV_RECVINST,  sckt_instruc, SERV_ANSW,    execute   },
  { SERV_ANSW,answer_prepare, SERV_CLOSE, answer_response },
  { SERV_CLOSE,sckt_close, SERV_APT, finish },
  {-1, NULL, -1, NULL },
};


void Des_Ser_init(){
Des_Ser=Abre_Socket_Inet (PORT,1);

}


fsm_t* serv_init(){
rt_mutex_create(&fe_mutex, NULL);
fsm_t* serv_fsm = fsm_new (serv);

return serv_fsm;
}
int serv_state_trans(fsm_t* serv_fsm){
fsm_fire (serv_fsm); 
return flag_leer;
}



char* get_buff(){

return buff;

}

void set_res(char *resp,int length){
rt_mutex_acquire(&fe_mutex,TM_INFINITE);
memset(respuesta,'\0',MAXDATASIZE);
if(length>MAXDATASIZE){
	strncpy(respuesta,"Respuesta muy larga", 19);
	return;}
strncpy(respuesta,resp,length );

fenviar=1;
rt_mutex_release(&fe_mutex);
}
/*
int main()
{


Des_Ser=Abre_Socket_Inet (PORT,1);
  
  fsm_t* serv_fsm = fsm_new (serv);
while(1){

 fsm_fire (serv_fsm); 
}

return 0;
}*/
/*
*Lo más facil y elegante es añadir getter y setters. Lanzando la máquina de estado en el interp, el getter sería como un notify.
*Lo más rapido es rehacer el interp para que convivan con los sockets
*Otras opciones son hebras y punteros.
int socket_interp()
{


Des_Ser=Abre_Socket_Inet (PORT,1);
  
  fsm_t* serv_fsm = fsm_new (serv);
while(1){

 fsm_fire (serv_fsm); 
}

return 0;
}*/
