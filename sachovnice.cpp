#include <SDL/SDL.h>
#include "grafika.h"
#include "sachovnice.h"
#include "pesak.h"
#include "Vez.h"
#include "kun.h"
#include "strelec.h"
#include "kralovna.h"
#include "kral.h"
#include <vector>


Sachovnice::Sachovnice()
{
	rozmery_sachovnice = 800;
	ctverecky = 8;
	strana_ctverecku = rozmery_sachovnice / ctverecky;
	vybrano = false;
	barvicka = BILAF;

	for (int r = 0; r < 8; r++)
		for (int c = 0; c < 8; c++)
			pozice[r][c] = NULL;

	reset();
	pole = new Hashtable[1 << 25];
	if(!pole)
	{
		printf("error");
	}
	memset (pole, 0, 1 <<25 * sizeof(Hashtable));

	ramecek.nacti("ramecek.png");
	Sach_Alert.nacti("sach.png");
	Sach_Mat_Alert.nacti("sachmat.png");
	ramecekx = -1;
	rameceky = -1;
   //int table[8][8][12];
	for (int r = 0; r < 8; r++)
		for (int c = 0; c < 8; c++)
			mozneTah[r][c] = false;
}
Uint64 Sachovnice::cislogenerator()
{
	return ((Uint64)rand())^((Uint64)rand()<<10)^((Uint64)rand()<<33);
}
void Sachovnice::initZorbistTable()
{
	for(int r = 0;r<8;r++)
		for(int c = 0;c<8;c++)
			for(int p = 0;p<12;p++)
					table[r][c][p] = cislogenerator();

    blackToMove = cislogenerator();
}

bool Sachovnice::jeMat(int barvaKrale) {
	if (!jeSach(barvaKrale)) return false;

	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			if (pozice[r][c] && pozice[r][c]->barva == barvaKrale) {
				for (int y = 0; y < 8; y++) {
					for (int x = 0; x < 8; x++) {
						if (pozice[r][c]->validniTah(r, c, y, x, this) &&
								pozice[r][c]->validniTahSach(r, c, y, x, this)) {
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}
bool Sachovnice::jeFigurka(int y, int x)
{
	return pozice[y][x] != NULL;
}

bool Sachovnice::jeNepritel(int y, int x, int barva)
{
	return pozice[y][x] != NULL && pozice[y][x]->barva != barva;

}

void Sachovnice::prank()
{
	for (int r = 0; r < 8; r++)
	{
		for (int c = 0; c < 2; c++)
		{
			if (pozice[r][c] != NULL)
			{
				delete pozice[r][c];
				pozice[r][c] = NULL;
			}
			if(r == 0)
			{
				if(c == 4)
					new Kral(CERNAF);
			}
			else
			{
				new Kralovna(CERNAF);
			}

		}
	}
}


void Sachovnice::reset()
{
    genHashForWholeTable();
	for (int r = 0; r < 8; r++)
	{
		for (int c = 0; c < 8; c++)
		{
			if (pozice[r][c] != NULL)
			{
				delete pozice[r][c];
				pozice[r][c] = NULL;
			}

			int barvafigurky = (r < 2) ? CERNAF : BILAF;

			if (r == 1 || r == 6)
				pozice[r][c] = new Pesak(barvafigurky);
			else if (r == 0 || r == 7)
			{
				if (c == 0 || c == 7) pozice[r][c] = new Vez(barvafigurky);
				else if (c == 1 || c == 6) pozice[r][c] = new Kun(barvafigurky);
				else if (c == 2 || c == 5) pozice[r][c] = new Strelec(barvafigurky);
				else if (c == 3) pozice[r][c] = new Kralovna(barvafigurky);
				else if (c == 4) pozice[r][c] = new Kral(barvafigurky);
			}
		}
	}

	vybrano = false;
	barvicka = BILAF;

	for (int r = 0; r < 8; r++)
		for (int c = 0; c < 8; c++)
			mozneTah[r][c] = false;
}
void Sachovnice::genHashForWholeTable()
{
	for(int r = 0;r<8;r++)
		for(int c = 0;c<8;c++)
		{
			if(pozice[r][c] != NULL)
			{
				int whoami = pozice[r][c]->kdoJsi();
				hash = hash^table[r][c][whoami];
			}
		}
    if(barvicka == CERNAF) hash ^= blackToMove;
}
void Sachovnice::kresli()
{
	hodnota = hodnotaSachovnice();
	if(jeMat(barvicka))
	{
		Sach_Mat_Alert.umisti(300,1);
		Sach_Mat_Alert.kresli();
	}
	else if(jeSach(barvicka))
	{
		Sach_Alert.umisti(400,1);
		Sach_Alert.kresli();
		//barva(CERVENA);
		//obdelnik(1,1,100,100);
	}

	barva(101, 67, 33);
	obdelnik(80, 80, 920, 920);

	for (int r = 0; r < ctverecky; r++)
	{
		for (int c = 0; c < ctverecky; c++)
		{
			if ((r + c) % 2 == 1) barva(139, 69, 19);
			else barva(245, 245, 220);

			int x1 = strana_ctverecku + c * strana_ctverecku;
			int y1 = strana_ctverecku + r * strana_ctverecku;
			int x2 = x1 + strana_ctverecku - 1;
			int y2 = y1 + strana_ctverecku - 1;

			obdelnik(x1, y1, x2, y2);



			if (pozice[r][c])
				pozice[r][c]->kresli(x1 + 25, y1);
			if (mozneTah[r][c])
			{
				barva(180, 180, 180);
				kruh(x1 + strana_ctverecku / 2, y1 + strana_ctverecku / 2, 12);
			}
		}
	}

	if (vybrano)
	{
		int x1 = strana_ctverecku + ramecekx * strana_ctverecku;
		int y1 = strana_ctverecku + rameceky * strana_ctverecku;
		ramecek.umisti(x1, y1);
		ramecek.kresli();
	}
}

int Sachovnice::hodnotaSachovnice()
{
	int hodnota = 0;
	for(int r = 0; r < 8; r++)
		for(int c = 0; c < 8;c++)
		{
			if(pozice[r][c])
			{
				Figurka* f = pozice[r][c];
				if(f->barva == BILAF)
					hodnota += f->hodnotaFigurky(r,c);
				if(f->barva == CERNAF)
					hodnota -= f->hodnotaFigurky(r,c);
			}
		}
	if (jeMat(BILAF))  hodnota += 99999;
	if (jeMat(CERNAF)) hodnota -= 99999;
	return hodnota;


}
int Sachovnice::negaMax(int hloubka, int alpha, int beta, int barva)
{
	
	if (hloubka == 0)
	{
		int skore = hodnotaSachovnice();
		return (barva == BILAF) ? skore : -skore;
	}

	int maxEval = -99999;
	int eval;
	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			if (pozice[r][c] && pozice[r][c]->barva == barva) {
				for (int y = 0; y < 8; y++) {
					for (int x = 0; x < 8; x++) {
						if (pozice[r][c]->validniTah(r, c, y, x, this) &&
								pozice[r][c]->validniTahSach(r, c, y, x, this)) {
							pohni(r, c, y, x);

							if(pole[hash % 2000000].tahHash == hash && pole[hash % 2000000].hloubka == hloubka)
							{
								eval = pole[2000000% hash].hodnota;
							}

							else
							{
							pole[hash % 2000000].tahHash = hash;
							pole[hash % 2000000].hloubka = hloubka;
							pole[hash % 2000000].hodnota = -negaMax(hloubka - 1, -beta, -alpha, barvicka);
							int eval = pole[hash % 2000000].hodnota;
}

							tahniZpet();

							if (eval > maxEval) {
								maxEval = eval;
							}

							if (eval > alpha) {
								alpha = eval;
							}


							if (alpha >= beta) {
								return alpha;
							}
						}
					}
				}
			}
		}
	}
	return maxEval;
}
void Sachovnice::robot()
{
	std::vector<Tah> nejlepsiTahy;
	int maxEval = -99999;
	int hloubka = 5;

	int alpha = -100000;
	int beta = 100000;

	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			if (pozice[r][c] && pozice[r][c]->barva == barvicka) {
				for (int y = 0; y < 8; y++) {
					for (int x = 0; x < 8; x++) {
						if (pozice[r][c]->validniTah(r, c, y, x, this) && pozice[r][c]->validniTahSach(r, c, y, x, this)) {


							int eval = -negaMax(hloubka, -beta, -alpha, barvicka);
							tahniZpet();

							if (eval > maxEval) {
								maxEval = eval;
								nejlepsiTahy.clear();
								Tah t; t.fromX=c; t.fromY=r; t.toX=x; t.toY=y;
								nejlepsiTahy.push_back(t);

								if (eval > alpha) alpha = eval;

							} else if (eval == maxEval) {
								Tah t; t.fromX=c; t.fromY=r; t.toX=x; t.toY=y;
								nejlepsiTahy.push_back(t);
							}
						}
					}
				}
			}
		}
	}

	if (!nejlepsiTahy.empty()) {
		Tah vybrany = nejlepsiTahy[rand() % nejlepsiTahy.size()];
		pohni(vybrany.fromY, vybrany.fromX, vybrany.toY, vybrany.toX);
	}
}
void Sachovnice::pohni(int fromY, int fromX, int toY,int  toX){

	Figurka* f = pozice[fromY][fromX];
    int whoami = f->kdoJsi();
    hash ^= table[fromY][fromX][whoami];
    if (pozice[toY][toX] != NULL) {
        hash ^= table[toY][toX][pozice[toY][toX]->kdoJsi()];
    }
    Tah aktualniTah;
	int dy = abs(fromY -toY);
	int dx = abs(fromX -toX);
	aktualniTah.fromX = fromX;
	aktualniTah.fromY = fromY;
	aktualniTah.toX = toX;
	aktualniTah.toY = toY;
	aktualniTah.vyhozena = pozice[toY][toX];
	aktualniTah.promoce = false;
	Pesak* pesak = dynamic_cast<Pesak*>(f);
	if (pesak != NULL) {
		if ((pesak->barva == BILAF && toY == 0) || (pesak->barva == CERNAF && toY == 7)) {
			aktualniTah.promoce = true;
			aktualniTah.promoce2 = f;
		}
	}
	Kral* kral = dynamic_cast<Kral*>(f);
	aktualniTah.rosada = false;
	if(kral != NULL){
		if(dy == 0 && dx == 2 && toX == 6){
			aktualniTah.rosada = true;
		}
		if(dy == 0 && dx == 2 && toX == 2){
			aktualniTah.rosada = true;
		}
	}
	if (aktualniTah.promoce) {
		tahZpet.push_back(aktualniTah);
		pozice[toY][toX] = new Kralovna(f->barva);
		pozice[fromY][fromX] = NULL;
		f->pohlase(fromY,fromX,toY,toX,this);

	}
	else{
		tahZpet.push_back(aktualniTah);
		pozice[toY][toX] = f;
		pozice[fromY][fromX] = NULL;
		f->pohlase(fromY,fromX,toY,toX,this);
	}

    int f2 = pozice[toY][toX]->kdoJsi();
    hash ^= table[toY][toX][f2];
    hash ^= blackToMove;


	barvicka = (barvicka == BILAF) ? CERNAF : BILAF;
}
void Sachovnice::klik(int kx, int ky)
{
	int x = (kx - strana_ctverecku) / strana_ctverecku;
	int y = (ky - strana_ctverecku) / strana_ctverecku;

	if (x < 0 || x >= 8 || y < 0 || y >= 8) return;

	for (int r = 0; r < 8; r++)
		for (int c = 0; c < 8; c++)
			mozneTah[r][c] = false;

	if (!vybrano)
	{
		if (pozice[y][x] && pozice[y][x]->barva == barvicka)
		{
			vybrana[0] = y;
			vybrana[1] = x;
			vybrano = true;

			ramecekx = x;
			rameceky = y;

			Figurka* f = pozice[y][x];
			for (int r = 0; r < 8; r++)
				for (int c = 0; c < 8; c++)
					if (f->validniTah(y, x, r, c, this))
					{
						if(f->validniTahSach(y,x,r,c,this))
							mozneTah[r][c]=true;




					}
		}
		return;
	}

	if (vybrana[0] == y && vybrana[1] == x)
	{
		vybrano = false;
		return;
	}

	Figurka* f = pozice[vybrana[0]][vybrana[1]];
	bool tahJeValidni = f->validniTah(vybrana[0], vybrana[1], y, x, this);
	bool noSach = f->validniTahSach(vybrana[0], vybrana[1],y,x,this);

	if (tahJeValidni)
	{
		if(noSach)
		{
			pohni(vybrana[0],vybrana[1],y,x);
		}
	}

	vybrano = false;

	for (int r = 0; r < 8; r++)
		for (int c = 0; c < 8; c++)
			mozneTah[r][c] = false;
}
bool Sachovnice::jeSach(int barvaKrale){
	int kx,ky;
	for(int r = 0; r < 8; r++){
		for(int c = 0; c < 8; c++){
			if(pozice[r][c] != NULL)
			{
				Kral* kral = dynamic_cast<Kral*>(pozice[r][c]);
				if(kral && kral->barva == barvaKrale){
					kx =c;
					ky =r;
					goto ven;
				}
			}
		}
	}
ven:
	int barvaUtocnika = (barvaKrale == BILAF) ? CERNAF : BILAF;
	if(jePolickoOhrozeno(kx,ky,barvaUtocnika)) return true;

	return false;
}

bool Sachovnice::jePolickoOhrozeno(int x, int y, int barvaUtocnika){
	for(int r = 0; r < 8; r++){
		for(int c = 0; c < 8; c++){
			if(pozice[r][c] != NULL)
			{
				Figurka* f = pozice[r][c];
				if(f && f->barva == barvaUtocnika ){
					if(f->validniTah(r,c,y,x,this))
						return true;
				}
			}
		}
	}
	return false;
}
void Sachovnice::tahniZpet() {



	if (tahZpet.empty()) return;

	Tah posledni = tahZpet.back();
	tahZpet.pop_back();

    Figurka* f = pozice[posledni.toY][posledni.toX];
    hash ^= blackToMove;
    hash ^= table[posledni.toY][posledni.toX][f->kdoJsi()];
	if (posledni.promoce) {
		delete pozice[posledni.toY][posledni.toX];
		int barva = (barvicka == BILAF) ? CERNAF : BILAF;
		pozice[posledni.fromY][posledni.fromX] = new Pesak(barva);
	}

	pozice[posledni.fromY][posledni.fromX] = pozice[posledni.toY][posledni.toX];
	if (pozice[posledni.fromY][posledni.fromX]) {
		pozice[posledni.fromY][posledni.fromX]->tah -= 1;
	}
	pozice[posledni.toY][posledni.toX] = posledni.vyhozena;

	if (posledni.rosada) {
		if (posledni.toX == 6) {
			Figurka* vez = pozice[posledni.toY][5];
			pozice[posledni.toY][7] = vez;
			if (vez) vez->tah -= 1;
			pozice[posledni.toY][5] = NULL;
		}
		if (posledni.toX == 2) {
			Figurka* vez = pozice[posledni.toY][3];
			pozice[posledni.toY][0] = vez;
			if (vez) vez->tah -= 1;
			pozice[posledni.toY][3] = NULL;
		}
	}
    int f2 = posledni.promoce ? (f->barva == BILAF ? 0 : 6) : f->kdoJsi();
    hash ^= table[posledni.fromY][posledni.fromX][f2];

    if (posledni.vyhozena) {
        hash ^= table[posledni.toY][posledni.toX][posledni.vyhozena->kdoJsi()];
    }
    barvicka = (barvicka == BILAF) ? CERNAF : BILAF;
}
void Sachovnice::tahniZpetuser() {
	if (tahZpet.empty()) return;
	if (tahZpet.size() < 2) return;

	for(int i = 0; i<2; ++i){
		Tah posledni = tahZpet.back();
		tahZpet.pop_back();

		if (posledni.promoce) {
			delete pozice[posledni.toY][posledni.toX];
			int barva = (barvicka == BILAF) ? CERNAF : BILAF;
			pozice[posledni.fromY][posledni.fromX] = new Pesak(barva);
		}

		pozice[posledni.fromY][posledni.fromX] = pozice[posledni.toY][posledni.toX];
		if (pozice[posledni.fromY][posledni.fromX]) {
			pozice[posledni.fromY][posledni.fromX]->tah -= 1;
		}
		pozice[posledni.toY][posledni.toX] = posledni.vyhozena;

		if (posledni.rosada) {
			if (posledni.toX == 6) {
				Figurka* vez = pozice[posledni.toY][5];
				pozice[posledni.toY][7] = vez;
				if (vez) vez->tah -= 1;
				pozice[posledni.toY][5] = NULL;
			}
			if (posledni.toX == 2) {
				Figurka* vez = pozice[posledni.toY][3];
				pozice[posledni.toY][0] = vez;
				if (vez) vez->tah -= 1;
				pozice[posledni.toY][3] = NULL;
			}
		}
		barvicka = (barvicka == BILAF) ? CERNAF : BILAF;
	}
}
