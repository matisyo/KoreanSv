
#include "Server/server.h"
#include "Utils/Logger.h"

int main(int argc, char *argv[])
{
	server* servidor = new server(atoi(argv[1]),atoi(argv[2]));
	servidor->escuchar();
	while(servidor->checkStatus())
	{
		servidor->aceptar();

	}

	servidor->closeAllsockets();
    delete servidor;
    Logger::Instance()->Close();
    return 0;
}
