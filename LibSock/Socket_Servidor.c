/*
* Javier Abellan, 20 Jun 2000
*
* Funciones para la apertura de un socket servidor y la conexion con sus
* clientes
*
* MODIFICACIONES:
* 4 Septiembre 2003: Añadida función Abre_Socket_Udp() 
*/

/* Includes del sistema */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "Socket_Servidor.h"
/*
* Se le pasa un socket de servidor y acepta en el una conexion de cliente.
* devuelve el descriptor del socket del cliente o -1 si hay problemas.
* Esta funcion vale para socket AF_INET o AF_UNIX.
*/
int Acepta_Conexion_Cliente (int Descriptor)
{
	socklen_t Longitud_Cliente;
	struct sockaddr_in Cliente; 
	int Hijo;
	char str[INET_ADDRSTRLEN];
	/*
	* La llamada a la funcion accept requiere que el parametro 
	* Longitud_Cliente contenga inicialmente el tamano de la
	* estructura Cliente que se le pase. A la vuelta de la
	* funcion, esta variable contiene la longitud de la informacion
	* util devuelta en Cliente
	*/
	Longitud_Cliente = sizeof (Cliente);
	//sin_size=sizeof(struct sockaddr_in);
	Hijo = accept (Descriptor,(struct sockaddr *)&Cliente, &Longitud_Cliente);
	if (Hijo == -1)
	{
		printf("error en accept()\n");
		return -1;
	}
	inet_ntop(AF_INET, &(Cliente.sin_addr), str, INET_ADDRSTRLEN);
 	printf("Se obtuvo una conexión desde %s\n",str); 
     	/* que mostrará la IP del cliente */
	
	/*
	* Se devuelve el descriptor en el que esta "enchufado" el cliente.
	*/
	return Hijo;
}

/*
* Abre un socket servidor de tipo AF_INET. Devuelve el descriptor
*	del socket o -1 si hay probleamas
* 
*/
int Abre_Socket_Inet ( uint16_t Port,int Backlog)
{
	struct sockaddr_in Direccion;
	
	//socklen_t Longitud_Cliente;
	//struct servent *Puerto;
	int Descriptor;/* los ficheros descriptores */	
	// se abre el socket
	
	Descriptor = socket (AF_INET, SOCK_STREAM, 0);
	if (Descriptor == -1)
	 	return -1;

	 /**     futura implementación
        * 
	//Se obtiene el servicio del fichero /etc/services
	
	Puerto = getservbyname (Servicio, "tcp");
	if (Puerto == NULL)
		return -1;
	**/


	/*
	* Se rellenan los campos de la estructura Direccion, necesaria
	* para la llamada a la funcion bind()
	*/
	Direccion.sin_family = AF_INET;
	//Direccion.sin_port = Puerto->s_port;
	Direccion.sin_port = htons(Port);
	Direccion.sin_addr.s_addr =INADDR_ANY;
	/* INADDR_ANY coloca nuestra dirección IP automáticamente */
	bzero(&(Direccion.sin_zero),8); 
   	/* escribimos ceros en el reto de la estructura */
	if(bind(Descriptor,(struct sockaddr*)&Direccion,
           sizeof(struct sockaddr))==-1)
	{
		printf("error en bind() \n");
		//close (Descriptor);
		return -1;
	}

	/*
	* Se avisa al sistema que comience a atender llamadas de clientes
	*/
	if (listen (Descriptor, Backlog) == -1)
	{	printf("error en listen()\n");
		//close (Descriptor);
		return -1;
	}


	/*
	* Se devuelve el descriptor del socket servidor
	*/
	return Descriptor;
}
