/*
 * Enemy.cpp
 *
 *  Created on: May 22, 2016
 *      Author: gonzalo
 */

#include "Enemy.h"

Enemy::Enemy(): MoveableObject(),
		m_health(10),
	    m_collisionDamage(100),
		m_dead(false),
		m_dying(false)
{
    m_enemyWeapon = new BasicWeapon();
}

Enemy::~Enemy()
{

}

void Enemy::load(int x, int y, int width, int height, int textureID, int numFrames)
{
	 MoveableObject::load(x, y, width, height, textureID, numFrames);
}

void Enemy::draw()
{
	MoveableObject::draw();
}
void Enemy::update()
{
	MoveableObject::update();
	if (m_dirty)
		sendDrawMessage(!m_dead);
}
void Enemy::clean()
{
	MoveableObject::clean();
	if (m_enemyWeapon)
	{
		delete m_enemyWeapon;
	}
}

void Enemy::shoot()
{
	if (m_enemyWeapon)
	{
		m_enemyWeapon->shoot(Vector2D(m_position.m_x + m_position.m_x/2, m_position.m_y));
	}
}

void Enemy::damage(int damageReceived)
{
	m_health -= damageReceived;
	if (m_health <= 0)
	{
		//Hacer explosion, setear dying en true, etc
		m_dead = true;
	}
}

void Enemy::collision()
{
	MoveableObject::collision();
}

void Enemy::sendDrawMessage(bool isAlive)
{
	DrawMessage drawMsg;
	drawMsg.unused1 = false;
	drawMsg.connectionStatus = true;
	drawMsg.alive = isAlive;
	drawMsg.hasSound = false;

	drawMsg.objectID = m_objectId;
	drawMsg.layer = m_layer;
	drawMsg.soundID = 0;
	drawMsg.column = m_currentFrame;
	drawMsg.row = m_currentRow;
	drawMsg.posX = m_position.getX();
	drawMsg.posY = m_position.getY();
	drawMsg.textureID = m_textureID;

	if (USE_DRAWMESSAGE_PACKAGING)
	{
		Game::Instance()->addToPackage(drawMsg);
	}
	else
	{
		Game::Instance()->sendToAllClients(drawMsg);
	}
}
