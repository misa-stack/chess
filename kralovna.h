#ifndef KRALOVNA_H
#define KRALOVNA_H

#include"figurka.h"

class Kralovna: public Figurka
{
public:
	Kralovna(const int barva);
	bool validniTah(int fromY, int fromX, int toY, int toX, Sachovnice *s);
};

#endif // KRALOVNA_H
