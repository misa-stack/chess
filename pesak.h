#ifndef PESAK_H
#define PESAK_H
#include <SDL/SDL.h>
#include "grafika.h"
#include "figurka.h"
#include "sachovnice.h"
class Pesak: public Figurka
{
public:
	Pesak(const int barva);
	bool validniTah(int fromY, int fromX, int toY, int toX, Sachovnice *s);
	double hodnotaFigurky(int y, int x);

};


#endif // PESAK_H
