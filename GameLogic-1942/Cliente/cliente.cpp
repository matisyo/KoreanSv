#include "cliente.h"

using namespace std;

void cliente::error(const char *msg)
{
	Logger::Instance()->LOG(msg, ERROR);
	Logger::Instance()->Close();
    exit(0);
}

bool cliente::conectar()
{
	if (m_connecting)
		return true;

	m_connecting = true;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
    	Logger::Instance()->LOG("Cliente: Error en la creación del socket", ERROR);
    	m_connecting = false;
    	return false;
    }

    setTimeOut();

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
    	Logger::Instance()->LOG("Cliente: El cliente no se pudo conectar satisfactoriamente", WARN);
    	m_connecting = false;
       return false;
    }
    m_connected = true;
	leer();

	if (m_connected)
	{
		serverTimeOut->Reset();
		sendTimeOutTimer->Reset();
		serverTimeOut->Start();
		sendTimeOutTimer->Start();
		//createTimeoutThread();
	}
	else
	{
		cerrarSoket();
	}
	m_connecting = false;
    return m_connected;
}
void cliente::desconectar()
{
	if (!m_connected)
		return;
	m_connected = false;
	serverTimeOut->Stop();
	cerrarSoket();
	Logger::Instance()->LOG("Cliente: El cliente se ha desconectado satisfactoriamente", DEBUG);
}

cliente::cliente(int argc, string ip, int port, std::vector<Mensaje> listaDeMensajesCargados){
	m_connected = false;
	m_connecting = false;
	m_alanTuring = new AlanTuring();

    pthread_mutex_init(&m_readingMutex, NULL);
    pthread_mutex_init(&m_writingMutex, NULL);
    pthread_cond_init(&m_condv, NULL);

    portno = port;
    /*sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    	error("Cliente: Error en la creación del socket");*/
    const char* ip1 = ip.c_str();
    server = gethostbyname(ip1);
    if (server == NULL) {
    	error("Cliente: No se pudo encontrar el servidor ingresado");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    listaDeMensajes = listaDeMensajesCargados;

    serverTimeOut = new Timer();
    sendTimeOutTimer = new Timer();


}
cliente::~cliente()
{
    pthread_mutex_destroy(&m_readingMutex);
    pthread_mutex_destroy(&m_writingMutex);
    pthread_cond_destroy(&m_condv);
	delete serverTimeOut;
	delete sendTimeOutTimer;

	delete m_alanTuring;
}

void cliente::escribir(Mensaje mensaje)
{
	/*
    char bufferEscritura[MESSAGE_BUFFER_SIZE];
    bzero(bufferEscritura,MESSAGE_BUFFER_SIZE);
    int msgLength = m_alanTuring->encodeXMLMessage(mensaje, bufferEscritura);
    printf("Se deben enviar %d bytes \n", msgLength);
    n = send(sockfd, bufferEscritura, msgLength, 0);
    printf("Se enviaron %d bytes \n", n);
    if (n < 0)
    	Logger::Instance()->LOG("Cliente: No se pudo enviar el mensaje.", WARN);
	*/
	sendMsg(mensaje);
}

void cliente::sendMsg(Mensaje msg)
{
	char bufferEscritura[MESSAGE_BUFFER_SIZE];
	int msgLength = m_alanTuring->encodeXMLMessage(msg, bufferEscritura);
	char *ptr = (char*) bufferEscritura;

    while (msgLength > 0)
    {
        int bytesEnviados = send(sockfd, ptr, msgLength, 0);
        if (bytesEnviados < 1)
        {
        	Logger::Instance()->LOG("Server: No se pudo enviar el mensaje.", WARN);
        	return;
        }
        ptr += bytesEnviados;
        msgLength -= bytesEnviados;
    }

	/*int bytesEnviados = send(socketReceptor,bufferEscritura , msgLength, 0);
    if (bytesEnviados <= 0)
    	Logger::Instance()->LOG("Server: No se pudo enviar el mensaje.", WARN);
    while (bytesEnviados < msgLength)
    {
    	bytesEnviados =  send(sockfd, bufferEscritura + bytesEnviados , msgLength, 0);
        if (bytesEnviados <= 0)
        	Logger::Instance()->LOG("Server: No se pudo enviar el mensaje.", WARN);

    }*/

    if (msg.tipo.compare("timeoutACK") != 0)
    {
		std::stringstream ss;
		ss << "Cliente: Se ha enviado con éxito el mensaje con ID: " << msg.id.c_str() << ".";
		Logger::Instance()->LOG(ss.str(), DEBUG);
    }
}

//Envia mensaje de timeOut cada x tiempo
bool cliente::sendTimeOutMsg()
{
	pthread_mutex_lock(&m_writingMutex);
	if (!checkServerConnection())
		return false;

	if ((float)sendTimeOutTimer->GetTicks()/CLOCKS_PER_SEC >= 1)
	{
		Mensaje timeOutMsg = MessageFactory::Instance()->createMessage("to", "",msgTimeOutACK);
		sendMsg(timeOutMsg);

		sendTimeOutTimer->Reset();
		//espera procesamiento del server
		leer();
	}
	pthread_mutex_unlock(&m_writingMutex);
	return true;

}


bool cliente::checkServerConnection()
{
	//printf("Timer del server = %f\n", (float)serverTimeOut->GetTicks()/CLOCKS_PER_SEC);
	if (((float)serverTimeOut->GetTicks()/CLOCKS_PER_SEC >= TIMEOUT_SECONDS) || (m_connected == false))
	{
		printf("Se perdio conexión con el servidor.\n");
		desconectar();
		printf("Presione 1 para reconectar.\n");
		return false;
	}
	return true;
}


bool cliente::leer()
{
    pthread_mutex_lock(&m_readingMutex);
    bzero(buffer,256);
    n = recv(sockfd, buffer, 255, 0);
    /*string buf = buffer;
   if (strcmp(buf.c_str(),"exit\n") == 0){
	   m_conected = false;
   }*/
   if (!lecturaExitosa(n))
   {
	   //Se perdio la conexion con el server
	   return false;
   }
   int messageLength = (int)m_alanTuring->decodeLength(buffer);
   //loopea hasta haber leido la totalidad de los bytes necarios
   while (n < messageLength)
   {
	   n = recv(sockfd, buffer, 255, 0);

       if (!lecturaExitosa(n))
       {
    	   //se perdio la conexion con el server
    	  return false;
       }
   }
   pthread_cond_signal(&m_condv);
   pthread_mutex_unlock(&m_readingMutex);
   //llego el mensaje bien
   serverTimeOut->Reset();

   NetworkMessage netMsgRecibido = m_alanTuring->decode(buffer);
   procesarMensaje(netMsgRecibido);

   return true;

}
bool cliente::isConnected(){
	return m_connected;
}
bool cliente::isConnecting(){
	return m_connecting;
}
void cliente::cerrarSoket()
{
    close(sockfd);
}

void cliente::setTimeOut()
{
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SECONDS;
    timeout.tv_usec = TIMEOUT_MICROSECONDS;

    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
    	Logger::Instance()->LOG("Cliente: No se pudo setear un timeout en la conexion con el servidor.", WARN);
    }

    if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
    	Logger::Instance()->LOG("Cliente: No se pudo setear un timeout en la conexion con el servidor.", WARN);
    }
}

void cliente::procesarMensaje(NetworkMessage networkMessage)
{
	DataMessage dataMsg = m_alanTuring->decodeMessage(networkMessage);

	//LA IDEA ES HACER MENSAJES CALCULARLOS Y DEVOLVER LA POS DEL OBJETO
	//por ahroa esta echo solo para player asike paso el vec dps la mejoramos si anda
	//EJ update(player,ID,vec,img)
	//EJ update(enemy,ID,vec,img)
	//EJ update(proyectil,ID,vec,img)
	Vector2D* mov;
	if ((networkMessage.msg_Code[0] == 'm') && (networkMessage.msg_Code[1] == 'v') && (networkMessage.msg_Code[2] == 'x'))
	{
		printf("ACA: %s HEHE %s JOJO",dataMsg.msg_value,networkMessage.msg_Code);
		mov = new Vector2D(stof(dataMsg.msg_value),0);
		Game::Instance()->update(mov);
	}
	if ((networkMessage.msg_Code[0] == 'm') && (networkMessage.msg_Code[1] == 'v') && (networkMessage.msg_Code[2] == 'y'))
	{
		printf("aca: %s jeje %s jojo",dataMsg.msg_value,networkMessage.msg_Code);
		mov = new Vector2D(0,stof(dataMsg.msg_value));
		Game::Instance()->update(mov);
	}

	//string msg
	if ((networkMessage.msg_Code[0] == 's') && (networkMessage.msg_Code[1] == 't') && (networkMessage.msg_Code[2] == 'r'))
	{
		if (!validarMensaje(dataMsg))
			return;

		printf("Tipo de Mensaje: string \n");
		string valorMensaje(dataMsg.msg_value);
		printf("Valor del Mensaje: %s \n", valorMensaje.c_str());
	}
	if ((networkMessage.msg_Code[0] == 'i') && (networkMessage.msg_Code[1] == 'n') && (networkMessage.msg_Code[2] == 't'))
	{
		if (!validarMensaje(dataMsg))
			return;
		printf("Tipo de Mensaje: int \n");
		string valorMensaje(dataMsg.msg_value);
		int valorInt = stoi(dataMsg.msg_value);
		printf("Valor del Mensaje: %d \n", valorInt);
	}
	if ((networkMessage.msg_Code[0] == 'd') && (networkMessage.msg_Code[1] == 'b') && (networkMessage.msg_Code[2] == 'l'))
	{
		if (!validarMensaje(dataMsg))
			return;
		printf("Tipo de Mensaje: double \n");
		string valorMensaje(dataMsg.msg_value);
		double valorDouble = stod(dataMsg.msg_value);
		printf("Valor del Mensaje: %f \n", valorDouble);
	}
	if ((networkMessage.msg_Code[0] == 'c') && (networkMessage.msg_Code[1] == 'h') && (networkMessage.msg_Code[2] == 'r'))
	{
		if (!validarMensaje(dataMsg))
			return;
		printf("Tipo de Mensaje: char \n");
		string valorMensaje(dataMsg.msg_value);
		char valorChar  = valorMensaje.at(0);
		printf("Valor del Mensaje: %s \n", valorMensaje.c_str());
	}
	if ((networkMessage.msg_Code[0] == 'c') && (networkMessage.msg_Code[1] == 'n') && (networkMessage.msg_Code[2] == 't'))
	{
		//El cliente se conecto con exito.
		printf("Conección con el server exitosa. \n");
		Logger::Instance()->LOG("Cliente: Conección al servidor exitosa.\n", DEBUG);
		//string valorMensaje(dataMsg.msg_value);
		//char valorChar = valorMensaje.at(0);
		//printf("Valor del Mensaje: %c \n", valorChar);
	}
	if ((networkMessage.msg_Code[0] == 'e') && (networkMessage.msg_Code[1] == 'x') && (networkMessage.msg_Code[2] == 't'))
	{
		//El cliente fue pateado
		desconectar();
		printf("El cliente ha sido desconectado del server.\n");
		Logger::Instance()->LOG("Cliente: El cliente ha sido desconectado del server.", DEBUG);
		//string valorMensaje(dataMsg.msg_value);
		//char valorChar = valorMensaje.at(0);
		//printf("Valor del Mensaje: %c \n", valorChar);
	}
	if ((networkMessage.msg_Code[0] == 'f') && (networkMessage.msg_Code[1] == 'u') && (networkMessage.msg_Code[2] == 'l'))
	{
		//El server esta lleno. Patear
		desconectar();
		m_connected = false;

		printf("El servidor está lleno.\n");
		Logger::Instance()->LOG("Cliente: No se pudo conectar al servidor. El servidor está lleno.", DEBUG);
	}
	if ((networkMessage.msg_Code[0] == 't') && (networkMessage.msg_Code[1] == 'm') && (networkMessage.msg_Code[2] == 'o'))
	{
		//TimeOut ACK, lo dej opor si en el futuro queremos hacer algo extra
		//printf("Llego un time Out ACK\n");
	}
}

bool cliente::validarMensaje(DataMessage dataMsg)
{
	bool mensajeValido = true;
	string messageID(dataMsg.msg_ID);
	std::stringstream ss;
	if ((dataMsg.msg_status == '-') || (dataMsg.msg_status == 'I'))
	{
		mensajeValido = false;
		ss.clear();
		ss << "El Mensaje con ID: " << messageID.c_str() << " fue rechazado.";
		Logger::Instance()->LOG(ss.str(), DEBUG);
		ss << "\n";
		printf("%s", ss.str().c_str());

	}
	if (dataMsg.msg_status == 'V')
	{
		mensajeValido = true;
		ss.clear();
		ss << "El Mensaje con ID: " << messageID.c_str() << " fue procesado correctamente.";
		Logger::Instance()->LOG(ss.str(), DEBUG);
		ss << "\n";
		printf("%s", ss.str().c_str());
	}
	return mensajeValido;
}
void *cliente::sendTimeOuts(void)
{

    while(false)
    {
    	if (!sendTimeOutMsg())
    		break;
	}
    pthread_exit(NULL);

}
void *cliente::timeOut_method(void *context)
{
	return ((cliente *)context)->sendTimeOuts();
}
void cliente::createTimeoutThread(){

	pthread_create(&timeOutThread, NULL, &cliente::timeOut_method, (void*)this);
}

bool cliente::lecturaExitosa(int bytesLeidos)
{
    if (n < 0)
    {
    	//Se perdio la coneccion con el server
    	desconectar();
    	return false;
    }
	if (n == 0){
		//Se perdio la coneccion con el server
		desconectar();
		return false;
    }
	return true;
}
