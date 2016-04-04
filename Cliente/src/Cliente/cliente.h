#ifndef CLIENTE_H
#define CLIENTE_H

#include "../Utils/Logger.h"
#include "../Cliente/mensije.h"
#include "../Cliente/multiqueue.h"
#include "AlanTuring.h"
#include "MessageFactory.h"

#include <stdio.h>
#include <queue>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class cliente
{
    public:
        cliente(int argc, char *ip,char *port);
        ~cliente();
        void conectar();
        void escribir();
        void leer();
        void cerrarSoket();
        bool checkConection();

    private:
        AlanTuring* m_alanTuring;
        int sockfd, portno, n;
        bool m_conected;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        void error(const char *msg);

        char buffer[256];

        bool lecturaExitosa(int bytesLeidos);
        void procesarMensaje(NetworkMessage networkMessage);
        void sendMsg(Mensaje msg);
        bool validarMensaje(DataMessage dataMsg);

};

#endif // CLIENTE_H
