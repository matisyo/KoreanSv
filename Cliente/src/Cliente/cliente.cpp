#include "cliente.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
void cliente::error(const char *msg)
{
	Logger::Instance()->LOG(msg, ERROR);
    exit(0);
}

void cliente::conectar()
{
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
       error("Cliente: El cliente no se pudo conectar satisfactoriamente");
}
cliente::cliente(int argc, char *ip,char *port){
    if (argc < 3) {
       fprintf(stderr,"usage hostname port\n");
       exit(0);
    }
    portno = atoi(port);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    	error("Cliente: Error en la creación del socket");
    server = gethostbyname(ip);// en caso de no ser ip si es ip anda igual
    if (server == NULL) {
    	error("Cliente: No se pudo encontrar el servidor ingresado");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);

}
void cliente::escribir()
{
    printf("Por favor. Ingrese un mensaje: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = send(sockfd, buffer, strlen(buffer), 0);
    if (n < 0)
    	Logger::Instance()->LOG("Cliente: No se pudo escribir el mensaje.", WARN);

}
void cliente::leer()
{
    bzero(buffer,256);
    n = recv(sockfd, buffer, 255, 0);
    if (n < 0)
       error("Cliente: Error al leer del socket.");
   printf("%s\n", buffer);
}

void cliente::cerrarSoket()
{
    close(sockfd);
}

int main(int argc, char *argv[])
{
    cliente* client = new cliente(argc,argv[1],argv[2]);

    client->conectar();

    while(true){
    	client->escribir();
    	client->leer();
    }
    client->cerrarSoket();

    Logger::Instance()->Close();
    delete client;
    return 0;
}
