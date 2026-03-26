#include "kun.h"

Kun::Kun(const int barva): Figurka(barva)
{
	if (barva == BILAF)
	{
		figurka.nacti("kunb.png");
	}
	else if (barva == CERNAF)
	{
		figurka.nacti("kunc.png");
	}
	hodnota = 3;

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
