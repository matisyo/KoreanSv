#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "Cliente.h"
#include <iostream>
using namespace std;


void Cliente::error(const char *msg)
{
    perror(msg);
    exit(0);
}

void Cliente::conectar()
{
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
}
Cliente::Cliente(int argc, char *ip,char *port){
    if (argc < 3) {
       fprintf(stderr,"usage hostname port\n");
       exit(0);
    }
    portno = atoi(port);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(ip);// en caso de no ser ip si es ip anda igual
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);

}
void Cliente::escribir()
{
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
         error("ERROR writing to socket");

}
void Cliente::leer()
{
    bzero(buffer,256);
    n = read(sockfd, buffer, 255);
    if (n < 0)
       error("ERROR reading from socket");
   printf("%s\n", buffer);
}

void Cliente::cerrarSoket()
{
    close(sockfd);
}
int main(int argc, char *argv[])
{
    string fileName;

    cout << "Bienvenido al cliente \n";
    cout << "Ingrese el nombre del archivo con el que desea trabajar \n";

    cin >> fileName;
    //Aca parseo con ParserCliente el archivo que desea trabajar y consigo 
    //Las variables argc y argv que antes entraban por consola

    Cliente clientiti (argc,argv[1],argv[2]);

    clientiti.conectar();

    while(true){
    clientiti.escribir();
    clientiti.leer();
    }
    clientiti.cerrarSoket();
    return 0;
}
