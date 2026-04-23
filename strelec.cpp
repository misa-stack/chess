#include "strelec.h"

Strelec::Strelec(const int barva): Figurka(barva)
{
	if (barva == BILAF)
	{
		figurka.nacti("strelecb.png");

	}
	else if (barva == CERNAF)
	{
		figurka.nacti("strelecc.png");
	}
	hodnota = 30;

}
double Strelec::hodnotaFigurky(int y, int x){
	double hodnotaC[8][8] =
	{
	    {-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0},
	    {-1.0,  0.5,  0.0,  0.0,  0.0,  0.0,  0.5, -1.0},
	    {-1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0, -1.0},
	    {-1.0,  0.0,  1.0,  1.0,  1.0,  1.0,  0.0, -1.0},
	    {-1.0,  0.5,  0.5,  1.0,  1.0,  0.5,  0.5, -1.0},
	    {-1.0,  0.0,  0.5,  1.0,  1.0,  0.5,  0.0, -1.0},
	    {-1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0},
	    {-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0}
	};
	double hodnotaB[8][8] =
	{
	    {-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0},
	    {-1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0},
	    {-1.0,  0.0,  0.5,  1.0,  1.0,  0.5,  0.0, -1.0},
	    {-1.0,  0.5,  0.5,  1.0,  1.0,  0.5,  0.5, -1.0},
	    {-1.0,  0.0,  1.0,  1.0,  1.0,  1.0,  0.0, -1.0},
	    {-1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0, -1.0},
	    {-1.0,  0.5,  0.0,  0.0,  0.0,  0.0,  0.5, -1.0},
	    {-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0}
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


bool Strelec::validniTah(int fromY, int fromX, int toY, int toX, Sachovnice *s)
{


	int smerX = (toX > fromX) ? 1 : (toX < fromX ? -1 : 0);
	int smerY = (toY > fromY) ? 1 : (toY < fromY ? -1 : 0);

	if (smerX == 0) return false;
	if (smerY == 0) return false;

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
	if (!s->jeNepritel(toY, toX, barva)) return false;

	return false;
}

