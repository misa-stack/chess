#ifndef KUN_H
#define KUN_H
#include "figurka.h"

class Kun : public Figurka
{
public:
	Kun(const int barva);
	bool validniTah(int fromY, int fromX, int toY, int toX, Sachovnice *s);
};

#endif // KUN_H
