#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <queue>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "mensije.h"
#include "multiqueue.h"



class server
{
    public:
        server(int port,int maxC);
        void escuchar();
        void aceptar();
        void escribir(int id);
        void leer(int id);
        void *newDialog(void);
        void *procesar(void);
        static void *mati_method(void *context);//ES LO UNICO QUE PUEDE TENER GUION BAJO POR LO PODEROSO QUE ES
        static void *mati_method2(void *context);/ESTE ES AUN MAS PODEROSO
        //esta es privada en realidad LO CAMBIAMOS ESTO?
        void startThread();
        //la de arriba

        void closeAllsockets();
        ~server();


    private:
        wqueue m_queue;
        void error(const char *msg);
        //void* procesar(void* obj);
        int sockfd, newsockfd, portno;
        socklen_t clilen;
        char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;
        int n;
        pthread_t threadDeProcesos;
        int listaDeClientes[10];
        int clientNUm;
        //por favor haganla constante
        int MAXCLIENTES;
       //std::mutex mtx;
};

#endif // SERVER_H
