#include "pesak.h"
#include "sachovnice.h"
#include <SDL/SDL.h>
#include "grafika.h"

Pesak::Pesak(const int barva): Figurka(barva)
{
	if (barva == BILAF)
	{
		figurka.nacti("pesak.png");
	}
	else if (barva == CERNAF)
	{
		figurka.nacti("pesakcerny.png");
	}
	hodnota = 10;

}
double Pesak::hodnotaFigurky(int y, int x)
{
	double hodnotapesakaC[8][8] =
	{
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.5, 1.0, 1.0, -2.0, -2.0, 1.0, 1.0, 0.5},
		{0.5, -0.5, -1.0, 0.0, 0.0, -1.0, -0.5, 0.5},
		{0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0},
		{0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5, 0.5},
		{1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 1.0},
		{5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
	};
	double hodnotapesakaB[8][8] =
	{
	    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	    {5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0},
	    {1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 1.0},
	    {0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5, 0.5},
	    {0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0},
	    {0.5, -0.5, -1.0, 0.0, 0.0, -1.0, -0.5, 0.5},
	    {0.5, 1.0, 1.0, -2.0, -2.0, 1.0, 1.0, 0.5},
	    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
	};

	if (barva == CERNAF)
	{
		return hodnotapesakaC[x][y] + hodnota;
	}
	else if(barva == BILAF)
	{
		return hodnotapesakaB[x][y] + hodnota;

	}
}
bool Pesak::validniTah(int fromY, int fromX, int toY, int toX, Sachovnice* s)
{
	int smer = (barva == BILAF) ? -1 : 1;
	int start = (barva == BILAF) ? 6 : 1;

	int dy = toY - fromY;
	int dx = toX - fromX;

	if (dx == 0 && dy == smer && !s->jeFigurka(toY, toX))
		return true;

	if (dx == 0 && dy == 2*smer && fromY == start &&
			!s->jeFigurka(toY, toX) &&
			!s->jeFigurka(fromY + smer, fromX))
		return true;

	if (abs(dx) == 1 && dy == smer && s->jeNepritel(toY, toX, barva))
		return true;

	return false;
}

/*
bool Sachovnice::validniTahPesak(int fromY, int fromX, int toY, int toX)
{
    Figurka* p = pozice[fromY][fromX];
    int smer = (p->barva == BILAF) ? -1 : 1;
    int startovniRada = (p->barva == BILAF) ? 6 : 1;
    int dy = toY - fromY;
    int dx = toX - fromX;

    if (dx == 0 && dy == smer && pozice[toY][toX] == NULL) return true;
    if (dx == 0 && dy == 2 * smer && fromY == startovniRada && pozice[toY][toX] == NULL && pozice[fromY + smer][fromX] == NULL) return true;
    if (std::abs(dx) == 1 && dy == smer && pozice[toY][toX] != NULL && pozice[toY][toX]->barva != p->barva) return true;

    return false;
}
*/
