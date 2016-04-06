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
#include <unistd.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	//argv[1] es el pathdel archivo cliente.xml
	std::string fileName(argv[1]);

	ParserCliente* parsersito = new ParserCliente();
    parsersito->parsearDocumento(fileName);
    string ip = parsersito->getConexionInfo().ip;
    int porto = parsersito->getConexionInfo().puerto;
    std::vector<Mensaje> listaDeMensajes = parsersito->getListaMensajes() ;

	cliente* client = new cliente(argc,ip,porto, listaDeMensajes);

    MostrarMenu:
	cout << "\n";
	char eleccion = 0;

    while(eleccion != '1'){
	    cout << "1 - Para conectar \n";
	    cout << "2 - Para desconectar \n";
   	    int i = 0;
   	    for(;i< listaDeMensajes.size();i++)
	  	{
   	    	cout << 3+i << " - Enviar el Mensaje " << listaDeMensajes[i].id << "\n";
	   	 }
   	    cout << (3+i) << " - Ciclar \n";
	   	cout << 4+i << " - Salir \n";
		int salida = (4+i);

		char salidaEnChar = salida + '0';
		char ciclarEnChar = (salida-1) + '0';
	   	cin >> eleccion;
	   	cout << "\n";
	    if(eleccion == '1')
	   	    	cout << "Conectando con el servidor...\n";
	    else if (eleccion == '2')
	   	    	cout << "No esta conectado a ningún servidor. No se ha podido desconectar.\n";
	    else if(eleccion == salidaEnChar)
   	    {
   	    	cout << "Se ha cerrado el cliente.\n";
	   	   	client->cerrarSoket();
	   	   	Logger::Instance()->Close();
	   	   	delete client;
	   	   	return 0;
	     }
  	    else if(eleccion >= '3' and eleccion <= ciclarEnChar)
  	    	cout << "No se puede enviar un mensaje sin estar conectado \n";
  	    else
	       	cout << "Por favor, ingrese un comando válido. \n";
	 }

	if (!client->conectar())
	{
		printf("No se pudo establecer conexión con el servidor.\n");
		goto MostrarMenu;
	}
	client->leer();


	while(client->isConnected())
	{
		Mensaje mensajeAEnviar;//Mensaje para que compile declarado, se va a cambiar
		//Si o si cuando se elija un mensaje valido del vector de Mensajes del cliente
		bool condicion = true;
		char option;
		cout << "\n";
		while(condicion){
			cout << "Opciones: \n";
			cout << "1 - Conectar \n";
			cout << "2 - Desconectar \n";
			int i = 0;
			for(;i < listaDeMensajes.size();i++)
			{
				cout << 3+i << " - Enviar el Mensaje "
					 << listaDeMensajes[i].id << "\n";
			}

			cout << 3+i << " - Ciclar \n";
			cout << 4+i << " - Salir \n";
			cin >> option;
			int salida = (4+i);
			char salidaEnChar = salida + '0';
			char ciclarEnChar = (salida-1) + '0';
		   	cout << "\n";

			if(option == '1')
	   	    	cout << "Ya esta conectado al servidor. \n";
	   	    else if (option == '2')
	   	    {
	   	    	client->desconectar();
	   	    	cout << "El cliente se ha desconectado del servidor. \n";
	   	    	goto MostrarMenu;
	   	    }
	   	    else if(option == salidaEnChar)
	   	    {
	   	    	client->cerrarSoket();
	   	    	cout << "Se ha cerrado el cliente.\n";
	   	    	Logger::Instance()->Close();
	   	    	//delete client;
	   	    	return 0;
	   	    }
	   	    else if(option >= '3' and option < ciclarEnChar )
	   	    {
	   	    	cout << "Se enviara el mensaje :" << (int)option -48 << "\n";
	   	    	int indice = (int)option - 51;
	   	    	mensajeAEnviar = listaDeMensajes[indice];
	   			client->escribir(mensajeAEnviar);
	   			client->leer();
	   	    	condicion = false;
	   	    }
	   	    else if(option == ciclarEnChar)
	   	    {
	   	    	unsigned int frecuencia;
	   	    	cout << "Ingrese el tiempo deseado en milisegundos: \n";
	   	    	cin >> frecuencia;
	   	    	cout << " \n";
	   	    	int freq;
	   	    	for(int counter = 0; counter< listaDeMensajes.size();counter++)
	   	    	{
	   	   			cout << "Se enviará el mensaje: "
	   	   			<< listaDeMensajes[counter].id << "\n";
	   	   			client->escribir(listaDeMensajes[counter]);
	   	   			client->leer();
	   	   			freq = frecuencia*1000;
	   	   			usleep(freq);
	   	    	}
	   	    }
	   	    else
		       	cout << "Por favor, ingrese un comando válido. \n";
	   	}
	}
	client->cerrarSoket();

	Logger::Instance()->Close();
	delete client;
    return 0;

}


