/*
 * Mati.h
 *
 *  Created on: May 22, 2016
 *      Author: matias
 */

#ifndef MATI_H_
#define MATI_H_
#include "Enemy.h"

class Mati : public Enemy {
public:
	Mati();
	virtual ~Mati();
	void update();
};

#endif /* MATI_H_ */
