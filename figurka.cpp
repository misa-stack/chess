#include "figurka.h"
#include <SDL/SDL.h>
#include "grafika.h"

Figurka::Figurka(int barva): barva(barva)
{
	tah = 0;
}
void Figurka::pohlase(int fromY, int fromX, int toY, int toX, Sachovnice *s)
{
tah++;
}
bool Figurka::validniTahSach(int fromY, int fromX, int toY, int toX, Sachovnice *s)
{
	Tah aktualniTah;

	aktualniTah.fromX = fromX;
	aktualniTah.fromY = fromY;
	aktualniTah.toX = toX;
	aktualniTah.toY = toY;
	aktualniTah.vyhozena = s->pozice[toY][toX];
	s->pozice[toY][toX] = s->pozice[fromY][fromX];
	s->pozice[fromY][fromX] = NULL;
	if(!s->jeSach(s->barvicka))

	{
		s->pozice[fromY][fromX] = s->pozice[toY][toX];
		s->pozice[toY][toX] = aktualniTah.vyhozena;
		return true;
	}
	else
	{
		s->pozice[fromY][fromX] = s->pozice[toY][toX];
		s->pozice[toY][toX] = aktualniTah.vyhozena;
		return false;

	}
}
void Figurka::kresli(int x, int y)
{
	figurka.umisti(x,y);
	figurka.kresli();
}
bool Figurka::validniTah(int fromY, int fromX, int toY, int toX, Sachovnice* s)
{

	if (!s->jeNepritel(toY, toX, barva)) return false;

	return true;


}
