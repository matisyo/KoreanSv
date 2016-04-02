#include "server.h"


server::server(int port, int maxC): MAX_CLIENTES(maxC)
{
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_condv, NULL);
    //Creo Socket
    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
         if (sockfd < 0)
            error("Error: No se pudo crear el socket");

    // Limpio el struct todo en 0file:///home/matias/Desktop/Taller/server.h

     bzero((char *) &serv_addr, sizeof(serv_addr));

    /*Seteo la gilada basica del server*/
     serv_addr.sin_family = AF_INET;

     //Setea el adress del srv
     serv_addr.sin_addr.s_addr = INADDR_ANY;

     //Transforma el int por el tema del endianness
     serv_addr.sin_port = htons(port);

     //Por ultimo enlazo el socket
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
              error("Server: No se enlazo correctamente el socket del server.");

    m_clientNum = 0;
    m_listaDeClientes.resize(MAX_CLIENTES);
    m_clientThreads.resize(MAX_CLIENTES);

    startThread();
    printf("Bienvenido a servu\n");
}

server::~server()
{
	closeAllsockets();
	m_listaDeClientes.clear();
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_condv);
}
void server::error(const char *msg)
{
	Logger::Instance()->LOG(msg, ERROR);
    exit(1);
}

void server::escuchar()
{
    //Se frena el thread hasta que escucha a alguien CREO
    listen(sockfd,5);
}
void server::aceptar(){
    //Aca el accept va a pisar el cli_addr y este nuevo es el sokete que lo relaciona a ese cliente
    //Deberia meter el nuevo thread por aca
	socklen_t clilen = sizeof(cli_addr);

    if(getNumClientes() < MAX_CLIENTES){
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
    	send(newsockfd, "Server O K   \n", 21, 0);
    }else{
    	newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
    	send(newsockfd, "exit", 5, 0);
    	close(newsockfd);
    	sleep(1);
    	return;
    	//printf("Un cliente ha sido rechazado por falta de capacidad\n");
    	//Logger::Instance()->LOG("Server: Cliente rechazado. El servidor no puede aceptar más clientes.", WARN);
	}
    if (newsockfd < 0)
    {
    	std::stringstream ss;
    	ss <<"Server: No se pudo aceptar al cliente " << inet_ntoa(cli_addr.sin_addr);
    	Logger::Instance()->LOG(ss.str(), WARN);
    	return;
    }
    else{
    	//Crea el cliente
    	m_lastID = m_listaDeClientes.add(newsockfd);
    	if (m_lastID < 0)
    	{
    		Logger::Instance()->LOG("Server: Cliente rechazado. El servidor no puede aceptar más clientes.", WARN);
    		return;
    	}
    	printf("se agrego en la posicion %d \n", m_lastID);
    	//pthread_t nuevoThread;
    	//pthread_create(&nuevoThread, NULL, &server::mati_method, (void*)this);
    	pthread_create(&m_clientThreads[m_lastID], NULL, &server::mati_method, (void*)this);
    	aumentarNumeroClientes();


    	printf("Se creo un thread %d\n", m_lastID);
    	std::stringstream ss;
    	ss << "Server: Se acepto el cliente: " << inet_ntoa(cli_addr.sin_addr);
    	Logger::Instance()->LOG(ss.str(), DEBUG);
	}

}


void server::escribir(int id)
{
    send(m_listaDeClientes.getElemAt(id), "Llego correctamente!\n", 21, 0);
}
bool server::leer(int id)
{
    //Reseteo el buffer que se va a completar con nuevos mensajes
    bzero(buffer,256);
    int n = recv(m_listaDeClientes.getElemAt(id), buffer, 255, 0);
    if (n < 0)
    {
    	//Cliente Desconectado
    	closeSocket(id);
    	printf("leyo -1 el recv\n");
    	return false;

    }
    if (n == 0)
    {
    	//Cliente se desconecto?
    	closeSocket(id);
    	printf("leyo 0 el recv\n");
    	return false;
    }

    string my_str2 (buffer);
    mensije msg;
    msg.id = id;
    msg.texto = my_str2;

    m_queue.add(msg);
    return true;
}

void* server::procesar(void)
{
    int aux=0;
	while(true)
	{
		aux++;
		if (aux ==10000000)
		{
			aux =0;
			if (m_queue.size() != 0)
			{
				mensije msg = m_queue.remove();

				printf("%d: %s \n",msg.id + 1,msg.texto.c_str());
			}
		}
	}
}

void *server::newDialog(void)
{
    int id = this->m_lastID;
    while(true)
    {
    	if (!this->leer(id))
    		break;
        this->escribir(id);
	}
    pthread_exit(NULL);

}
// nunca deberais desafiar el poder del matimethod...o sera muy tarde...fuera de joda no tokes nada aca gil ah si? ven aqui
void *server::mati_method(void *context)
{
	return ((server *)context)->newDialog();
}
void *server::mati_method2(void *context)
{
	return ((server *)context)->procesar();
}


void server::startThread()
{
	pthread_create(&threadDeProcesos, NULL, mati_method2,(void*)this );
}

void server::closeAllsockets()
{
    //Cuando el Server tenga lista de threads hay q recorrer cerrando
	for (int i = 0; i < m_listaDeClientes.size(); i++)
	{
		closeSocket(i);
	}
    //close(newsockfd);
    close(sockfd);
}

void server::closeSocket(int id)
{
	Logger::Instance()->LOG("Server: Se desconectó un cliente.", DEBUG);
	reducirNumeroClientes();
	printf("Se desconectó un cliente, hay lugar para un chaval mas.\n");
	close(m_listaDeClientes.getElemAt(id));
	m_listaDeClientes.removeAt(id);
	//terminar thread
}

void server::aumentarNumeroClientes()
{
    pthread_mutex_lock(&m_mutex);
    m_clientNum++;
    pthread_cond_signal(&m_condv);
    pthread_mutex_unlock(&m_mutex);
}
void server::reducirNumeroClientes()
{
    pthread_mutex_lock(&m_mutex);
    m_clientNum--;
    pthread_cond_signal(&m_condv);
    pthread_mutex_unlock(&m_mutex);
}
const int server::getNumClientes()
{
    pthread_mutex_lock(&m_mutex);
    int cant = m_clientNum;
    pthread_mutex_unlock(&m_mutex);
    return cant;
}

const int server::getMaxClientes()
{
	return MAX_CLIENTES;
}


int main(int argc, char *argv[])
{
	server* servidor = new server(atoi(argv[1]),atoi(argv[2]));
	//int i = 1;
	//pthread_t threads[servidor->getMaxClientes()];
	//void* status;
	servidor->escuchar();
	while(true)
	{
		servidor->aceptar();

		//pthread_create(&threads[i], NULL, &server::mati_method, (void*)servidor);
	}

	servidor->closeAllsockets();
    delete servidor;
    Logger::Instance()->Close();
    return 0;
}
