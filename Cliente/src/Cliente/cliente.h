#ifndef CLIENTE_H
#define CLIENTE_H

#include "../Utils/Logger.h"

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
#include "../Cliente/mensije.h"
#include "../Cliente/multiqueue.h"



class cliente
{
    public:
        cliente(int argc, char *ip,char *port);
        void conectar();
        void escribir();
        void leer();
        void cerrarSoket();
        bool checkConection();

    private:
        int sockfd, portno, n;
        bool m_conected;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        void error(const char *msg);

        char buffer[256];

};

#endif // CLIENTE_H
