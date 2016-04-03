#include "cliente.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
void cliente::error(const char *msg)
{
	Logger::Instance()->LOG(msg, ERROR);
    exit(0);
}

void cliente::conectar()
{
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
       error("Cliente: El cliente no se pudo conectar satisfactoriamente");
}
cliente::cliente(int argc, char *ip,char *port){
	m_conected = true;
	m_alanTuring = new AlanTuring();
    if (argc < 3) {
       fprintf(stderr,"usage hostname port\n");
       exit(0);
    }
    portno = atoi(port);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    	error("Cliente: Error en la creación del socket");
    server = gethostbyname(ip);// en caso de no ser ip si es ip anda igual
    if (server == NULL) {
    	error("Cliente: No se pudo encontrar el servidor ingresado");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);

}
cliente::~cliente()
{
	delete m_alanTuring;
}

void cliente::escribir()
{
	//deberia recogerlo del xml
	Mensaje mensaje;
	mensaje.id = "mensaje1";
	mensaje.tipo = "string";
    printf("Por favor. Ingrese valor mensaje: ");
    char buf[256];
    bzero(buf,256);
    fgets(buf,255,stdin);
    string msgIngresado(buf);
    mensaje.valor = msgIngresado;

    char bufferEscritura[MESSAGE_BUFFER_SIZE];
    bzero(bufferEscritura,MESSAGE_BUFFER_SIZE);
    int msgLength = m_alanTuring->encodeXMLMessage(mensaje, bufferEscritura);
    printf("Se deben enviar %d bytes \n", msgLength);
    n = send(sockfd, bufferEscritura, msgLength, 0);
    printf("Se enviaron %d bytes \n", n);
    if (n < 0)
    	Logger::Instance()->LOG("Cliente: No se pudo enviar el mensaje.", WARN);
}
void cliente::leer()
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
	   exit(-1);
   }
   int messageLength = (int)m_alanTuring->decodeLength(buffer);
   //loopea hasta haber leido la totalidad de los bytes necarios
   while (n < messageLength)
   {
	   n = recv(sockfd, buffer, 255, 0);

       if (!lecturaExitosa(n))
       {
    	   //se perdio la conexion con el server
    	   exit(-1);
       }
   }
   printf("Se leyeron %d bytes del mensaje ya procesado.\n", n);
   NetworkMessage netMsgRecibido = m_alanTuring->decode(buffer);
   mostrarMensaje(netMsgRecibido);

   printf("%s\n", buffer);
}
bool cliente::checkConection(){
	return m_conected;
}
void cliente::cerrarSoket()
{
    close(sockfd);
}

void cliente::mostrarMensaje(NetworkMessage networkMessage)
{
	DataMessage dataMsg = m_alanTuring->decodeMessage(networkMessage.msg_Data);
	string messageID(dataMsg.msg_ID);
	std::stringstream ss;
	if ((dataMsg.msg_status == '-') || (dataMsg.msg_status == 'I'))
	{
		ss.clear();
		ss << "El Mensaje con ID: " << messageID.c_str() << " fue rechazado.";
		Logger::Instance()->LOG(ss.str(), DEBUG);
		ss << "\n";
		printf("%s", ss.str().c_str());
	}
	if (dataMsg.msg_status == 'V')
	{
		ss.clear();
		ss << "El Mensaje con ID: " << messageID.c_str() << " fue procesado correctamente.";
		Logger::Instance()->LOG(ss.str(), DEBUG);
		ss << "\n";
		printf("%s", ss.str().c_str());
	}
	//string msg
	if ((networkMessage.msg_Code[0] == 's') && (networkMessage.msg_Code[1] == 't') && (networkMessage.msg_Code[2] == 'r'))
	{
		printf("Tipo de Mensaje: string \n");
		string valorMensaje(dataMsg.msg_value);
		char decoded[200];
		for (int i = 0; i < 3;i++)
			printf("char i: %c", dataMsg.msg_value[i]);
		//printf("Valor del Mensaje: %s \n", valorMensaje.c_str());
	}
	//int msg
	/*if ((networkMessage.msg_Code[0] == 'i') && (networkMessage.msg_Code[1] == 'n') && (networkMessage.msg_Code[2] == 't'))
	{
		printf("Tipo de Mensaje: int \n");
		string valorMensaje(dataMsg.msg_value);
		int valorInt = stoi(dataMsg.msg_value);
		printf("Valor del Mensaje: %s \n", valorMensaje.c_str());
	}*/
}

bool cliente::lecturaExitosa(int bytesLeidos)
{
    if (n < 0)
    {
    	//Se perdio la coneccion con el server
    	cerrarSoket();
    	return false;
    }
	if (n == 0){
		//Se perdio la coneccion con el server
		cerrarSoket();
		return false;
    }
	return true;
}
