#ifndef KRALOVNA_H
#define KRALOVNA_H

#include"figurka.h"

class Kralovna: public Figurka
{
public:
	Kralovna(const int barva);
	bool validniTah(int fromY, int fromX, int toY, int toX, Sachovnice *s);
	double hodnotaFigurky(int y, int x);

};


#endif // KRALOVNA_H
