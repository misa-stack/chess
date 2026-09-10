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
#include <algorithm>
#include <chrono>
#include <atomic>
#include <thread>
#include <cmath>

static const int TRANSPOZICNI_VELIKOST = 1 << 17;
static const int TRANSPOZICNI_MASKA = TRANSPOZICNI_VELIKOST - 1;

HodnotyFigurek HodnotyFigurek::vychozi()
{
    HodnotyFigurek h;

    h.pesakMG = 10.0;   h.pesakEG = 13.0;
    h.kunMG = 32.0;     h.kunEG = 28.0;
    h.strelecMG = 33.0; h.strelecEG = 35.0;
    h.vezMG = 50.0;     h.vezEG = 55.0;
    h.damaMG = 90.0;    h.damaEG = 95.0;
    h.kralMG = 900.0;   h.kralEG = 900.0;
    return h;
}

double HodnotyFigurek::materialMG(int typ) const
{
    switch (typ) {
        case 0: return pesakMG;
        case 1: return vezMG;
        case 2: return kunMG;
        case 3: return strelecMG;
        case 4: return damaMG;
        case 5: return kralMG;
        default: return 0.0;
    }
}

double HodnotyFigurek::material(int typ, double faze) const
{
    double mg = materialMG(typ);
    double eg;
    switch (typ) {
        case 0: eg = pesakEG; break;
        case 1: eg = vezEG; break;
        case 2: eg = kunEG; break;
        case 3: eg = strelecEG; break;
        case 4: eg = damaEG; break;
        case 5: eg = kralEG; break;
        default: eg = 0.0; break;
    }
    if (faze < 0.0) faze = 0.0;
    if (faze > 1.0) faze = 1.0;
    return mg * faze + eg * (1.0 - faze);
}

static Figurka* vytvorPromoci(int typ, int barva)
{
    switch (typ) {
        case 1: return new Vez(barva);
        case 2: return new Kun(barva);
        case 3: return new Strelec(barva);
        default: return new Kralovna(barva);
    }
}

Sachovnice::Sachovnice()
{
    rozmery_sachovnice = 800;
    ctverecky = 8;
    strana_ctverecku = rozmery_sachovnice / ctverecky;
    vybrano = false;
    barvicka = BILAF;
    enPassantX = -1;
    enPassantY = -1;
    polotahyBezBrani = 0;
    vybranaPromoce = 4;
    konecHledani = std::chrono::steady_clock::time_point::max();
    vyprselCas = false;
    hodnoty = HodnotyFigurek::vychozi();

    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            pozice[r][c] = NULL;

    initZorbistTable();
    reset();

    pole = new Hashtable[TRANSPOZICNI_VELIKOST];
    if(!pole)
    {
        printf("error");
    }
    memset(pole, 0, TRANSPOZICNI_VELIKOST * sizeof(Hashtable));

    ramecek.nacti("ramecek.png");
    Sach_Alert.nacti("sach.png");
    Sach_Mat_Alert.nacti("sachmat.png");
    ramecekx = -1;
    rameceky = -1;

    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            mozneTah[r][c] = false;
}

Sachovnice::~Sachovnice()
{
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            delete pozice[r][c];
    while (!tahZpet.empty()) {
        Tah t = tahZpet.back();
        tahZpet.pop_back();
        delete t.vyhozena;
    }
    delete[] pole;
}

Figurka* Sachovnice::kopieFigurky(const Figurka* f, bool nactiGrafiku) const
{
    if (!f) return NULL;
    Figurka* kopie = NULL;
    switch (f->kdoJsi() % 6) {
        case 0: kopie = new Pesak(f->barva, nactiGrafiku); break;
        case 1: kopie = new Vez(f->barva, nactiGrafiku); break;
        case 2: kopie = new Kun(f->barva, nactiGrafiku); break;
        case 3: kopie = new Strelec(f->barva, nactiGrafiku); break;
        case 4: kopie = new Kralovna(f->barva, nactiGrafiku); break;
        case 5: kopie = new Kral(f->barva, nactiGrafiku); break;
    }
    if (kopie) kopie->tah = f->tah;
    return kopie;
}

void Sachovnice::synchronizujHodnotyFigurek()
{
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Figurka* f = pozice[r][c];
            if (f) f->hodnota = hodnoty.materialMG(f->kdoJsi() % 6);
        }
}

Sachovnice::Sachovnice(const Sachovnice& zdroj, bool jenHledani)
{
    (void)jenHledani;
    rozmery_sachovnice = zdroj.rozmery_sachovnice;
    ctverecky = zdroj.ctverecky;
    strana_ctverecku = zdroj.strana_ctverecku;
    hodnota = zdroj.hodnota;
    hraju = zdroj.hraju;
    barvicka = zdroj.barvicka;
    vybrano = false;
    vybranaPromoce = zdroj.vybranaPromoce;
    enPassantX = zdroj.enPassantX;
    enPassantY = zdroj.enPassantY;
    polotahyBezBrani = zdroj.polotahyBezBrani;
    historieHash = zdroj.historieHash;
    hodnoty = zdroj.hodnoty;
    konecHledani = std::chrono::steady_clock::time_point::max();
    vyprselCas = false;
    memcpy(table, zdroj.table, sizeof(table));
    memcpy(castleRights, zdroj.castleRights, sizeof(castleRights));
    memcpy(enPassantHash, zdroj.enPassantHash, sizeof(enPassantHash));
    blackToMove = zdroj.blackToMove;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            pozice[r][c] = kopieFigurky(zdroj.pozice[r][c], false);
            mozneTah[r][c] = false;
        }
    pole = new Hashtable[TRANSPOZICNI_VELIKOST];
    memset(pole, 0, TRANSPOZICNI_VELIKOST * sizeof(Hashtable));
    genHashForWholeTable();
}

Uint64 Sachovnice::cislogenerator()
{
    return ((Uint64)rand())^((Uint64)rand()<<10)^((Uint64)rand()<<33);
}

void Sachovnice::initZorbistTable()
{
    for(int r = 0; r < 8; r++)
        for(int c = 0; c < 8; c++)
            for(int p = 0; p < 12; p++)
                table[r][c][p] = cislogenerator();

    blackToMove = cislogenerator();
    for (int i = 0; i < 16; ++i)
        castleRights[i] = cislogenerator();
    for (int i = 0; i < 65; ++i)
        enPassantHash[i] = cislogenerator();
}

bool Sachovnice::jeMat(int barvaKrale) {
    if (!jeSach(barvaKrale)) return false;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (pozice[r][c] && pozice[r][c]->barva == barvaKrale) {
                for (int y = 0; y < 8; y++) {
                    for (int x = 0; x < 8; x++) {
                        if (pozice[r][c]->validniTah(r, c, y, x, this) &&
			    pozice[r][c]->validniTahSach(r, c, y, x, this) && jeSach(barvaKrale)) {
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
    while (!tahZpet.empty())
    {
	    Tah t = tahZpet.back();
	    if(t.vyhozena) delete t.vyhozena;
	    tahZpet.pop_back();
    }

    vybrano = false;
    barvicka = BILAF;
    enPassantX = -1;
    enPassantY = -1;
    polotahyBezBrani = 0;
    historieHash.clear();
    hash = 0;
    genHashForWholeTable();
    historieHash.push_back(hash);
    synchronizujHodnotyFigurek();

    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            mozneTah[r][c] = false;
}

void Sachovnice::genHashForWholeTable()
{
    hash = 0;
    for(int r = 0; r < 8; r++)
    {
        for(int c = 0; c < 8; c++)
        {
            if(pozice[r][c] != NULL)
            {
                int whoami = pozice[r][c]->kdoJsi();
                hash = hash ^ table[r][c][whoami];
            }
        }
    }
    if(barvicka == CERNAF) hash ^= blackToMove;
    hash ^= castleRights[pravaRosady()];
    int enPassantIndex = (enPassantX >= 0 && enPassantY >= 0) ? enPassantY * 8 + enPassantX : 64;
    hash ^= enPassantHash[enPassantIndex];
}

int Sachovnice::pravaRosady() const
{
    int prava = 0;
    for (int barva = BILAF; barva <= CERNAF; ++barva) {
        int rada = (barva == BILAF) ? 7 : 0;
        Kral* kral = dynamic_cast<Kral*>(pozice[rada][4]);
        if (!kral || kral->barva != barva || kral->tah != 0) continue;
        Figurka* leva = pozice[rada][0];
        Figurka* pravaVez = pozice[rada][7];
        if (leva && leva->barva == barva && dynamic_cast<Vez*>(leva) && leva->tah == 0)
            prava |= (barva == BILAF) ? 1 : 4;
        if (pravaVez && pravaVez->barva == barva && dynamic_cast<Vez*>(pravaVez) && pravaVez->tah == 0)
            prava |= (barva == BILAF) ? 2 : 8;
    }
    return prava;
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
    if (jeRemiza()) return 0;
    int hodnota = statickeHodnoceni();
    if (jeMat(BILAF))  hodnota -= 99999;
    if (jeMat(CERNAF)) hodnota += 99999;
    return hodnota;
}

bool Sachovnice::jeRemizaFifty() const
{
    return polotahyBezBrani >= 100;
}

bool Sachovnice::jeRemizaOpakovanim() const
{
    if (historieHash.empty()) return false;
    Uint64 aktualni = historieHash.back();
    int pocet = 0;
    for (Uint64 h : historieHash)
        if (h == aktualni) ++pocet;
    return pocet >= 3;
}

bool Sachovnice::jeRemiza() const
{
    return jeRemizaFifty() || jeRemizaOpakovanim();
}

double Sachovnice::fazeHry() const
{
    int material = 0;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Figurka* f = pozice[r][c];
            if (!f) continue;
            switch (f->kdoJsi() % 6) {
                case 1: material += 2; break; // rook
                case 2: material += 1; break; // knight
                case 3: material += 1; break; // bishop
                case 4: material += 4; break; // queen
                default: break;
            }
        }
    const double maxMaterial = 24.0;
    double faze = material / maxMaterial;
    if (faze > 1.0) faze = 1.0;
    return faze;
}

double Sachovnice::hodnoceniPesaku(double faze) const
{
    double skore = 0.0;
    const double endgame = 1.0 - faze;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Figurka* f = pozice[r][c];
            if (!f || f->kdoJsi() % 6 != 0) continue;
            const int barva = f->barva;
            const int znamenko = (barva == BILAF) ? 1 : -1;
            const int smer = (barva == BILAF) ? -1 : 1;

            bool vedle = false;
            for (int dc = -1; dc <= 1; dc += 2) {
                int nx = c + dc;
                if (nx < 0 || nx > 7) continue;
                Figurka* n = pozice[r][nx];
                if (n && n->barva == barva && n->kdoJsi() % 6 == 0) vedle = true;
            }
            if (vedle) skore += znamenko * (0.15 + 0.15 * endgame);

            bool kryty = false;
            int zr = r - smer;
            if (zr >= 0 && zr < 8) {
                for (int dc = -1; dc <= 1; dc += 2) {
                    int nx = c + dc;
                    if (nx < 0 || nx > 7) continue;
                    Figurka* n = pozice[zr][nx];
                    if (n && n->barva == barva && n->kdoJsi() % 6 == 0) kryty = true;
                }
            }
            if (kryty) skore += znamenko * (0.12 + 0.18 * endgame);

            bool pred = false;
            for (int rr = r + smer; rr >= 0 && rr < 8; rr += smer) {
                for (int dc = -1; dc <= 1; ++dc) {
                    int nx = c + dc;
                    if (nx < 0 || nx > 7) continue;
                    Figurka* n = pozice[rr][nx];
                    if (n && n->barva == barva && n->kdoJsi() % 6 == 0) {
                        pred = true;
                        break;
                    }
                }
                if (pred) break;
            }
            if (pred) skore += znamenko * 0.08;

            bool volny = true;
            for (int rr = r + smer; rr >= 0 && rr < 8; rr += smer) {
                for (int dc = -1; dc <= 1; ++dc) {
                    int nx = c + dc;
                    if (nx < 0 || nx > 7) continue;
                    Figurka* n = pozice[rr][nx];
                    if (n && n->barva != barva && n->kdoJsi() % 6 == 0) {
                        volny = false;
                        break;
                    }
                }
                if (!volny) break;
            }
            if (volny) {
                int postup = (barva == BILAF) ? (6 - r) : (r - 1);
                if (postup < 0) postup = 0;
                double bonus = 0.25 + 0.35 * postup + 0.55 * postup * endgame;
                skore += znamenko * bonus;
            }

            bool sousedniSoubor = false;
            for (int dc = -1; dc <= 1; dc += 2) {
                int nx = c + dc;
                if (nx < 0 || nx > 7) continue;
                for (int rr = 0; rr < 8; ++rr) {
                    Figurka* n = pozice[rr][nx];
                    if (n && n->barva == barva && n->kdoJsi() % 6 == 0)
                        sousedniSoubor = true;
                }
            }
            if (!sousedniSoubor) skore -= znamenko * (0.18 + 0.12 * endgame);
        }
    }
    return skore;
}

double Sachovnice::hodnoceniKralu(double faze) const
{

    static const double midB[8][8] = {
        {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
        {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
        {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
        {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
        {-2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
        {-1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
        { 2.0,  2.0,  0.0,  0.0,  0.0,  0.0,  2.0,  2.0},
        { 2.0,  3.0,  1.0,  0.0,  0.0,  1.0,  3.0,  2.0}
    };
    static const double endC[8][8] = {
        {-5.0, -3.0, -1.0,  0.0,  0.0, -1.0, -3.0, -5.0},
        {-3.0, -1.0,  1.0,  2.0,  2.0,  1.0, -1.0, -3.0},
        {-1.0,  1.0,  3.0,  4.0,  4.0,  3.0,  1.0, -1.0},
        { 0.0,  2.0,  4.0,  5.0,  5.0,  4.0,  2.0,  0.0},
        { 0.0,  2.0,  4.0,  5.0,  5.0,  4.0,  2.0,  0.0},
        {-1.0,  1.0,  3.0,  4.0,  4.0,  3.0,  1.0, -1.0},
        {-3.0, -1.0,  1.0,  2.0,  2.0,  1.0, -1.0, -3.0},
        {-5.0, -3.0, -1.0,  0.0,  0.0, -1.0, -3.0, -5.0}
    };

    double skore = 0.0;
    const double endgame = 1.0 - faze;
    int wb[2] = {-1, -1}, cb[2] = {-1, -1};

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Figurka* f = pozice[r][c];
            if (!f || f->kdoJsi() % 6 != 5) continue;
            if (f->barva == BILAF) { wb[0] = r; wb[1] = c; }
            else { cb[0] = r; cb[1] = c; }

            double currentPst = (f->barva == BILAF) ? midB[r][c] : midB[7 - r][c];
            double mid = currentPst;
            double end = endC[r][c];
            double pst = mid * faze + end * endgame;
            double delta = pst - currentPst;
            skore += (f->barva == BILAF) ? delta : -delta;
        }
    }

    if (wb[0] >= 0 && cb[0] >= 0 && endgame > 0.45) {
        double wCent = -(std::abs(wb[0] - 3.5) + std::abs(wb[1] - 3.5));
        double cCent = -(std::abs(cb[0] - 3.5) + std::abs(cb[1] - 3.5));
        skore += (wCent - cCent) * 0.35 * endgame;
    }
    return skore;
}

double Sachovnice::hodnoceniMinor(double faze) const
{
    double skore = 0.0;
    const double endgame = 1.0 - faze;
    int bileStrelci = 0, cerneStrelci = 0;
    int bileKone = 0, cerneKone = 0;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Figurka* f = pozice[r][c];
            if (!f) continue;
            int typ = f->kdoJsi() % 6;
            if (typ == 3) {
                if (f->barva == BILAF) ++bileStrelci;
                else ++cerneStrelci;
                skore += (f->barva == BILAF ? 1 : -1) * (0.15 + 0.35 * endgame);
            } else if (typ == 2) {
                if (f->barva == BILAF) ++bileKone;
                else ++cerneKone;
                skore += (f->barva == BILAF ? 1 : -1) * (-0.05 - 0.25 * endgame);
            }
        }
    if (bileStrelci >= 2) skore += 0.35 + 0.25 * endgame;
    if (cerneStrelci >= 2) skore -= 0.35 + 0.25 * endgame;
    (void)bileKone; (void)cerneKone;
    return skore;
}

double Sachovnice::hodnoceniBezpecnostiKrale(double faze) const
{
    if (faze < 0.25) return 0.0;
    double skore = 0.0;
    for (int barva = BILAF; barva <= CERNAF; ++barva) {
        int kr = -1, kc = -1;
        for (int r = 0; r < 8; ++r)
            for (int c = 0; c < 8; ++c) {
                Figurka* f = pozice[r][c];
                if (f && f->barva == barva && f->kdoJsi() % 6 == 5) {
                    kr = r; kc = c;
                }
            }
        if (kr < 0) continue;
        const int znamenko = (barva == BILAF) ? 1 : -1;
        const int smer = (barva == BILAF) ? -1 : 1;

        int stit = 0;
        for (int dc = -1; dc <= 1; ++dc) {
            int x = kc + dc;
            int y = kr + smer;
            if (x < 0 || x > 7 || y < 0 || y > 7) continue;
            Figurka* p = pozice[y][x];
            if (p && p->barva == barva && p->kdoJsi() % 6 == 0) ++stit;
        }
        skore += znamenko * (stit - 2) * 0.25 * faze;

        for (int dc = -1; dc <= 1; ++dc) {
            int x = kc + dc;
            if (x < 0 || x > 7) continue;
            bool maPesaka = false;
            for (int y = 0; y < 8; ++y) {
                Figurka* p = pozice[y][x];
                if (p && p->barva == barva && p->kdoJsi() % 6 == 0) maPesaka = true;
            }
            if (!maPesaka) skore -= znamenko * 0.2 * faze;
        }

        int utoky = 0;
        for (int r = 0; r < 8; ++r)
            for (int c = 0; c < 8; ++c) {
                Figurka* f = pozice[r][c];
                if (!f || f->barva == barva) continue;
                int dist = std::max(abs(r - kr), abs(c - kc));
                int typ = f->kdoJsi() % 6;
                if (dist <= 3 && (typ == 4 || typ == 1)) ++utoky;
                if (dist <= 2 && (typ == 2 || typ == 3)) ++utoky;
            }
        skore -= znamenko * utoky * 0.22 * faze;
    }
    return skore;
}

double Sachovnice::hodnocenizonyDamy() const
{
    const double PENALTA = 0.35;
    const double PENALTA_KRAL = 0.7;
    double skore = 0.0;
    for (int qr = 0; qr < 8; ++qr) {
        for (int qc = 0; qc < 8; ++qc) {
            Figurka* dama = pozice[qr][qc];
            if (!dama || dama->kdoJsi() % 6 != 4) continue;
            for (int r = qr - 2; r <= qr + 1; ++r) {
                for (int c = qc - 2; c <= qc + 1; ++c) {
                    if (r < 0 || r > 7 || c < 0 || c > 7) continue;
                    if (r == qr && c == qc) continue;
                    Figurka* f = pozice[r][c];
                    if (!f || f->barva == dama->barva) continue;
                    double p = (f->kdoJsi() % 6 == 5) ? PENALTA_KRAL : PENALTA;
                    skore += (f->barva == BILAF) ? -p : p;
                }
            }
        }
    }
    return skore;
}

int Sachovnice::statickeHodnoceni()
{

    double skore = 0.0;
    for(int r = 0; r < 8; r++)
    {
        for(int c = 0; c < 8; c++)
        {
            if(pozice[r][c])
            {
                Figurka* f = pozice[r][c];
                if(f->barva == BILAF)
                    skore += f->hodnotaFigurky(r,c);
                if(f->barva == CERNAF)
                    skore -= f->hodnotaFigurky(r,c);
            }
        }
    }
    const double faze = fazeHry();
    skore += hodnoceniPesaku(faze);
    skore += hodnoceniKralu(faze);
    skore += hodnoceniMinor(faze);
    skore += hodnoceniBezpecnostiKrale(faze);
    skore += hodnocenizonyDamy();
    return static_cast<int>(skore * 10.0);
}

bool Sachovnice::doselCas() const
{
    return std::chrono::steady_clock::now() >= konecHledani;
}

bool Sachovnice::tahZOteviraciKnihy(Tah& tah) const
{

    struct KnihovniTah { int fromX, fromY, toX, toY; };
    static const KnihovniTah sicilska[] = {
        {4, 6, 4, 4}, {2, 1, 2, 3}, // 1. e4 c5
        {6, 7, 5, 5}, {3, 1, 3, 2}, // 2. Nf3 d6
        {3, 6, 3, 4}, {2, 3, 3, 4}, // 3. d4 cxd4
        {5, 5, 3, 4}, {6, 0, 5, 2}  // 4. Nxd4 Nf6
    };

    static const KnihovniTah italska[] = {
        {4, 6, 4, 4}, {4, 1, 4, 3}, // 1. e4 e5
        {6, 7, 5, 5}, {1, 0, 2, 2}, // 2. Nf3 Nc6
        {5, 7, 2, 4}, {5, 0, 2, 3}, // 3. Bc4 Bc5
        {4, 7, 6, 7}, {6, 0, 5, 2}  // 4. O-O Nf6
    };
    static const KnihovniTah londyn[] = {
        {3, 6, 3, 4}, {3, 1, 3, 3}, // 1. d4 d5
        {2, 7, 5, 4}, {6, 0, 5, 2}, // 2. Bf4 Nf6
        {4, 6, 4, 5}, {2, 1, 2, 3}, // 3. e3 c5
        {6, 7, 5, 5}, {1, 0, 2, 2}  // 4. Nf3 Nc6
    };
    static const KnihovniTah damin_gambit[] = {
        {3, 6, 3, 4}, {3, 1, 3, 3}, // 1. d4 d5
        {2, 6, 2, 4}, {4, 1, 4, 2}, // 2. c4 e6
        {1, 7, 2, 5}, {6, 0, 5, 2}  // 3. Nc3 Nf6
    };



    static const KnihovniTah viana[] = {
        {4, 6, 4, 4}, {4, 1, 4, 3}, // 1. e4 e5
        {6, 7, 4, 5}, {6, 0, 5, 2}, // 2. Nc3 Nf6
        {5, 6, 5, 5}, {1, 1, 1, 3}, // 3. f3 d6
        {2, 7, 5, 4}, {5, 0, 4, 2}  // 4. Bc4 Be7
    };

    static const KnihovniTah giuco_piano[] = {
        {4, 6, 4, 4}, {4, 1, 4, 3}, // 1. e4 e5
        {6, 7, 5, 5}, {1, 0, 2, 2}, // 2. Nf3 Nc6
        {5, 7, 2, 4}, {5, 0, 2, 3}, // 3. Bc4 Bc5
        {2, 6, 2, 4}, {6, 0, 5, 2}  // 4. c3 Nf6
    };

    const KnihovniTah* knihy[] = {sicilska, italska, londyn, damin_gambit , viana, giuco_piano};
    const size_t delky[] = {
        sizeof(sicilska) / sizeof(sicilska[0]),
        sizeof(italska) / sizeof(italska[0]),
        sizeof(londyn) / sizeof(londyn[0]),
        sizeof(damin_gambit) / sizeof(damin_gambit[0]),
        sizeof(viana) / sizeof(viana[0]),
        sizeof(giuco_piano) / sizeof(giuco_piano[0])
    };

    for (size_t k = 0; k < sizeof(knihy) / sizeof(knihy[0]); ++k) {
        if (tahZpet.size() >= delky[k]) continue;
        bool odpovida = true;
        size_t i = 0;
        for (std::list<Tah>::const_iterator it = tahZpet.begin(); it != tahZpet.end(); ++it, ++i) {
            const KnihovniTah& ocekavany = knihy[k][i];
            if (it->fromX != ocekavany.fromX || it->fromY != ocekavany.fromY ||
                it->toX != ocekavany.toX || it->toY != ocekavany.toY) {
                odpovida = false;
                break;
            }
        }
        if (odpovida) {
            const KnihovniTah& dalsi = knihy[k][tahZpet.size()];
            tah = {};
            tah.fromX = dalsi.fromX; tah.fromY = dalsi.fromY;
            tah.toX = dalsi.toX; tah.toY = dalsi.toY;
            tah.promoceTyp = 4;
            return true;
        }
    }
    return false;
}

std::vector<Tah> Sachovnice::generujTahy(int barva, bool jenBrani)
{
    bool zonaDamy[8][8] = {};
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Figurka* d = pozice[r][c];
            if (!d || d->barva == barva || d->kdoJsi() % 6 != 4) continue;
            for (int y = r - 2; y <= r + 1; ++y)
                for (int x = c - 2; x <= c + 1; ++x)
                    if (y >= 0 && y < 8 && x >= 0 && x < 8)
                        zonaDamy[y][x] = true;
        }

    std::vector<Tah> tahy;
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) {
        Figurka* f = pozice[r][c];
        if (!f || f->barva != barva) continue;
        for (int y = 0; y < 8; ++y) for (int x = 0; x < 8; ++x) {
            bool jePromoce = dynamic_cast<Pesak*>(f) && (y == 0 || y == 7);
            if (jenBrani && !pozice[y][x] && !jePromoce) continue;
            if (!f->validniTah(r, c, y, x, this) || !f->validniTahSach(r, c, y, x, this)) continue;
            Tah t = {};
            t.fromX = c; t.fromY = r; t.toX = x; t.toY = y;
            t.priorita = pozice[y][x] ? static_cast<int>(pozice[y][x]->hodnota * 100 - f->hodnota) : 0;
            if (!pozice[y][x] && zonaDamy[y][x])
                t.priorita -= 40;
            if (jePromoce) {

                for (int typ = 1; typ <= 4; ++typ) {
                    Tah p = t;
                    p.promoceTyp = typ;
                    p.priorita += 8000 + (typ == 4 ? 100 : 0);
                    tahy.push_back(p);
                }
            } else {
                t.promoceTyp = 4;
                tahy.push_back(t);
            }
        }
    }
    std::sort(tahy.begin(), tahy.end(), [](const Tah& a, const Tah& b) { return a.priorita > b.priorita; });
    return tahy;
}

int Sachovnice::quiescence(int alpha, int beta, int barva, int ply)
{
    if (doselCas()) { vyprselCas = true; return 0; }
    if (jeRemiza()) return 0;
    bool vSachu = jeSach(barva);
    int standPat = (barva == BILAF) ? statickeHodnoceni() : -statickeHodnoceni();
    if (!vSachu) {
        if (standPat >= beta) return beta;
        if (standPat > alpha) alpha = standPat;
    }
    std::vector<Tah> tahy = generujTahy(barva, !vSachu);
    if (tahy.empty()) {
        if (vSachu) return -99999 + ply;

        return generujTahy(barva).empty() ? 0 : alpha;
    }
    for (const Tah& t : tahy) {
        pohni(t.fromY, t.fromX, t.toY, t.toX, t.promoceTyp);
        int score = -quiescence(-beta, -alpha, barvicka, ply + 1);
        tahniZpet();
        if (vyprselCas) return 0;
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

int Sachovnice::negaMax(int hloubka, int alpha, int beta, int barva)
{
    if (doselCas()) { vyprselCas = true; return 0; }
    if (jeRemiza()) return 0;
    const int puvodniAlpha = alpha;
    int tableIndex = hash & TRANSPOZICNI_MASKA;
    Hashtable& entry = pole[tableIndex];
    if (entry.tahHash == hash && entry.hloubka >= hloubka)
    {
        if (entry.typ == 0) return entry.hodnota;
        if (entry.typ == 1) alpha = std::max(alpha, entry.hodnota);
        if (entry.typ == 2) beta = std::min(beta, entry.hodnota);
        if (alpha >= beta) return entry.hodnota;
    }
    if (hloubka == 0) return quiescence(alpha, beta, barva, 0);

    std::vector<Tah> tahy = generujTahy(barva);
    if (tahy.empty()) return jeSach(barva) ? -99999 : 0;
    if (entry.tahHash == hash) {
        std::stable_sort(tahy.begin(), tahy.end(), [&entry](const Tah& a, const Tah& b) {
            bool aTT = a.fromX == entry.fromX && a.fromY == entry.fromY && a.toX == entry.toX && a.toY == entry.toY;
            bool bTT = b.fromX == entry.fromX && b.fromY == entry.fromY && b.toX == entry.toX && b.toY == entry.toY;
            return aTT && !bTT;
        });
    }
    Tah nejlepsi = tahy[0];
    int maxEval = -100000;
    for (const Tah& t : tahy) {
        pohni(t.fromY, t.fromX, t.toY, t.toX, t.promoceTyp);
        int eval = -negaMax(hloubka - 1, -beta, -alpha, barvicka);
        tahniZpet();
        if (vyprselCas) return 0;
        if (eval > maxEval) { maxEval = eval; nejlepsi = t; }
        alpha = std::max(alpha, eval);
        if (alpha >= beta) break;
    }
    entry.tahHash = hash; entry.hloubka = hloubka; entry.hodnota = maxEval;
    entry.typ = (maxEval <= puvodniAlpha) ? 2 : (maxEval >= beta ? 1 : 0);
    entry.fromX = nejlepsi.fromX; entry.fromY = nejlepsi.fromY; entry.toX = nejlepsi.toX; entry.toY = nejlepsi.toY;
    return maxEval;
}

Tah Sachovnice::najdiTahRobota()
{
    std::vector<Tah> tahy = generujTahy(barvicka);
    if (tahy.empty()) { Tah zadny = {}; zadny.fromX = -1; return zadny; }

    Tah knihovniTah = {};
    if (tahZOteviraciKnihy(knihovniTah)) {

        for (const Tah& legalniTah : tahy) {
            if (legalniTah.fromX == knihovniTah.fromX && legalniTah.fromY == knihovniTah.fromY &&
                legalniTah.toX == knihovniTah.toX && legalniTah.toY == knihovniTah.toY) {
                return legalniTah;
            }
        }
    }
    Tah vybrany = tahy[0];
    konecHledani = std::chrono::steady_clock::now() + std::chrono::milliseconds(4000);
    for (int hloubka = 1; hloubka <= 32; ++hloubka) {
        struct Vysledek { int hodnota; bool vyprsel; };
        std::vector<Vysledek> vysledky(tahy.size(), Vysledek{-100000, false});
        std::atomic<size_t> dalsi(0);
        const unsigned jadra = std::thread::hardware_concurrency();

        const unsigned pocetPracovniku = std::min<unsigned>(
            tahy.size(), jadra > 1 ? jadra - 1 : 1);
        std::vector<std::thread> pracovnici;
        pracovnici.reserve(pocetPracovniku);
        for (unsigned i = 0; i < pocetPracovniku; ++i) {
            pracovnici.push_back(std::thread([&]() {
                Sachovnice lokalni(*this, true);
                lokalni.konecHledani = konecHledani;
                for (;;) {
                    size_t index = dalsi.fetch_add(1);
                    if (index >= tahy.size() || lokalni.doselCas()) break;
                    const Tah& t = tahy[index];
                    lokalni.pohni(t.fromY, t.fromX, t.toY, t.toX, t.promoceTyp);
                    int eval = -lokalni.negaMax(hloubka - 1, -100000, 100000, lokalni.barvicka);
                    lokalni.tahniZpet();
                    vysledky[index] = Vysledek{eval, lokalni.vyprselCas};
                }
            }));
        }
        for (std::thread& pracovnik : pracovnici) pracovnik.join();
        if (doselCas()) break;
        Tah kandidat = vybrany;
        int maxEval = -100000;
        bool neuplnaHloubka = false;
        for (size_t i = 0; i < tahy.size(); ++i) {
            if (vysledky[i].vyprsel) { neuplnaHloubka = true; break; }
            if (vysledky[i].hodnota > maxEval) {
                maxEval = vysledky[i].hodnota;
                kandidat = tahy[i];
            }
        }
        if (neuplnaHloubka) break;
        vybrany = kandidat;
        std::stable_sort(tahy.begin(), tahy.end(), [&vybrany](const Tah& a, const Tah& b) {
            bool aBest = a.fromX == vybrany.fromX && a.fromY == vybrany.fromY && a.toX == vybrany.toX && a.toY == vybrany.toY;
            bool bBest = b.fromX == vybrany.fromX && b.fromY == vybrany.fromY && b.toX == vybrany.toX && b.toY == vybrany.toY;
            return aBest && !bBest;
        });
    }
    return vybrany;
}

void Sachovnice::robot()
{
    Tah vybrany = najdiTahRobota();
    if (vybrany.fromX >= 0 && vybrany.fromX < 8)
        pohni(vybrany.fromY, vybrany.fromX, vybrany.toY, vybrany.toX, vybrany.promoceTyp);
}
void Sachovnice::pohni(int fromY, int fromX, int toY, int toX, int promoceTyp) {
    Figurka* f = pozice[fromY][fromX];
    if (!f) return;

    Tah aktualniTah = {};
    aktualniTah.fromX = fromX;
    aktualniTah.fromY = fromY;
    aktualniTah.toX = toX;
    aktualniTah.toY = toY;
    aktualniTah.vyhozena = pozice[toY][toX];
    aktualniTah.promoce = false;
    aktualniTah.promoceTyp = promoceTyp;
    aktualniTah.rosada = false;
    aktualniTah.enPassant = false;
    aktualniTah.zajataX = toX;
    aktualniTah.zajataY = toY;
    aktualniTah.predchoziEnPassantX = enPassantX;
    aktualniTah.predchoziEnPassantY = enPassantY;
    aktualniTah.predchoziPocetTahu = f->tah;
    aktualniTah.predchoziPolotahy = polotahyBezBrani;

    Pesak* pesak = dynamic_cast<Pesak*>(f);
    if (pesak && toX == enPassantX && toY == enPassantY &&
        pozice[toY][toX] == NULL && abs(toX - fromX) == 1) {
        aktualniTah.enPassant = true;
        aktualniTah.zajataX = toX;
        aktualniTah.zajataY = fromY;
        aktualniTah.vyhozena = pozice[fromY][toX];
    }

    // Castling rights are position state, not derivable from piece placement.
    hash ^= castleRights[pravaRosady()];
    int staryEnPassant = (enPassantX >= 0 && enPassantY >= 0) ? enPassantY * 8 + enPassantX : 64;
    hash ^= enPassantHash[staryEnPassant];

    hash ^= table[fromY][fromX][f->kdoJsi()];
    if (aktualniTah.vyhozena) {
        hash ^= table[aktualniTah.zajataY][aktualniTah.zajataX][aktualniTah.vyhozena->kdoJsi()];
    }

    if (pesak != NULL) {
        if ((pesak->barva == BILAF && toY == 0) || (pesak->barva == CERNAF && toY == 7)) {
            aktualniTah.promoce = true;
            aktualniTah.promoce2 = f;
        }
    }

    Kral* kral = dynamic_cast<Kral*>(f);
    if (kral != NULL) {
        int dy = abs(fromY - toY);
        int dx = abs(fromX - toX);
        if (dy == 0 && dx == 2) {
            aktualniTah.rosada = true;
        }
    }

    if (aktualniTah.promoce) {
        tahZpet.push_back(aktualniTah);
        pozice[toY][toX] = vytvorPromoci(promoceTyp, f->barva);
        pozice[fromY][fromX] = NULL;
        f->pohlase(fromY, fromX, toY, toX, this);
        delete f;
    } else {
        if (aktualniTah.rosada) {
            int rookFrom = (toX == 6) ? 7 : 0;
            if (pozice[toY][rookFrom]) hash ^= table[toY][rookFrom][pozice[toY][rookFrom]->kdoJsi()];
        }
        tahZpet.push_back(aktualniTah);
        pozice[toY][toX] = f;
        pozice[fromY][fromX] = NULL;
        f->pohlase(fromY, fromX, toY, toX, this);
        if (aktualniTah.rosada) {
            int rookTo = (toX == 6) ? 5 : 3;
            if (pozice[toY][rookTo]) hash ^= table[toY][rookTo][pozice[toY][rookTo]->kdoJsi()];
        }
    }

    if (aktualniTah.enPassant)
        pozice[aktualniTah.zajataY][aktualniTah.zajataX] = NULL;

    enPassantX = -1;
    enPassantY = -1;
    if (pesak && abs(toY - fromY) == 2) {
        enPassantX = fromX;
        enPassantY = (fromY + toY) / 2;
    }

    if (pesak || aktualniTah.vyhozena)
        polotahyBezBrani = 0;
    else
        ++polotahyBezBrani;

    hash ^= table[toY][toX][pozice[toY][toX]->kdoJsi()];

    hash ^= blackToMove;
    barvicka = (barvicka == BILAF) ? CERNAF : BILAF;
    hash ^= castleRights[pravaRosady()];
    int novyEnPassant = (enPassantX >= 0 && enPassantY >= 0) ? enPassantY * 8 + enPassantX : 64;
    hash ^= enPassantHash[novyEnPassant];
    historieHash.push_back(hash);
}

void Sachovnice::nastavPromoci(int typ)
{
    if (typ >= 1 && typ <= 4) vybranaPromoce = typ;
}

void Sachovnice::tahniZpet() {
    if (tahZpet.empty()) return;

    Tah posledni = tahZpet.back();
    tahZpet.pop_back();

    Figurka* f = pozice[posledni.toY][posledni.toX];
    if (!f) return;

    if (!historieHash.empty()) historieHash.pop_back();
    polotahyBezBrani = posledni.predchoziPolotahy;

    hash ^= castleRights[pravaRosady()];
    int aktualniEnPassant = (enPassantX >= 0 && enPassantY >= 0) ? enPassantY * 8 + enPassantX : 64;
    hash ^= enPassantHash[aktualniEnPassant];

    hash ^= table[posledni.toY][posledni.toX][f->kdoJsi()];

    if (posledni.promoce) {
	    int puvod = pozice[posledni.toY][posledni.toX]->barva;

		delete pozice[posledni.toY][posledni.toX];
		pozice[posledni.fromY][posledni.fromX] = new Pesak(puvod);
    }
    else {
        pozice[posledni.fromY][posledni.fromX] = f;
    }

    if (pozice[posledni.fromY][posledni.fromX]) {
        pozice[posledni.fromY][posledni.fromX]->tah = posledni.predchoziPocetTahu;
    }

    pozice[posledni.toY][posledni.toX] = NULL;
    if (posledni.enPassant)
        pozice[posledni.zajataY][posledni.zajataX] = posledni.vyhozena;
    else
        pozice[posledni.toY][posledni.toX] = posledni.vyhozena;

    if (posledni.rosada) {
        if (posledni.toX == 6) {
            Figurka* vez = pozice[posledni.toY][5];
            if (vez) hash ^= table[posledni.toY][5][vez->kdoJsi()];
            pozice[posledni.toY][7] = vez;
            if (vez) vez->tah -= 1;
            pozice[posledni.toY][5] = NULL;
            if (vez) hash ^= table[posledni.toY][7][vez->kdoJsi()];
        }
        if (posledni.toX == 2) {
            Figurka* vez = pozice[posledni.toY][3];
            if (vez) hash ^= table[posledni.toY][3][vez->kdoJsi()];
            pozice[posledni.toY][0] = vez;
            if (vez) vez->tah -= 1;
            pozice[posledni.toY][3] = NULL;
            if (vez) hash ^= table[posledni.toY][0][vez->kdoJsi()];
        }
    }

    hash ^= table[posledni.fromY][posledni.fromX][pozice[posledni.fromY][posledni.fromX]->kdoJsi()];

    if (posledni.vyhozena) {
        hash ^= table[posledni.zajataY][posledni.zajataX][posledni.vyhozena->kdoJsi()];
    }

    hash ^= blackToMove;
    barvicka = (barvicka == BILAF) ? CERNAF : BILAF;
    enPassantX = posledni.predchoziEnPassantX;
    enPassantY = posledni.predchoziEnPassantY;
    hash ^= castleRights[pravaRosady()];
    int obnovenyEnPassant = (enPassantX >= 0 && enPassantY >= 0) ? enPassantY * 8 + enPassantX : 64;
    hash ^= enPassantHash[obnovenyEnPassant];
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
            pohni(vybrana[0],vybrana[1],y,x,vybranaPromoce);
        }
    }

    vybrano = false;

    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            mozneTah[r][c] = false;
}

bool Sachovnice::jeSach(int barvaKrale){
    int kx = 0, ky = 0;
    bool kralNalezen = false;
    for(int r = 0; r < 8; r++){
        for(int c = 0; c < 8; c++){
            if(pozice[r][c] != NULL)
            {
                Kral* kral = dynamic_cast<Kral*>(pozice[r][c]);
                if(kral && kral->barva == barvaKrale){
			kx = c;
			ky = r;
			kralNalezen = true;
			goto ven;
                }
            }
        }
    }
ven:

    if (!kralNalezen) return true;
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

                    if (dynamic_cast<Pesak*>(f)) {
                        int smer = (barvaUtocnika == BILAF) ? -1 : 1;
                        if (y - r == smer && abs(x - c) == 1) return true;
                        continue;
                    }
                    if(f->validniTah(r,c,y,x,this))
                        return true;
                }
            }
        }
    }
    return false;
}


void Sachovnice::tahniZpetuser() {
    if (tahZpet.empty()) return;
    if (tahZpet.size() < 2) return;

    for(int i = 0; i < 2; ++i){
        tahniZpet();
    }
}
