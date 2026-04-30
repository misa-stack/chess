#ifndef SACHOVNICE_H
#define SACHOVNICE_H
#include <SDL/SDL.h>
#include "grafika.h"
//#include"figurka.h"
#include <list>

class Figurka;
struct Tah {
    int fromX;
    int fromY;
    int toX;
    int toY;
    int hodnota;
    bool rosada;
    bool promoce;
    Figurka* promoce2;
    Figurka* vyhozena;
};


class Sachovnice
{
public:

    int hodnota;
    bool hraju;
    Sachovnice();
    Figurka* pozice[8][8];
    int rozmery_sachovnice;
    int ctverecky;
    int strana_ctverecku;
    void kresli();
    void reset();
    void klik(int kx, int ky);
    int barvicka;
    int vybrana[2];
    bool vybrano;
    bool sudex;
    bool validniTahVez(int fromY, int fromX, int toY, int toX);
    bool jeNepritel(int x,int y, int barva);
    bool jeFigurka(int x, int y);
    int cerneBody();
    int bileBody();
    Obrazek ramecek;
    Obrazek Sach_Alert;
    int ramecekx;
    int rameceky;
    bool mozneTah[8][8];
    std::list<Tah> tahZpet;
    void tahniZpet();
    bool jePolickoOhrozeno(int x , int y, int barvaUtocnika );
    bool jeSach(int barvaKrale);
    void robot();
    void pohni(int fromY, int fromX, int toY, int toX);
    int hodnotaTahu;
    int hodnotaSachovnice();
    int negaMax(int hloubka, int alpha, int beta, int barva);
    void tahniZpetuser();
    bool jeMat(int barvaKrale);
    void prank();
    void zorbistTable();
    int table[8][8][12][5];


};

#endif // SACHOVNICE_H
