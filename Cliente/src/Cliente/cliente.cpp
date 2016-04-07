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
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
    	Logger::Instance()->LOG("Cliente: Error en la creación del socket", ERROR);
    	return false;
    }

    setTimeOut();

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
    	Logger::Instance()->LOG("Cliente: El cliente no se pudo conectar satisfactoriamente", WARN);
       return false;
    }
    m_connected = true;
	leer();
    return m_connected;
}
void cliente::desconectar()
{
	m_connected = false;
	cerrarSoket();
	Logger::Instance()->LOG("Cliente: El cliente se ha desconectado satisfactoriamente", DEBUG);
}

cliente::cliente(int argc, string ip, int port, std::vector<Mensaje> listaDeMensajesCargados){
	m_connected = false;
	m_alanTuring = new AlanTuring();

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

}
cliente::~cliente()
{
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
	int n =  send(sockfd, bufferEscritura , msgLength, 0);
    if (n < 0)
    	Logger::Instance()->LOG("Cliente: No se pudo enviar el mensaje.", WARN);

	std::stringstream ss;
	ss << "Cliente: Se ha enviado con éxito el mensaje con ID: " << msg.id.c_str() << ".";
	Logger::Instance()->LOG(ss.str(), DEBUG);
}


bool cliente::leer()
{
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

   NetworkMessage netMsgRecibido = m_alanTuring->decode(buffer);
   procesarMensaje(netMsgRecibido);

   return true;

}
bool cliente::isConnected(){
	return m_connected;
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
