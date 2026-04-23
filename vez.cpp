#include "Vez.h"
#include <SDL/SDL.h>
#include "pesak.h"
#include "grafika.h"
#include <list>
Vez::Vez(const int barva): Figurka (barva)
{
	if (barva == BILAF)
	{
		figurka.nacti("vezb.png");

	}
	else if (barva == CERNAF)
	{
		figurka.nacti("vezc.png");
	}
	hodnota = 50;




}
double Vez::hodnotaFigurky(int y, int x){
	double hodnotaC[8][8] =
	{
	    {0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0},
	    {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
	    {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
	    {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
	    {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
	    {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
	    {0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5},
	    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
	};
	double hodnotaB[8][8] =

		{
		    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		    {0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5},
		    {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		    {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		    {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		    {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		    {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
		    {0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0}
		};

		if (barva == CERNAF)
		{
			return hodnotaC[x][y] + hodnota;
		}
		else if(barva == BILAF)
		{
			return hodnotaB[x][y] + hodnota;

		}
	}

bool Vez::validniTah(int fromY, int fromX, int toY, int toX, Sachovnice* s)
{

	if (fromY != toY && fromX != toX) return false;

	int smerX = (toX > fromX) ? 1 : (toX < fromX ? -1 : 0);
	int smerY = (toY > fromY) ? 1 : (toY < fromY ? -1 : 0);

	int x = fromX + smerX;
	int y = fromY + smerY;

	while (x != toX || y != toY)
	{
		if (s->jeFigurka(y, x)) return false;
		x += smerX;
		y += smerY;
	}


	if (!s->jeFigurka(toY, toX)) return true;
	if (s->jeNepritel(toY, toX, barva)) return true;

	return false;
}

