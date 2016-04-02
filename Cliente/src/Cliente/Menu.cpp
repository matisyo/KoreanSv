/*
 * Menu.cpp
 *
 *  Created on: Apr 2, 2016
 *      Author: matias
 */

#include "Menu.h"

Menu::Menu() {
	// TODO Auto-generated constructor stub

}

Menu::~Menu() {
	// TODO Auto-generated destructor stub
}

int main(int argc, char *argv[])
{

    cliente* client = new cliente(argc,argv[1],argv[2]);

    client->conectar();
    client->leer();

    while(client->checkConection()){
    	client->escribir();
    	client->leer();
    }
    client->cerrarSoket();

    Logger::Instance()->Close();
    delete client;
    return 0;
}
