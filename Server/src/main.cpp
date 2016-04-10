
#include "Server/server.h"
#include "Utils/Logger.h"
#include "Utils/Parser/ParserServidor.h"
#include <iostream>
using namespace std;


int main(int argc, char *argv[])
{
	std::string fileName;
	if(argc < 2)
		fileName = "src/Utils/Default/servidor.xml";
	//argv[1] es el path del archivo cliente.xml
	else
		fileName.assign(argv[1]);

	ParserServidor* servidorParser = new ParserServidor();
	servidorParser->parsearDocumento(fileName);

	int porto = servidorParser->getServidorInfo().puerto ;
	int maxClientes = servidorParser->getServidorInfo().cantMaximaClientes;

	server* servidor = new server(porto, maxClientes);
	servidor->escuchar();
	while(servidor->isRunning())
	{
		servidor->aceptar();

	}
	servidor->closeAllsockets();
    delete servidor;
    Logger::Instance()->Close();
    return 0;
}
