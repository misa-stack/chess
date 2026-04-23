
#include "kralovna.h"

Kralovna::Kralovna(const int barva): Figurka(barva)
{
	if (barva == BILAF)
	{
		figurka.nacti("kralovnab.png");
	}
	else if (barva == CERNAF)
	{
		figurka.nacti("kalovnac.png");
	}
	hodnota = 90;

}
double Kralovna::hodnotaFigurky(int y, int x){

	double hodnota[8][8] =
	{
	    {-2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0},
	    {-1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0},
	    {-1.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0},
	    {-0.5,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5},
	    {0.0,   0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5},
	    {-1.0,  0.5,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0},
	    {-1.0,  0.0,  0.5,  0.0,  0.0,  0.0,  0.0, -1.0},
	    {-2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0}
	};


			return hodnota[x][y] + this->hodnota;

}
bool Kralovna::validniTah(int fromY, int fromX, int toY, int toX, Sachovnice *s)
{
	int dx = toX - fromX;
	int dy = toY - fromY;

	if (!(dx == 0 || dy == 0 || abs(dx) == abs(dy)))
		return false;

	int smerX = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
	int smerY = (dy > 0) ? 1 : (dy < 0 ? -1 : 0);

	int x = fromX + smerX;
	int y = fromY + smerY;

	while (x != toX || y != toY)
	{
		if (s->jeFigurka(y, x))
			return false;

		x += smerX;
		y += smerY;
	}

	if (!s->jeFigurka(toY, toX))
		return true;

	return s->jeNepritel(toY, toX, barva);
}
