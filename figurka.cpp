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
double Figurka::hodnotaFigurky(int y, int x){
	return 0.0;
}
bool Figurka::validniTahSach(int fromY, int fromX, int toY, int toX, Sachovnice *s)
{
	Tah aktualniTah;

	aktualniTah.fromX = fromX;
	aktualniTah.fromY = fromY;
	aktualniTah.toX = toX;
	aktualniTah.toY = toY;
	aktualniTah.vyhozena = s->pozice[toY][toX];

	if (aktualniTah.vyhozena && aktualniTah.vyhozena->kdoJsi() % 6 == 5)
		return false;

	Figurka* enPassantPawn = NULL;
	int enPassantY = fromY;
	if (kdoJsi() % 6 == 0 && toX == s->enPassantX && toY == s->enPassantY &&
		aktualniTah.vyhozena == NULL) {
		enPassantPawn = s->pozice[enPassantY][toX];
		if (!enPassantPawn || enPassantPawn->kdoJsi() % 6 != 0 ||
			enPassantPawn->barva == barva) return false;
		s->pozice[enPassantY][toX] = NULL;
	}
	s->pozice[toY][toX] = s->pozice[fromY][fromX];
	s->pozice[fromY][fromX] = NULL;
	if(!s->jeSach(s->barvicka))

	{
		s->pozice[fromY][fromX] = s->pozice[toY][toX];
		s->pozice[toY][toX] = aktualniTah.vyhozena;
		if (enPassantPawn) s->pozice[enPassantY][toX] = enPassantPawn;
		return true;
	}
	else
	{
		s->pozice[fromY][fromX] = s->pozice[toY][toX];
		s->pozice[toY][toX] = aktualniTah.vyhozena;
		if (enPassantPawn) s->pozice[enPassantY][toX] = enPassantPawn;
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
