#ifndef KRAL_H
#define KRAL_H

#include "figurka.h"

class Kral: public Figurka
{
public:
	Kral(const int barva);
	bool validniTah(int fromY, int fromX, int toY, int toX, Sachovnice *s);
	void pohlase(int fromY, int fromX,int toY,int toX, Sachovnice *s);

};

#endif // KRAL_H
