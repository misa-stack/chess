#include "kral.h"

Kral::Kral(const int barva): Figurka(barva)
{
	if (barva == BILAF)
	{
		figurka.nacti("kralb.png");
	}
	else if (barva == CERNAF)
	{
		figurka.nacti("kralc.png");
	}

	hodnota = 90;
}
void Kral::pohlase(int fromY, int fromX,int toY,int toX, Sachovnice *s)
{
	int dy = abs(toY - fromY);
	if (dy == 0 && tah == 0)
	{
		if (toX == 6) {
			Figurka* vez = s->pozice[toY][7];
			if (vez && vez->tah == 0) {
				s->pozice[toY][5] = vez;
				s->pozice[fromY][7] = NULL;
				vez->pohlase(fromY,7,toY,5,s);

			}
		}

		if (toX == 2) {
			Figurka* vez = s->pozice[toY][0];
			if (vez && vez->tah == 0) {
				s->pozice[toY][3] = vez;
				s->pozice[fromY][0] = NULL;
				vez->pohlase(fromY,0,toY,3,s);
			}
		}
	}

tah++;
}
bool Kral::validniTah(int fromY, int fromX, int toY, int toX, Sachovnice *s)
{
	int dx = abs(toX - fromX);
	int dy = abs(toY - fromY);

	if (dy == 0 && tah == 0)
	{
		if (toX == 6) {
			Figurka* vez = s->pozice[toY][7];
			if (vez && vez->tah == 0) {
				if (!s->jeFigurka(toY, 5) && !s->jeFigurka(toY, 6))
				{
					return true;
				}
			}
		}

		if (toX == 2) {
			Figurka* vez = s->pozice[toY][0];
			if (vez && vez->tah == 0) {
				if (!s->jeFigurka(toY, 1) && !s->jeFigurka(toY, 2) && !s->jeFigurka(toY, 3))
				{
					return true;
				}
			}
		}
	}

	//int barvaUtocnika = (barva == BILAF) ? CERNAF : BILAF;
	//if(s->jePolickoOhrozeno(toX,toY,barvaUtocnika)) return false;

	if (dx > 1 || dy > 1)
		return false;

	if (dx == 0 && dy == 0)
		return false;

	if (!s->jeFigurka(toY, toX))
		return true;

	if (s->jeNepritel(toY, toX, barva))
		return true;

	return false;
}


