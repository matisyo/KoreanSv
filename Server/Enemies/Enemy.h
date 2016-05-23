/*
 * Enemy.h
 *
 *  Created on: May 22, 2016
 *      Author: gonzalo
 */

#ifndef ENEMIES_ENEMY_H_
#define ENEMIES_ENEMY_H_

#include "../MoveableObject.h"
#include "../Weapons/PlayerWeapons/BasicWeapon.h"
#include "../Utils/TiposDefinidos.h"
class Weapon;

class Enemy : public MoveableObject
{
public:

    virtual ~Enemy();

    virtual void load(int x, int y, int width, int height, int textureID, int numFrames);

    virtual void draw();
    virtual void update();
    virtual void clean();

    virtual void shoot();

    virtual void collision();

    void setCollisionDamage(int collisionDamage) { m_collisionDamage = collisionDamage; }

    bool isDead() { return m_dead; }
    bool isDying() { return m_dying; }
    int getCollisionDamage() { return m_collisionDamage; }

    void damage(int damageReceived);

    void sendDrawMessage(bool isAlive);

protected:
    Enemy();

    Weapon* m_enemyWeapon;

    int m_health;
    int m_collisionDamage;

    bool m_dead;
    bool m_dying;
};


#endif /* ENEMIES_ENEMY_H_ */
