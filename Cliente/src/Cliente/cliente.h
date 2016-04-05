#ifndef CLIENTE_H
#define CLIENTE_H

#include "../Utils/Logger.h"
#include "../Cliente/mensije.h"
#include "../Cliente/multiqueue.h"
#include "AlanTuring.h"
#include "MessageFactory.h"
#include "../Utils/TiposDefinidos.h"

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
        cliente(int argc, string ip,int port, std::vector<Mensaje> listaDeMensajesCargados );
        ~cliente();
        void conectar();
        void desconectar();
        void escribir(Mensaje mensaje);
        void leer();
        void cerrarSoket();
        bool checkConection();
        int sockfd, portno, n;

    private:
        AlanTuring* m_alanTuring;

        bool m_conected;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        void error(const char *msg);
        std::vector<Mensaje> listaDeMensajes;

        char buffer[256];

        bool lecturaExitosa(int bytesLeidos);
        void procesarMensaje(NetworkMessage networkMessage);
        void sendMsg(Mensaje msg);
        bool validarMensaje(DataMessage dataMsg);

};

#endif // CLIENTE_H
