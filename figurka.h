#ifndef FIGURKA_H
#define FIGURKA_H

#include <SDL/SDL.h>
#include "grafika.h"
#include "sachovnice.h"

#define BILAF 0
#define CERNAF 1

class Figurka
{
public:
	Obrazek figurka;
	int barva;
int tah;
double hodnota;
	virtual ~Figurka() {}
	Figurka(int barva);
	virtual void pohlase(int fromY, int fromX, int toY, int toX, Sachovnice *s);
	void kresli(int x, int y);
	virtual bool validniTah(int fromY, int fromX, int toY, int toX, Sachovnice* s);
	virtual bool validniTahSach(int fromY, int fromX, int toY, int toX, Sachovnice* s);
	virtual double hodnotaFigurky(int y, int x);

};

#endif // FIGURKA_H
