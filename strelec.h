#ifndef STRELEC_H
#define STRELEC_H
#include "figurka.h"

class Strelec : public Figurka
{
public:
	Strelec(const int barva );
	bool validniTah(int fromY, int fromX, int toY, int toX, Sachovnice *s);
	double hodnotaFigurky(int y, int x);

};

#endif // STRELEC_H
