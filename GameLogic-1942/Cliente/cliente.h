#ifndef CLIENTE_H
#define CLIENTE_H
#include "../Utils/Logger.h"
#include "../Cliente/mensije.h"
#include "../Cliente/multiqueue.h"
#include "AlanTuring.h"
#include "MessageFactory.h"
#include "../Utils/Timer.h"
#include "../Utils/TiposDefinidos.h"
#include "../Game.h"
#include "../Vector2D.h"

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

#define TIMEOUT_SECONDS 10
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
        bool isConnecting();
        bool checkServerConnection();

        //timeout
        void *sendTimeOuts(void);
        void createTimeoutThread();
        static void *timeOut_method(void *context);
        bool sendTimeOutMsg();

    private:
        AlanTuring* m_alanTuring;
        int sockfd, portno, n;
        bool m_connected;
        bool m_connecting;
        Timer* serverTimeOut;
        Timer* sendTimeOutTimer;
        pthread_t timeOutThread;
        pthread_mutex_t  m_readingMutex;
        pthread_mutex_t  m_writingMutex;
        pthread_cond_t   m_condv;

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
