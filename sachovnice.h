#ifndef SACHOVNICE_H
#define SACHOVNICE_H
#include <SDL/SDL.h>
#include "grafika.h"
//#include"figurka.h"
#include <list>
#include <vector>
#include <chrono>
#include <thread>

class Figurka;
struct Tah {
    int fromX;
    int fromY;
    int toX;
    int toY;
    int hodnota;
    int priorita;
    bool rosada;
    bool promoce;
    int promoceTyp; // 1 rook, 2 knight, 3 bishop, 4 queen
    Figurka* promoce2;
    Figurka* vyhozena;
    bool enPassant;
    int zajataX;
    int zajataY;
    int predchoziEnPassantX;
    int predchoziEnPassantY;
    int predchoziPocetTahu;
    int predchoziPolotahy; // fifty-move halfmove clock before this move
};
struct Hashtable {
	int hloubka;
	Uint64 tahHash;
	int hodnota;
	int typ; // 0 exact, 1 lower bound, 2 upper bound
	int fromX, fromY, toX, toY;
};

// Phase-dependent material (units = tenths of a pawn, same as piece `hodnota`).
// Middlegame (MG) and endgame (EG) are interpolated by fazeHry().
struct HodnotyFigurek {
    double pesakMG, pesakEG;
    double kunMG, kunEG;
    double strelecMG, strelecEG;
    double vezMG, vezEG;
    double damaMG, damaEG;
    double kralMG, kralEG;

    static HodnotyFigurek vychozi();
    double material(int typ, double faze) const; // typ = kdoJsi() % 6
    double materialMG(int typ) const;
};

class Sachovnice
{
public:

    int hodnota;
    bool hraju;
    Sachovnice();
    // Creates an SDL-free, independent board intended for a search worker.
    Sachovnice(const Sachovnice& zdroj, bool jenHledani);
    ~Sachovnice();
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
    Obrazek Sach_Mat_Alert;
    int ramecekx;
    int rameceky;
    bool mozneTah[8][8];
    std::list<Tah> tahZpet;
    void tahniZpet();
    bool jePolickoOhrozeno(int x , int y, int barvaUtocnika );
    bool jeSach(int barvaKrale);
    void robot();
    // Search only: unlike robot(), this does not change the board.  It is safe
    // to call on a private snapshot from a worker thread.
    Tah najdiTahRobota();
    void pohni(int fromY, int fromX, int toY, int toX, int promoceTyp = 4);
    void nastavPromoci(int typ);
    int hodnotaTahu;
    int hodnotaSachovnice();
    int statickeHodnoceni();
    int negaMax(int hloubka, int alpha, int beta, int barva);
    int quiescence(int alpha, int beta, int barva, int ply);
    std::vector<Tah> generujTahy(int barva, bool jenBrani = false);
    bool tahZOteviraciKnihy(Tah& tah) const;
    bool doselCas() const;
    void tahniZpetuser();
    bool jeMat(int barvaKrale);
    bool jeRemizaOpakovanim() const;
    bool jeRemizaFifty() const;
    bool jeRemiza() const;
    void prank();
    void initZorbistTable();
    Uint64 table[8][8][12];
    Uint64 castleRights[16];
    Uint64 enPassantHash[65];
    void genHashForWholeTable();
    Uint64 cislogenerator();
    Uint64 hash = 0;
    Hashtable *pole;
    Uint64 blackToMove;
    int enPassantX;
    int enPassantY;
    int polotahyBezBrani; // halfmoves since pawn move or capture (fifty-move)
    std::vector<Uint64> historieHash; // positions after each committed move
    int vybranaPromoce;
    std::chrono::steady_clock::time_point konecHledani;
    bool vyprselCas;
    int pravaRosady() const;
    HodnotyFigurek hodnoty; // tunable piece values (MG/EG)
    // +1 = attack, -1 = defend for the given colour (clamped).
    double postoj(int barva) const;

private:
    // Search snapshots deliberately contain no SDL images.  SDL stays on the
    // UI thread while workers get fully independent chess state.
    Figurka* kopieFigurky(const Figurka* figurka, bool nactiGrafiku) const;
    double fazeHry() const; // 1 = opening/middlegame, 0 = endgame
    double materialBarvy(int barva, double faze) const;
    double hodnoceniPesaku(double faze) const;
    double hodnoceniKralu(double faze) const;
    double hodnoceniMinor(double faze) const;
    double hodnoceniBezpecnostiKrale(double faze) const;
    double hodnocenizonyDamy() const;
    double hodnoceniUtokObrana(double faze) const;
    void synchronizujHodnotyFigurek();

};

#endif // SACHOVNICE_H
