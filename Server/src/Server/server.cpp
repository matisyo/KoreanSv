#include "server.h"


server::server(int port, int maxC): MAX_CLIENTES(maxC)
{
	m_svRunning = true;
	m_alanTuring = new AlanTuring();

    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_condv, NULL);
    //Creo Socket
    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
         if (sockfd < 0)
            error("Error: No se pudo crear el socket");

    // Limpio el struct tod o en 0file:///home/matias/Desktop/Taller/server.h

     bzero((char *) &serv_addr, sizeof(serv_addr));

    /*Seteo la gilada basica del server*/
     serv_addr.sin_family = AF_INET;

     //Setea el adress del srv
     serv_addr.sin_addr.s_addr = INADDR_ANY;

     //Transforma el int por el tema del endianness
     serv_addr.sin_port = htons(port);

     //Por ultimo enlazo el socket
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
              error("Server: No se enlazo correctamente el socket del server.");

    m_clientNum = 0;
    m_listaDeClientes.resize(MAX_CLIENTES);
    m_clientThreads.resize(MAX_CLIENTES);
    m_queuePost.resize(MAX_CLIENTES);
    m_clientResponseThreads.resize(MAX_CLIENTES);
    startThread();
    printf("Bienvenido a servu\n");
}

server::~server()
{
	closeAllsockets();
	m_listaDeClientes.clear();
	m_queuePost.clear();

	delete m_alanTuring;
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_condv);
}
void server::error(const char *msg)
{
	Logger::Instance()->LOG(msg, ERROR);
	Logger::Instance()->Close();
    exit(1);
}

void server::escuchar()
{
	int success = 0;
    success = listen(sockfd, MAX_CLIENTES);
    if (success < 0)
    {
    	Logger::Instance()->LOG("Server: El server no se pudo configurar satisfactoriamente", ERROR);
    	exit(-1);
    }
}
void server::aceptar(){
    //Aca el accept va a pisar el cli_addr y este nuevo es el sokete que lo relaciona a ese cliente
    //Deberia meter el nuevo thread por aca
	socklen_t clilen = sizeof(cli_addr);
    if(getNumClientes() < MAX_CLIENTES){
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
        Mensaje connectedMessage = MessageFactory::Instance()->createMessage("mensajeConeccion", "Conectado al Server\n", msgConnected);
        sendMsg(newsockfd, connectedMessage);
    	//send(newsockfd, "Server O K   \n", 21, 0);
    }else{
    	//Informa al socket solicitante, que el server ya no posee capacidad
    	newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
        Mensaje exitMessage = MessageFactory::Instance()->createMessage("servidorLLeno", "El Servidor está lleno\n", msgExit);
        sendMsg(newsockfd, exitMessage);
    	//send(newsockfd, "exit\n", 6, 0);
    	close(newsockfd);
    	sleep(1);
    	return;
	}
    if (newsockfd < 0)
    {
    	std::stringstream ss;
    	ss <<"Server: No se pudo aceptar al cliente " << inet_ntoa(cli_addr.sin_addr);
    	Logger::Instance()->LOG(ss.str(), WARN);
    	return;
    }
    else{
    	//Crea el cliente
    	m_lastID = m_listaDeClientes.add(newsockfd);
    	if (m_lastID < 0)
    	{
    		Logger::Instance()->LOG("Server: Cliente rechazado. El servidor no puede aceptar más clientes.", WARN);
    		return;
    	}
    	printf("se agrego en la posicion %d \n", m_lastID);

    	pthread_create(&m_clientThreads[m_lastID], NULL, &server::mati_method, (void*)this);
    	pthread_create(&m_clientResponseThreads[m_lastID], NULL, &server::mati_method3, (void*)this);

    	aumentarNumeroClientes();


    	printf("Se creo un thread %d\n", m_lastID);
    	std::stringstream ss;
    	ss << "Server: Se acepto el cliente: " << inet_ntoa(cli_addr.sin_addr);
    	Logger::Instance()->LOG(ss.str(), DEBUG);
	}

}


void server::escribir(int id)
{

    //send(m_listaDeClientes.getElemAt(id), "Llego correctamente!\n", 21, 0);
}

void server::sendMsg(int socketReceptor, Mensaje msg)
{
	char bufferEscritura[MESSAGE_BUFFER_SIZE];
	int msgLength = m_alanTuring->encodeXMLMessage(msg, bufferEscritura);
	send(socketReceptor,bufferEscritura , msgLength, 0);
}

bool server::leer(int id)
{
    //Reseteo el buffer que se va a completar con nuevos mensajes
    bzero(buffer,256);
    int n = recv(m_listaDeClientes.getElemAt(id), buffer, 255, 0);

    if (!lecturaExitosa(n, id))
    	return false;

    printf("Se leyeron %d bytes.\n", n);
    int messageLength = (int)m_alanTuring->decodeLength(buffer);

    //loopea hasta haber leido la totalidad de los bytes necarios
    while (n < messageLength)
    {
    	n = recv(m_listaDeClientes.getElemAt(id), buffer, 255, 0);
    	printf("Se leyeron %d bytes.", n);
        if (!lecturaExitosa(n, id))
        	return false;
    }

    NetworkMessage netMsgRecibido = m_alanTuring->decode(buffer);

    ServerMessage serverMsg;
    serverMsg.clientID = id;
    serverMsg.networkMessage = netMsgRecibido;
 /*string my_str2 (buffer);
    mensije msg;
    msg.id = id;
    msg.texto = my_str2;*/

    m_queue.add(serverMsg);
    printf("Se agrego un mensaje a la cola satisfactoriamente.\n");
    return true;
}

void* server::procesar(void)
{

	while(this->checkStatus())
	{

			if (m_queue.size() != 0)
			{
				ServerMessage serverMsg = m_queue.remove();
				DataMessage dataMessage = m_alanTuring->decodeMessage(serverMsg.networkMessage);
				string messageID(dataMessage.msg_ID);

				// BLOQUE DE PROCESAMIENTO
				bool mensajeValido = procesarMensaje(serverMsg);

				if (!mensajeValido)
				{
					std::stringstream ss;
					ss << "El Mensaje con ID: " << messageID.c_str() << " fue rechazado.";
					Logger::Instance()->LOG(ss.str(), DEBUG);
					m_alanTuring->setNetworkMessageStatus(&serverMsg.networkMessage, 'I');

				}
				printf("mensaje valido\n");
				std::stringstream ss;
				ss << "El Mensaje con ID: " << messageID.c_str() << " fue procesado correctamente.";
				Logger::Instance()->LOG(ss.str(), DEBUG);
				m_alanTuring->setNetworkMessageStatus(&serverMsg.networkMessage, 'V');

				//FUNCION ANTIGUA
				/*string buf = msg.texto;
				printf("%s \n",buf.c_str());
				printf("comparacion %d\n",strcmp(buf.c_str(),"KillSv\n"));
				if (strcmp(buf.c_str(),"KillSv\n") == 0 ){
					m_svRunning = false;
					printf("consola: ");
				}
				printf("%d: %s \n",msg.id + 1,msg.texto.c_str());*/

				// BLOQUE DE PROCESAMIENTO
			    printf("Se agrego el mensaje a la cola de mensajes procesados.\n");
			    std::vector<int> vec_de_ids_aux = m_listaDeClientes.getAllClientsID();
			    for(int loco:vec_de_ids_aux){
			    	m_queuePost[loco].add(serverMsg);
			    }
			}

	}
}
void* server::postProcesamiento(void)
{
	int id = this->m_lastID;

	bool ciclar = true;
	while(ciclar)
	{

			if (m_queuePost[id].size() != 0)
			{
				printf("%d\n",m_queuePost[id].size());
				ServerMessage msg = m_queuePost[id].remove();
				char bufferEscritura[MESSAGE_BUFFER_SIZE];
				int msgLength = m_alanTuring->encodeNetworkMessage(msg.networkMessage, bufferEscritura);
				send(m_listaDeClientes.getElemAt(id),bufferEscritura , msgLength, 0);

			}

	}
}

void *server::newDialog(void)
{
    int id = this->m_lastID;
    while(true)
    {
    	if (!this->leer(id))
    		break;
        this->escribir(id);
	}
    pthread_exit(NULL);

}
// nunca deberais desafiar el poder del matimethod...o sera muy tarde...fuera de joda no tokes nada aca gil ah si? ven aqui
void *server::mati_method(void *context)
{
	return ((server *)context)->newDialog();
}
void *server::mati_method2(void *context)
{
	return ((server *)context)->procesar();
}
void *server::mati_method3(void *context)
{
	return ((server *)context)->postProcesamiento();
}


void server::startThread()
{
	pthread_create(&threadDeProcesos, NULL, mati_method2,(void*)this );
}

void server::closeAllsockets()
{
    //Cuando el Server tenga lista de threads hay q recorrer cerrando
	for (int i = 0; i < m_listaDeClientes.size(); i++)
	{
		closeSocket(i);
	}
    //close(newsockfd);
    close(sockfd);
}

void server::closeSocket(int id)
{
	Logger::Instance()->LOG("Server: Se desconectó un cliente.", DEBUG);
	reducirNumeroClientes();
	printf("Se desconectó un cliente, hay lugar para un chaval mas.\n");
	close(m_listaDeClientes.getElemAt(id));
	m_listaDeClientes.removeAt(id);

	//terminar thread
}

void server::aumentarNumeroClientes()
{
    pthread_mutex_lock(&m_mutex);
    m_clientNum++;
    pthread_cond_signal(&m_condv);
    pthread_mutex_unlock(&m_mutex);
}
void server::reducirNumeroClientes()
{
    pthread_mutex_lock(&m_mutex);
    m_clientNum--;
    pthread_cond_signal(&m_condv);
    pthread_mutex_unlock(&m_mutex);
}
const int server::getNumClientes()
{
    pthread_mutex_lock(&m_mutex);
    int cant = m_clientNum;
    pthread_mutex_unlock(&m_mutex);
    return cant;
}

const int server::getMaxClientes()
{
	return MAX_CLIENTES;
}
bool server::checkStatus()
{
	return m_svRunning;
}

bool server::lecturaExitosa(int bytesLeidos, int clientID)
{
    if (bytesLeidos < 0)
    {
    	//Cliente Desconectado
    	closeSocket(clientID);
    	printf("leyo -1 el recv\n");
    	return false;

    }
    if (bytesLeidos == 0)
    {
    	//Cliente Desconectado. Hay diferencias con recibir -1? Sino lo ponemos to do junto, hacen lo mismo
    	closeSocket(clientID);
    	printf("leyo 0 el recv\n");
    	return false;
    }
    return true;
}

bool server::procesarMensaje(const ServerMessage serverMsg)
{
	bool procesamientoExitoso = true;;
	NetworkMessage netMsg = serverMsg.networkMessage;
	DataMessage dataMsg = m_alanTuring->decodeMessage(netMsg);
	std::string stringValue(dataMsg.msg_value);
	printf("Procesando mensaje con ID: %s \n", dataMsg.msg_ID);

	//int msg
	if ((netMsg.msg_Code[0] == 'i') && (netMsg.msg_Code[1] == 'n') && (netMsg.msg_Code[2] == 't'))
	{
		procesamientoExitoso = (StringHelper::validateInt(stringValue));
	}
	//char msg
	if ((netMsg.msg_Code[0] == 'c') && (netMsg.msg_Code[1] == 'h') && (netMsg.msg_Code[2] == 'r'))
	{
		procesamientoExitoso = (StringHelper::validateChar(stringValue));
	}
	//char msg
	if ((netMsg.msg_Code[0] == 'd') && (netMsg.msg_Code[1] == 'b') && (netMsg.msg_Code[2] == 'l'))
	{
		procesamientoExitoso = (StringHelper::validateDouble(stringValue));
	}
	//Por ahora no hay validacion para valores string
	return procesamientoExitoso;
}
