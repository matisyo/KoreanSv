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
	std::string fileName(argv[1]);

	ParserCliente* parsersito = new ParserCliente();
    parsersito->parsearDocumento(fileName);
    string ip = parsersito->getConexionInfo().ip;
    int porto = parsersito->getConexionInfo().puerto;
    std::vector<Mensaje> listaDeMensajes = parsersito->getListaMensajes() ;


	cliente* client = new cliente(argc,ip,porto, listaDeMensajes);
    char eleccion = 0;

    while(eleccion != '1'){
	    cout << "1 - Para conectar \n";
	    cout << "2 - Para desconectar \n";
   	    int i = 0;
   	    for(i;i< listaDeMensajes.size();i++)
	  	{
   	    	cout << 3+i << " - Mensaje "
   	    		 << listaDeMensajes[i].id << ", "
   	    		 << listaDeMensajes[i].tipo << ", "
   	    		 << listaDeMensajes[i].valor << "\n";
	   	 }
	   	cout << 3+i << " - Salir \n";
		int salida = (3+i);
		cout << (4+i) << " - Ciclar \n";
		char salidaEnChar = salida + '0';
		char ciclarEnChar = (salida+1) + '0';
	   	cin >> eleccion;

	    if(eleccion == '1')
	   	    	cout << "Se va a conectar al servidor \n";
	    else if (eleccion == '2')
	   	    	cout << "No esta conectado a ningún servidor. No se ha podido desconectar \n";
	    else if(eleccion == salidaEnChar)
   	    {
	    	cout << "Se cerrará el cliente \n";
	   	   	client->cerrarSoket();
	   	   	Logger::Instance()->Close();
	   	   	//delete client;
	   	   	return 0;
	     }
  	    else if((eleccion >= '3' and eleccion < salidaEnChar) or eleccion == ciclarEnChar )
  	    	cout << "No se puede enviar un mensaje sin estar conectado \n";
  	    else
	       	cout << "Ingreso un comando inválido";
	 }
	client->conectar();
	client->leer();

	while(client->checkConection())
	{
		Mensaje mensajeAEnviar = listaDeMensajes[0];//Mensaje para que compile declarado, se va a cambiar
		//Si o si cuando se elija un mensaje valido del vector de Mensajes del cliente
		bool condicion = true;
		char option;
		while(condicion){
			cout << "Opciones: \n";
			cout << "1 - Conectar \n";
			cout << "2 - Desconectar \n";
			int i = 0;
			for(i;i< listaDeMensajes.size();i++)
			{
				cout << 3+i << " - Enviar el Mensaje "
					 << listaDeMensajes[i].id << "\n";
			}
			cout << 3+i << " - Salir \n";
			cout << 4+i << " - Ciclar \n";
			cin >> option;
			int salida = (3+i);
			char salidaEnChar = salida + '0';
			char ciclarEnChar = (salida+1) + '0';

			if(option == '1')
	   	    	cout << "Ya esta conectado al servidor  \n";
	   	    else if (option == '2')
	   	    	cout << "Se va a desconectar del servidor \n";
	   	    else if(option == salidaEnChar)
	   	    {
	   	    	cout << "Se cerrará el cliente \n";
	   	    	client->cerrarSoket();
	   	    	Logger::Instance()->Close();
	   	    	//delete client;
	   	    	return 0;
	   	    }
	   	    else if(option >= '3' and option < salidaEnChar )
	   	    {
	   	    	cout << "Se enviara el mensaje :" << (int)option -48 << "\n";
	   	    	int indice = (int)option - 51;
	   	    	mensajeAEnviar = listaDeMensajes[indice];
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
	   	    	cout << "Ingreso un comando inválido";
	   	}
		client->escribir(mensajeAEnviar);
		client->leer();
	}
	client->cerrarSoket();

	Logger::Instance()->Close();
	delete client;
    return 0;

}

void wait (int segundos)
{
	clock_t endwait;
	endwait = clock() + segundos*CLOCKS_PER_SEC;
	while(clock() < endwait){}
}

