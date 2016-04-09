/*
 * NaveObject.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: gonzalo
 */

#include "Nave.h"

Nave::Nave() :  GameObject(),
				m_direction(0,0),
				m_speed(5,5),
				m_dead(false),
				m_dying(false)
{
	m_tag = "Nave";
}

void Nave::load(int x, int y, int width, int height, std::string textureID, int numFrames)
{
    // posicion de la nave en el juego. Puede estar fuera de camara y se dibujará aunque no sea visible
    m_position = Vector2D(x, y);

    // Informacion del sprite/spritesheet
    m_width = width;
    m_height = height;
    m_textureID = textureID;
    m_numFrames = numFrames;
}

void Nave::draw()
{
    //TextureManager::Instance()->drawFrame(m_textureID, m_position.getX(), m_position.getY(), m_width, m_height,
    			//							m_currentRow, m_currentFrame, Game::Instance()->getRenderer(), m_angle, m_alpha);
}

void Nave::update()
{
	//se mueve en la dirección seteada
    m_position.m_x += (m_direction.getX() * m_speed.getX());
    m_position.m_y += (m_direction.getY() * m_speed.getY());
}

void Nave::clean()
{

}

void Nave::collision()
{

}
