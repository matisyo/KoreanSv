#include "Game.h"

Game* Game::s_pInstance = 0;

Game::Game():
m_pWindow(0),
m_pRenderer(0),
m_running(false),
m_reseting(false),
m_scrollSpeed(2)
{
	pthread_mutex_init(&m_resetMutex, NULL);
}

Game::~Game()
{
    // we must clean up after ourselves to prevent memory leaks
    m_pRenderer= 0;
    m_pWindow = 0;
    pthread_mutex_destroy(&m_resetMutex);
}


bool Game::init(const char* title, int xpos, int ypos, int width, int height)
{
    // Tamaño de la ventana

    m_parserNivel = new ParserNivel();
    m_parserNivel->parsearDocumento(XML_PATH);

    m_textureHelper = new TextureHelper();

    m_gameWidth = m_parserNivel->getVentana().ancho;
    m_gameHeight = m_parserNivel->getVentana().alto;

    //es necesario llamar a initialize Textures despues de haber creado el parser y textureHelper porque los usa
    initializeTexturesInfo();

    //printf("Path isla: %s \n", m_parserNivel->getListaSprites()[5].path.c_str());
   // printf("ID isla: %s \n", m_parserNivel->getListaSprites()[5].id.c_str());

    printf("Se cargo el escenario con ancho %d y alto %d\n",m_gameWidth, m_gameHeight );

    inicializarServer();

    m_level = new Level();
    m_level->loadFromXML();

    enemy = new Enemy();
    enemy->load(0,0,50,50,0,1);
    CollitionHandler::Instance()->addEnemy(enemy);

    //tudo ben
    m_running = true;

    return true;
}

bool Game::createPlayer(int clientID,  const std::string& playerName)
{

	bool nameExists;
	std::stringstream ss;

	//Se fija si existe un jugador con el nombre ingresado
	nameExists = !validatePlayerName(playerName);

	if (nameExists)
	{
		int actualPlayerID = getFromNameID(playerName);
		Player* player = m_listOfPlayer[actualPlayerID];

		//agrega jugador al manejador de colisiones
		CollitionHandler::Instance()->addPlayer(player);

		if (player->isConnected()) //El jugador con ese nombre ya esta conectado
		{
			ss <<"Server: El jugador con nombre" << playerName << " ya se encuentra conectado.";
			Logger::Instance()->LOG(ss.str(), WARN);
			printf("%s \n", ss.str().c_str());
			player->refreshDirty();
			return false;
		}
		else //Se desconecto y se esta volviendo a conectar
		{
			player->setConnected(true);
			player->refreshDirty();
			m_server->informGameBegan(clientID);
			m_server->informPlayerReconnected(clientID);
			setPlayersDirty();
			return true;
		}
	}
	//Si no existe el nombre:

	//Esto controla que solo se puedan volver a conectar los que arrancaron jugando al ppio

	if (m_listOfPlayer.size() == m_parserNivel->getEscenario().cantidadJugadores)
	{
		ss <<"Server: El jugador con nombre" << playerName << " no se pudo conectar, ya está llena la partida.";
		Logger::Instance()->LOG(ss.str(), WARN);
		printf("%s \n", ss.str().c_str());
		return false;
	}

	int playerSpeed = m_parserNivel->getAvion().velDespl;
	int shootingCooldown = m_parserNivel->getAvion().cdDisp;
	int bulletsSpeed = m_parserNivel->getAvion().velDisp;

	Player* newPlayer = new Player();
	newPlayer->setObjectID(clientID);
	newPlayer->setSpeed(Vector2D(playerSpeed, playerSpeed));

	newPlayer->setWeaponStats(bulletsSpeed, shootingCooldown, newPlayer->getObjectId(), newPlayer->getTeamNumber());

	m_playerNames[clientID] = playerName;

	ss << "player" << (clientID + 1);
	string playerStringID = ss.str();
	int playerTextureID = m_textureHelper->stringToInt(playerStringID);

	//14 HARDCODEADO
	newPlayer->load(m_gameWidth/2, m_gameHeight/2, 38, 64, playerTextureID, 14);
	newPlayer->setConnected(true);

	m_listOfPlayer[newPlayer->getObjectId()]= newPlayer;
	printf("Player: %s inicializado con objectID: %d y textureID: %d\n",m_playerNames[clientID].c_str(), newPlayer->getObjectId(), clientID);

	return true;
}

int Game::getFromNameID(const std::string& playerName)
{
	for (std::map<int, std::string>::iterator it = m_playerNames.begin(); it != m_playerNames.end(); ++it )
	{
		if (it->second.compare(playerName.c_str()) == 0)
			return it->first;
	}
	return -1;
}

bool Game::validatePlayerName(const std::string& playerName)
{
	for (std::map<int, std::string>::iterator it = m_playerNames.begin(); it != m_playerNames.end(); ++it )
	{
	    if (it->second.compare(playerName.c_str()) == 0)
	        return false;
	}
	//Nombre disponible
	return true;
}

void Game::disconnectPlayer(int playerID)
{
	PlayerDisconnection playerDiscMsg;
	std::size_t length = m_playerNames[playerID].copy(playerDiscMsg.name, MAX_NAME_LENGTH, 0);
	playerDiscMsg.name[length]='\0';
	playerDiscMsg.objectID = m_listOfPlayer[playerID]->getObjectId();
	playerDiscMsg.layer = m_listOfPlayer[playerID]->getLayer();
	m_server->informPlayerDisconnection(playerDiscMsg, playerID);

	m_listOfPlayer[playerID]->setConnected(false);
	//m_playerNames.erase(playerID);
	//listOfPlayer.erase(id);
	//mostrar en gris
}

void Game::render()
{
    //SDL_RenderClear(m_pRenderer);


    //Dibujar lo que haya que dibujar
 /*   m_background->draw(); //Provisorio
    m_island->draw(); //Provisorio
    m_player->draw();//Provisorio
*/
    //SDL_RenderPresent(m_pRenderer);
}


void Game::update()
{
	BulletsHandler::Instance()->updateBullets();

	m_level->update();

	enemy->update();

	for (std::map<int,Player*>::iterator it=m_listOfPlayer.begin(); it != m_listOfPlayer.end(); ++it)
	{
		//printf("objectID = %d \n", it->second.getObjectId());
		if (it->second)
		{
			it->second->update();
		}
	}
	for (std::map<int,GameObject*>::iterator it=m_listOfGameObjects.begin(); it != m_listOfGameObjects.end(); ++it)
	{
		//printf("objectID = %d \n", it->second.getObjectId());
		if (it->second)
		{
			it->second->update();
		}
	}

	CollitionHandler::Instance()->handleCollitions();

}

void Game::initializeTexturesInfo()
{
	std::vector<Sprite> sprites = m_parserNivel->getListaSprites();
	for (std::vector<Sprite>::iterator it = sprites.begin() ; it !=  sprites.end(); ++it)
	{
		TextureInfo textureInfo;
		//id string to int
		textureInfo.textureID = m_textureHelper->stringToInt((*it).id);
		//path string to buffer
		std::size_t length = (*it).path.copy(textureInfo.path, PATH_MAX_LENGTH, 0);
		textureInfo.path[length]='\0';
		//otras variables de imagen
		textureInfo.numFrames = (*it).cantidad;
		textureInfo.height = (*it).alto;
		textureInfo.width = (*it).ancho;
		textureInfo.lastTexture = false;

		TextureManager::Instance()->addTextureInfo(textureInfo);
	}

}

void Game::setPlayersDirty()
{
	for (std::map<int,Player*>::iterator it=m_listOfPlayer.begin(); it != m_listOfPlayer.end(); ++it)
	{
		if (it->second)
		{
			it->second->setDirty(true);
		}
	}
}

void Game::handleEvents()
{
}
void Game::inicializarServer()
{

	std::string fileName = "Utils/Default/servidor.xml";

	ParserServidor* servidorParser = new ParserServidor();
	servidorParser->parsearDocumento(fileName);

	LoggerInfo loggerInfo = servidorParser->getLoggerInfo();
	Logger::Instance()->setLoglevel(loggerInfo.debugAvailable, loggerInfo.warningAvailable, loggerInfo.errorAvailable);

	int porto = servidorParser->getServidorInfo().puerto ;
	int maxClientes = m_parserNivel->getEscenario().cantidadJugadores;
	//printf("Creando enlazamiento\n");
	m_server = new server(porto, maxClientes);

    m_drawMessagePacker = new DrawMessagesPacker(m_server);

	//printf("Se pone a escuchar\n");
	m_server->escuchar();

	int auxi = 0;
	while(m_server->getNumClientes() < m_server->getMaxClientes())
	{
		auxi++;
		m_server->aceptar();
	}

	//Informa a los clientes que el juego comenzará
	m_server->informGameBeginning();

	keepListening();
}


void Game::sendToAllClients(DrawMessage mensaje)
{
	m_server->sendDrawMsgToAll(mensaje);
}

void Game::addToPackage(DrawMessage drawMsg)
{
	m_drawMessagePacker->addDrawMessage(drawMsg);
}
void Game::sendPackages()
{
	m_drawMessagePacker->sendPackedMessages();
}

void* Game::koreaMethod(void)
{
	while (m_server->isRunning())
	{
		m_server -> aceptar();
	}
	 pthread_exit(NULL);
}
void *Game::thread_method(void *context)
{
	return ((Game *)context)->koreaMethod();
}
void Game::keepListening()
{
	pthread_create(&listenThread, NULL, &Game::thread_method, (void*)this);

}

void Game::actualizarEstado(int id, InputMessage inputMsg){
	/*printf("Actualizar player %d\n",inputMsg.objectID);
	printf("button right: %d \n",inputMsg.buttonRight);
	printf("button left: %d \n",inputMsg.buttonLeft);
	printf("button up: %d \n",inputMsg.buttonUp);
	printf("button down: %d \n",inputMsg.buttonDown);*/

	m_listOfPlayer[inputMsg.objectID]->handleInput(inputMsg);
}

void Game::clean()
{
    cout << "cleaning game\n";
    BulletsHandler::Instance()->clearBullets();

	for (std::map<int,Player*>::iterator it=m_listOfPlayer.begin(); it != m_listOfPlayer.end(); ++it)
	{
		if (it->second)
		{
			 it->second->clean();
			 delete  it->second;
		}
	}
	for (std::map<int,GameObject*>::iterator it=m_listOfGameObjects.begin(); it != m_listOfGameObjects.end(); ++it)
	{
		if (it->second)
		{
			 it->second->clean();
			 delete  it->second;
		}
	}
    m_listOfPlayer.clear();
    m_listOfGameObjects.clear();
    m_playerNames.clear();

    m_drawMessagePacker->clean();
    delete m_drawMessagePacker;

    m_parserNivel->clean();
    delete m_parserNivel;

    m_level->clean();
    delete m_level;
    delete m_textureHelper;

    CollitionHandler::Instance()->clean();

    InputHandler::Instance()->clean();
    TextureManager::Instance()->clearTextureMap();


    SDL_DestroyWindow(m_pWindow);
    SDL_DestroyRenderer(m_pRenderer);
    SDL_Quit();
}

void Game::refreshPlayersDirty()
{
	for (std::map<int,Player*>::iterator it=m_listOfPlayer.begin(); it != m_listOfPlayer.end(); ++it)
	{
		if (it->second)
		{
			it->second->setDirty(true);
		}
	}
}

void Game::resetGame()
{
	pthread_mutex_lock(&m_resetMutex);
	 BulletsHandler::Instance()->clearBullets();
	 InputHandler::Instance()->clean();
	 CollitionHandler::Instance()->reset();
	 //delete m_background;
	 //delete m_island;
	 //m_listOfGameObjects.clear();
	 if (m_level)
	 {
		 m_level->clean();
		 delete m_level;
	 }
	 m_parserNivel->clean();

	 //CARGAR XML
	 m_parserNivel = new ParserNivel();
	 m_parserNivel->parsearDocumento(XML_PATH);
	 m_gameWidth = m_parserNivel->getVentana().ancho;
	 m_gameHeight = m_parserNivel->getVentana().alto;

	 m_level = new Level();
	 m_level->loadFromXML();

	int newPlayerSpeed = m_parserNivel->getAvion().velDespl;
	int newShootingCooldown = m_parserNivel->getAvion().cdDisp;
	int newBulletsSpeed = m_parserNivel->getAvion().velDisp;
	for (std::map<int,Player*>::iterator it=m_listOfPlayer.begin(); it != m_listOfPlayer.end(); ++it)
	{
		if (it->second)
		{
			 it->second->setSpeed(Vector2D(newPlayerSpeed, newPlayerSpeed));
			 it->second->setShootingCooldown(newShootingCooldown);
			 it->second->setShootingSpeed(newBulletsSpeed);
			 it->second->refreshDirty();
			 it->second->StopFlipAnimation();
		}
	}

	 //tudo ben
	 m_running = true;
	 pthread_mutex_unlock(&m_resetMutex);
}
