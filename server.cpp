#include "server.h"


server::server(int port, int maxC)
{

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
              error("Error: No se enlazo correctamente el socket al server.");
    clientNUm = 0;
    MAXCLIENTES = maxC;
    startThread();

}

server::~server()
{
	closeAllsockets();
}
void server::error(const char *msg)
{
    perror(msg);
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
    clilen = sizeof(cli_addr);
    if(clientNUm < MAXCLIENTES)
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
    else{
        error("Error: BASTA DE METER CLIENTES LOCO");
        }
    if (newsockfd < 0)
        error("Error: No se pudo aceptar al cliente por algun motivo.");
    else{
        printf("Se acepto un cliente \n");
        listaDeClientes[clientNUm] = newsockfd;
        clientNUm++;
        }

}

void server::escribir(int id)
{
    write(listaDeClientes[id-1], "Llego correctamente!\n", 21);
}
void server::leer(int id)
{
    //Reseteo el buffer que se va a completar con nuevos mensajes
    bzero(buffer,256);
    n = read(listaDeClientes[id-1],buffer,255);
    if (n < 0) error("Error:Hubo problemas para leer desde el socket");


    string my_str2 (buffer);
    mensije msg;
    msg.id = id;
    msg.texto = my_str2;

    m_queue.add(msg);
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
            mensije hola = m_queue.remove();

            printf("%d: %s \n",hola.id,hola.texto.c_str());
            }
        }
	}
}

void *server::newDialog(void)
    {
    int f = 0;
    int id = this->clientNUm;
    while(true)
        {
        this->leer(id);
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
//

void server::startThread()
{
	pthread_create(&threadDeProcesos, NULL, mati_method2,(void*)this );
}

void server::closeAllsockets()
{
    //Cuando el Server tenga lista de threads hay q recorrer cerrando
    close(newsockfd);
    close(sockfd);
}

int main(int argc, char *argv[])
{
	server* objetiti = new server(atoi(argv[1]),atoi(argv[2]));

	pthread_t threads[5];
	int rc;
	void* status;

	int i = 0;
    objetiti->escuchar();
	while(true)
	{
		objetiti->aceptar();

		rc = pthread_create(&threads[i], NULL, &server::mati_method, (void*)objetiti);

        printf("Se creo un thread %d\n", i+1);

		i++;
	}


    objetiti->closeAllsockets();
    delete objetiti;
    return 0;
}
