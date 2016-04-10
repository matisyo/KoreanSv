#ifndef CLIENTE_H
#define CLIENTE_H

#include "../Utils/Logger.h"
#include "../Cliente/mensije.h"
#include "../Cliente/multiqueue.h"
#include "AlanTuring.h"
#include "MessageFactory.h"
#include "../Utils/Timer.h"
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
#include <netdb.h>
#include <iostream>
#include <sys/time.h>

#define TIMEOUT_SECONDS 5
#define TIMEOUT_MICROSECONDS 0

class cliente
{
    public:
        cliente(int argc, string ip,int port, std::vector<Mensaje> listaDeMensajesCargados );
        ~cliente();
        bool conectar();
        void desconectar();
        void escribir(Mensaje mensaje);
        bool leer();
        bool isConnected();
        bool checkServerConnection();
        void *queContas(void);
        void hilo();
        static void *mati_method(void *context);
        void sendTimeOutMsg();

    private:
        AlanTuring* m_alanTuring;
        int sockfd, portno, n;
        bool m_connected;
        Timer* serverTimeOut;
        Timer* sendTimeOutTimer;
        pthread_t timeOutThread;

        struct sockaddr_in serv_addr;
        struct hostent *server;
        void error(const char *msg);
        std::vector<Mensaje> listaDeMensajes;

        char buffer[256];

        void cerrarSoket();
        void procesarMensaje(NetworkMessage networkMessage);
        void sendMsg(Mensaje msg);
        void setTimeOut();
        bool validarMensaje(DataMessage dataMsg);
        bool lecturaExitosa(int bytesLeidos);



};

#endif // CLIENTE_H
