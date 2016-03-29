#ifndef SERVER_H
#define SERVER_H

#include "ListaInteligente.h"
#include "mensije.h"
#include "multiqueue.h"
#include "../Utils/Logger.h"

#include <stdio.h>
#include <queue>
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class server
{
    public:
        server(int port,int maxC);
        void escuchar();
        void aceptar();
        void escribir(int id);
        bool leer(int id);
        void closeAllsockets();
        const int getMaxClientes();
        const int getNumClientes();
        ~server();

        //METODOS LOCOS
         void *procesar(void);
         static void *mati_method(void *context);//ES LO UNICO QUE PUEDE TENER GUION BAJO POR LO PODEROSO QUE ES
         static void *mati_method2(void *context);//ESTE ES AUN MAS PODEROSO

    private:

        const int MAX_CLIENTES;
        multiqueue m_queue;
        int m_clientNum;
        ListaInteligente<int> m_listaDeClientes;
        std::vector<pthread_t> m_clientThreads;

        int m_lastID;
        pthread_mutex_t  m_mutex;
        pthread_cond_t   m_condv;

        int sockfd, newsockfd, portno;
        socklen_t clilen;
        char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;
        pthread_t threadDeProcesos;

        void closeSocket(int id);
        void reducirNumeroClientes();
        void aumentarNumeroClientes();
        void startThread();
        void *newDialog(void);
        void error(const char *msg);

};

#endif // SERVER_H
