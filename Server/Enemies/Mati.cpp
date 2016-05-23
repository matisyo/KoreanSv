/*
 * Mati.cpp
 *
 *  Created on: May 22, 2016
 *      Author: matias
 */

#include "Mati.h"

Mati::Mati() :Enemy() {

	m_health= 300;
	m_enemyWeapon = new BasicWeapon();

}

Mati::~Mati() {
	// TODO Auto-generated destructor stub
}

void Mati::update(){
	Enemy::update();
	Enemy::shoot();
	int random = Random::getRange(0,10);
	if (random == 0 || random == 1||random == 2 ){
		m_position.setX(m_position.getX()+10);
	}
	if (random == 5|| random == 6||random == 7 ){
		m_position.setX(m_position.getX()-10);
	}
	if (random == 4 || random == 8){
			Enemy::shoot();
	}
	if (random == 9){
		m_position.setY(m_position.getY()-10);
	}
	if (random == 10){
		m_position.setY(m_position.getY()+10);
	}
}
