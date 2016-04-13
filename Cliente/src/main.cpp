
#include "Cliente/cliente.h"
#include <string>
#include <stdio.h>
#include "Utils/Logger.h"
#include "Utils/Parser/ParserCliente.h"
#include "Cliente/Menu.h"
#include <unistd.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{

	std::string fileName;
	if(argc < 2)
		fileName = "src/Utils/Default/cliente.xml";
	//argv[1] es el path del archivo cliente.xml
	else
		fileName.assign(argv[1]);

	ParserCliente* parsersito = new ParserCliente();
    parsersito->parsearDocumento(fileName);

	LoggerInfo loggerInfo = parsersito->getLoggerInfo();
	Logger::Instance()->setLoglevel(loggerInfo.debugAvailable, loggerInfo.warningAvailable, loggerInfo.errorAvailable);

    string ip = parsersito->getConexionInfo().ip;
    int porto = parsersito->getConexionInfo().puerto;
    std::vector<Mensaje> listaDeMensajes = parsersito->getListaMensajes() ;

	cliente* client = new cliente(argc,ip,porto, listaDeMensajes);
	Menu* menu = new Menu();

	bool conectado = true;

	while(conectado)
	{
		conectado = client->isConnected();

		//cout << conectado; Para debug para ver si esta conectado el client o no
		std::string eleccion = menu->menuzazo(client->isConnected(), listaDeMensajes);
		int comando = menu->cmpOptionMenu(eleccion, client->isConnected(), listaDeMensajes.size()+4);
		if(comando == 1 and !client->isConnected())//Me quiero conectar y estoy desconectado
		{
			if (client->isConnecting())
			{
				printf("Conectando. Aguarde por favor...\n");
				continue;
			}
			if (!client->conectar())
				{
					printf("No se pudo establecer conexión con el servidor.\n");
					conectado = true;
				}
			else
			{
				conectado = true;
			}
		}
		else if((comando == 1 and client->isConnected())or(comando == 2 and !client->isConnected())or(comando == 3 and !client->isConnected()) )
		//me quiero conectar pero ya estoy conectado, me quiero desconectar pero no estoy conectado
		//Quiero ciclar o mandar mensajes pero estoy desconectado
		{
			conectado = true;
		}
		else if(comando == 3 and client->isConnected())//Mando un mensaje y estoy conectado
		{
			int indice = stoi(eleccion) - 3;
			Mensaje mensajeAEnviar = listaDeMensajes[indice];
			cout << "Se enviara el mensaje :" << mensajeAEnviar.id << "\n";
			client->escribir(mensajeAEnviar);
			if (!client->leer())
			{
				Logger::Instance()->LOG("Cliente: Se ha perdido la conexión con el servidor. \n", WARN);
			   	printf("Se ha perdido la conexión con el servidor.\n");
			 }
		}
		else if(comando == 4)//Ciclo y estoy conectado
		{
			int lostConnection = menu->ciclar(listaDeMensajes, client);
			if (lostConnection == -1)
			{
				client->desconectar();
				continue;
			}
			else
				conectado =true;
		}
		else if (comando == 2 and client->isConnected())
		{
			client->desconectar();
			continue;
		}
		else if(comando == -1)
		{
			conectado = false;
		}
		else
			conectado = true;
	}
	client->desconectar();

	Logger::Instance()->Close();
	delete client;
    return 0;

}

