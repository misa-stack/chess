#ifndef VEZ_H
#define VEZ_H
#include"figurka.h"

class Vez : public Figurka
{
public:
	Vez(const int barva);
	bool validniTah(int fromY, int fromX, int toY, int toX, Sachovnice* s);
	double hodnotaFigurky(int y, int x);

};

#endif // VEZ_H
