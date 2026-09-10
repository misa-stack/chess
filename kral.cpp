
#include "kral.h"
#include "Vez.h"

Kral::Kral(const int barva, bool nactiGrafiku): Figurka(barva)
{
	if (nactiGrafiku && barva == BILAF)
	{
		figurka.nacti("kralb.png");
	}
	else if (nactiGrafiku && barva == CERNAF)
	{
		figurka.nacti("kralc.png");
	}

	hodnota = 900;
}
int Kral::kdoJsi() const{
	if(barva == CERNAF)
		return 11;
	if(barva == BILAF)
		return 5;
	return 0;
}
double Kral::hodnotaFigurky(int y, int x)
{

	double hodnotaB[8][8] ={
		{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
		{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
		{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
		{-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
		{-2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
		{-1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
		{2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0},
		{2.0, 3.0, 1.0, 0.0, 0.0, 1.0, 3.0, 2.0}
	    };
	double hodnotaC[8][8]=
	{
	    {2.0, 3.0, 1.0, 0.0, 0.0, 1.0, 3.0, 2.0},
	    {2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0},
	    {-1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
	    {-2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
	    {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
	    {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
	    {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
	    {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0}
	};

	if (barva == CERNAF)
	{
		return hodnotaC[y][x] + hodnota;
	}
	else if(barva == BILAF)
	{
		return hodnotaB[y][x] + hodnota;

	}
	return 0.0;
}
void Kral::pohlase(int fromY, int fromX,int toY,int toX, Sachovnice *s)
{
	int dy = abs(toY - fromY);
	int dx = abs(toX - fromX);
	if (fromX == 4 && dy == 0 && dx == 2 && tah == 0)
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


	if (fromX == 4 && dy == 0 && dx == 2 && tah == 0)
	{
		int nepritel = (barva == BILAF) ? CERNAF : BILAF;
		if (s->jeSach(barva)) return false;
		if (toX == 6) {
			Figurka* vez = s->pozice[toY][7];
			if (vez && vez->barva == barva && dynamic_cast<Vez*>(vez) && vez->tah == 0) {
				if (!s->jeFigurka(toY, 5) && !s->jeFigurka(toY, 6))
				{
					return !s->jePolickoOhrozeno(5, toY, nepritel) &&
					       !s->jePolickoOhrozeno(6, toY, nepritel);
				}
			}
		}

		if (toX == 2) {
			Figurka* vez = s->pozice[toY][0];
			if (vez && vez->barva == barva && dynamic_cast<Vez*>(vez) && vez->tah == 0) {
				if (!s->jeFigurka(toY, 1) && !s->jeFigurka(toY, 2) && !s->jeFigurka(toY, 3))
				{
					return !s->jePolickoOhrozeno(3, toY, nepritel) &&
					       !s->jePolickoOhrozeno(2, toY, nepritel);
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
