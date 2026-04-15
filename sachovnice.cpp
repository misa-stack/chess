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

	ramecek.nacti("ramecek.png");
	//ramecek.nacti("Sach_Alert.png");
	ramecekx = -1;
	rameceky = -1;

	for (int r = 0; r < 8; r++)
		for (int c = 0; c < 8; c++)
			mozneTah[r][c] = false;
}

bool Sachovnice::jeFigurka(int y, int x)
{
	return pozice[y][x] != NULL;
}

bool Sachovnice::jeNepritel(int y, int x, int barva)
{
	return pozice[y][x] != NULL && pozice[y][x]->barva != barva;
}

void Sachovnice::reset()
{
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

void Sachovnice::kresli()
{
	if(jeSach(barvicka))
	{
		barva(CERVENA);
		obdelnik(1,1,100,100);
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
int Sachovnice::bileBody(){
    int bilebody = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (pozice[r][c]) {
                if (pozice[r][c]->barva == BILAF)
                    bilebody += pozice[r][c]->hodnota;
            }
        }
    }
    return bilebody;
}
int Sachovnice::cerneBody(){

        int cernebody = 0;
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                if (pozice[r][c]) {
                    if (pozice[r][c]->barva == CERNAF)
                        cernebody += pozice[r][c]->hodnota;
                }
            }
        }
        return cernebody;
}
void Sachovnice::robot()
{
    std::vector<Tah> nejlepsiTahy;
    int maxHodnota = -1;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (pozice[r][c] && pozice[r][c]->barva == barvicka) {
                Figurka* f = pozice[r][c];
                for (int y = 0; y < 8; y++) {
                    for (int x = 0; x < 8; x++) {
                        if (f->validniTah(r, c, y, x, this) && f->validniTahSach(r, c, y, x, this)) {
                            int hodnota = pozice[y][x] ? pozice[y][x]->hodnota : 0;

                            if (hodnota > maxHodnota) {
                                maxHodnota = hodnota;
                                nejlepsiTahy.clear();
                            }

                            if (hodnota == maxHodnota) {
                                Tah t;
                                t.fromX = c; t.fromY = r;
                                t.toX = x;   t.toY = y;
                                t.vyhozena = pozice[y][x];
                                t.rosada = false;
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
        Figurka* f = pozice[vybrany.fromY][vybrany.fromX];

        tahZpet.push_back(vybrany);
        pozice[vybrany.toY][vybrany.toX] = f;
        pozice[vybrany.fromY][vybrany.fromX] = NULL;

        f->pohlase(vybrany.fromY, vybrany.fromX, vybrany.toY, vybrany.toX, this);
        barvicka = (barvicka == BILAF) ? CERNAF : BILAF;
        vybrano = false;
    }
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




					}}
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
			Tah aktualniTah;
			int dy = abs(vybrana[0] -y);
			int dx = abs(vybrana[1] -x);
			aktualniTah.fromX = vybrana[1];
			aktualniTah.fromY = vybrana[0];
			aktualniTah.toX = x;
			aktualniTah.toY = y;
			aktualniTah.vyhozena = pozice[y][x];
			Kral* kral = dynamic_cast<Kral*>(pozice[y][x]);
			if(kral != NULL){
				if(dy == 0 && dx == 2 && x == 6){
					aktualniTah.rosada = true;
				}
				if(dy == 0 && dx == 2 && x == 2){
					aktualniTah.rosada = true;
				}
			}
			aktualniTah.rosada = true;
			tahZpet.push_back(aktualniTah);
			pozice[y][x] = f;
			pozice[vybrana[0]][vybrana[1]] = NULL;
			if(!jeSach(barvicka))
			{
				f->pohlase(vybrana[0],vybrana[1],y,x,this);
				barvicka = (barvicka == BILAF) ? CERNAF : BILAF;
			}
			else
			{
				tahniZpet();
				barvicka = (barvicka == BILAF) ? CERNAF : BILAF;

			}
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
void Sachovnice::tahniZpet(){
	Tah posledni = tahZpet.back();
	bool jelistprazdny = tahZpet.empty();

	if(jelistprazdny)
	{
		return;
	}

	tahZpet.pop_back();
	pozice[posledni.fromY][posledni.fromX] = pozice[posledni.toY][posledni.toX];
	pozice[posledni.fromY][posledni.fromX]->tah -= 1;
	pozice[posledni.toY][posledni.toX] = posledni.vyhozena;

	if(posledni.rosada){
		if(posledni.toX == 6){
			Figurka* vez = pozice[posledni.toY][5];
			pozice[posledni.toY][7] = vez;
			pozice[posledni.toY][7]->tah -=1;
			pozice[posledni.fromY][5] = NULL;
		}
		if(posledni.toX == 2){
			Figurka* vez = pozice[posledni.toY][3];
			pozice[posledni.toY][0] = vez;
			pozice[posledni.toY][0]->tah -=1;
			pozice[posledni.fromY][3] = NULL;
		}
	}
	barvicka = (barvicka == BILAF) ? CERNAF : BILAF;



}

