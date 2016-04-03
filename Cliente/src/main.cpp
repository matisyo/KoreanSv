/*
 * main.cpp
 *
 *  Created on: Apr 3, 2016
 *      Author: gonzalo
 */

#include "Cliente/cliente.h"
#include <string>
#include <stdio.h>

int main(int argc, char *argv[])
{

    cliente* client = new cliente(argc,argv[1],argv[2]);

    client->conectar();
    //client->leer();

    while(client->checkConection()){
    	client->escribir();
    	client->leer();
    }
    client->cerrarSoket();

    Logger::Instance()->Close();
    delete client;
    return 0;
}
