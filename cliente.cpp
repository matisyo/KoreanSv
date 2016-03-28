#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "cliente.h"
void cliente::error(const char *msg)
{
    perror(msg);
    exit(0);
}

void cliente::conectar()
{
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
}
cliente::cliente(int argc, char *ip,char *port){
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
void cliente::escribir()
{
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
         error("ERROR writing to socket");

}
void cliente::leer()
{
    bzero(buffer,256);
    n = read(sockfd, buffer, 255);
    if (n < 0)
       error("ERROR reading from socket");
   printf("%s\n", buffer);
}

void cliente::cerrarSoket()
{
    close(sockfd);
}
int main(int argc, char *argv[])
{

    cliente clientiti (argc,argv[1],argv[2]);

    clientiti.conectar();

    while(true){
    clientiti.escribir();
    clientiti.leer();
    }
    clientiti.cerrarSoket();
    return 0;
}
