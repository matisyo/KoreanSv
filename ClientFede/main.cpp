/*
 * main.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: gonzalo
 */

#include "Game.h"
#include "Utils/Random.h"
#include <SDL2/SDL.h>
#include "Singletons/GameTimeHelper.h"
#include <iostream>

using namespace std;

const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;

int main(int argc, char **argv)
{
	Random::initialize();
	Game::Instance()->askForName();
	bool jugar = true;
	while(jugar){
		//se conecta
	nombre:
		if(Game::Instance()->initializeClient()){
			//Crea un juego con la ventanita bla bla
			Game::Instance()->createGame(DELAY_TIME);
			jugar = false;

		}
		/*else{
			//Si no se conecto
			if(!Game::Instance()->getRestart()){
				std::cout << "Che mankie y no te pude conectar, seguro taba lleno o tu nombre es 1 bosta\n";
				std::cout << "Queres seguir jugando? y/n\n";
				string choice;
				cin >> choice;

				if(choice.c_str()[0] == 'n'){
					jugar=false;
				}else{
					if(choice.c_str()[0]  != 'y'){
						std::cout << "Elegiste cualqueir bosta: "<< choice.c_str()[0]  <<", ahora jodete y juga\n";
					}
				}
			}
		}*/
		}
	if(Game::Instance()->getRestart()){
					std::cout << "Apretaste Restart\n";
					Game::Instance()->mrMusculo();
					Game::Instance()->setRunning(true);

					jugar = true;
					goto nombre;
				}

	//printf("FPS PROMEDIO: %d \n", (fpsCount/ framesCount));
    std::cout << "game closing...\n";
    Game::Instance()->clean();


    return 0;
}
