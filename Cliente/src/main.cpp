/*
 * main.cpp
 *
 *  Created on: Apr 3, 2016
 *      Author: gonzalo
 */

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
	if(argc <= 2)
		std::string fileName = "default";
	//argv[1] es el pathdel archivo cliente.xml
	else
		std::string fileName(argv[1]);

	ParserCliente* parsersito = new ParserCliente();
    parsersito->parsearDocumento(fileName);
    string ip = parsersito->getConexionInfo().ip;
    int porto = parsersito->getConexionInfo().puerto;
    std::vector<Mensaje> listaDeMensajes = parsersito->getListaMensajes() ;

	cliente* client = new cliente(argc,ip,porto, listaDeMensajes);
	Menu* menu = new Menu();

	MostrarMenuDesconectado:
	bool conectado = true;

	while(conectado)
	{
		conectado = client->isConnected();
		//cout << conectado; Para debug para ver si esta conectado el client o no
		std::string eleccion = menu->menuzazo(conectado, listaDeMensajes);
		int comando = menu->cmpOptionMenu(eleccion, conectado, listaDeMensajes.size()+4);
		if(comando == 1 and !conectado)//Me quiero conectar y estoy desconectado
			if (!client->conectar())
				{
					printf("No se pudo establecer conexión con el servidor.\n");
					conectado = true;
				}
			else
			{
				conectado = true;
			}
		else if((comando == 1 and conectado)or(comando == 2 and !conectado)or(comando == 3 and !conectado) )
		//me quiero conectar pero ya estoy conectado, me quiero desconectar pero no estoy conectado
		//Quiero ciclar o mandar mensajes pero estoy desconectado
		{
			conectado = true;
		}
		else if(comando == 3 and conectado)//Mando un mensaje y estoy conectado
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
			}
			else
				conectado =true;
		}
		else if (comando == 2 and conectado)
		{
			client->desconectar();
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

