#include "kun.h"

Kun::Kun(const int barva, bool nactiGrafiku): Figurka(barva)
{
	if (nactiGrafiku && barva == BILAF)
	{
		figurka.nacti("kunb.png");
	}
	else if (nactiGrafiku && barva == CERNAF)
	{
		figurka.nacti("kunc.png");
	}
	hodnota = 30;

}
int Kun::kdoJsi() const{
	if(barva == CERNAF)
		return 8;
	if(barva == BILAF)
		return 2;
	return 0;
}
double Kun::hodnotaFigurky(int y, int x){
	double hodnotakone[8][8] =
	{
		{-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0},
		{-4.0, -2.0,  0.0,  0.0,  0.0,  0.0, -2.0, -4.0},
		{-3.0,  0.0,  1.0,  1.5,  1.5,  1.0,  0.0, -3.0},
		{-3.0,  0.5,  1.5,  2.0,  2.0,  1.5,  0.5, -3.0},
		{-3.0,  0.0,  1.5,  2.0,  2.0,  1.5,  0.0, -3.0},
		{-3.0,  0.5,  1.0,  1.5,  1.5,  1.0,  0.5, -3.0},
		{-4.0, -2.0,  0.0,  0.5,  0.5,  0.0, -2.0, -4.0},
		{-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0}
	};
	return hodnota + hodnotakone[y][x];
}
bool Kun::validniTah(int fromY, int fromX, int toY, int toX, Sachovnice* s)
{
	int dx = abs(toX - fromX);
	int dy = abs(toY - fromY);

	if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2)))
		return false;

	if (!s->jeFigurka(toY, toX))
		return true;

	if (s->jeNepritel(toY, toX, barva))
		return true;

	return false;
}
