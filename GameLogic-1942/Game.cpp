#include "Game.h"

Game* Game::s_pInstance = 0;

Game::Game():
m_pWindow(0),
m_pRenderer(0),
m_running(false),
m_scrollSpeed(0.8)
{

}

Game::~Game()
{
    // we must clean up after ourselves to prevent memory leaks
    m_pRenderer= 0;
    m_pWindow = 0;
}


bool Game::init(const char* title, int xpos, int ypos, int width, int height, int SDL_WINDOW_flag)
{
    // Tamaño de la ventana
    m_gameWidth = width;
    m_gameHeight = height;

    if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        cout << "SDL init success\n";

        m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, SDL_WINDOW_flag);

        if(m_pWindow != 0)
        {
            cout << "window creation success\n";
            m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_SOFTWARE);

            if(m_pRenderer != 0)
            {
                cout << "renderer creation success\n";
                SDL_SetRenderDrawColor(m_pRenderer, 0,0,0,255);
            }
            else
            {
                cout << "renderer init fail\n";
                return false;
            }
        }
        else
        {
            cout << "window init fail\n";
            return false;
        }
    }
    else
    {
        cout << "SDL init fail\n";
        return false;
    }


    //Provisorio
    m_player = new Player(true);
    m_player->load(m_gameWidth/2, m_gameHeight/2, 38, 64, "blackship", 1);
    TextureManager::Instance()->load("Assets/Sprites/BlackShip.png", "blackship", m_pRenderer);
    setUpKorea();
    //tudo ben
    m_running = true;
    return true;
}

void Game::render()
{
    SDL_RenderClear(m_pRenderer);

    //Dibujar lo que haya que dibujar
    m_player->draw();//Provisorio

    SDL_RenderPresent(m_pRenderer);
}

void Game::update(Vector2D* direction)
{
	m_player->update(direction); // Provisorio
}

void Game::handleEvents()
{
	m_player->handleInput();
	InputHandler::Instance()->update();
}

void Game::setUpKorea()
{
		std::string	fileName = "Utils/Default/cliente.xml";

		ParserCliente* parsersito = new ParserCliente();
	    parsersito->parsearDocumento(fileName);

		LoggerInfo loggerInfo = parsersito->getLoggerInfo();
		Logger::Instance()->setLoglevel(loggerInfo.debugAvailable, loggerInfo.warningAvailable, loggerInfo.errorAvailable);

	    string ip = parsersito->getConexionInfo().ip;
	    int porto = parsersito->getConexionInfo().puerto;
	    std::vector<Mensaje> listaDeMensajes = parsersito->getListaMensajes() ;

	    m_client = new cliente(3,ip,porto, listaDeMensajes);


}
void Game::conectToKorea()
{
	if (!m_client->conectar())
					{
						printf("No se pudo establecer conexión con el servidor.\n");

					}
				else
				{
					readFromKorea();
				}
}
void Game::sendToKorea(Vector2D m_direction)
{
	int m_playerId = 1;
	Mensaje mensaje1;
	Mensaje mensaje2;

	mensaje1.id = "hola";
	mensaje1.tipo = "movex";

	std::ostringstream ss;
	ss << m_direction.m_x;
	std::string s(ss.str());
	mensaje1.valor = s;


	m_client->escribir(mensaje1);

	mensaje2.id = "hola";
	mensaje2.tipo = "movey";
	std::ostringstream gg;
	gg << m_direction.m_y;
	std::string g(gg.str());
	mensaje2.valor = s;



	m_client->escribir(mensaje2);

}
void* Game::koreaMethod(void)
{

	std::cout << "Empece a ciclar bitches!\n";
	while (Game::Instance()->isRunning()) {

			m_client->leer();
	        }
	 pthread_exit(NULL);
}
void *Game::thread_method(void *context)
{
	return ((Game *)context)->koreaMethod();
}
void Game::readFromKorea()
{
	pthread_create(&listenThread, NULL, &Game::thread_method, (void*)this);

}


void Game::clean()
{
    cout << "cleaning game\n";

    delete m_player; //Provisorio
    InputHandler::Instance()->clean();

    TextureManager::Instance()->clearTextureMap();

    SDL_DestroyWindow(m_pWindow);
    SDL_DestroyRenderer(m_pRenderer);
    SDL_Quit();
}
