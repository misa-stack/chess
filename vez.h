#ifndef VEZ_H
#define VEZ_H

#include <SDL/SDL.h>

#include "figurka.h"

class Vez : public Figurka
{
public:
	Vez(const int barva);
	bool validniTah(int fromY, int fromX, int toY, int toX, Sachovnice *s);

};

#endif // VEZ_H
