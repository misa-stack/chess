#include "menu.h"
#include <list>
#include <functional>


Tlacitko::Tlacitko(const char* nazev, std::function<void()> funkce)
{

	this ->funkce = funkce;
	this ->nazev = nazev;
	cislapismenamalaivelka.nacti("cislapismenamalaivelka.png", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
}
void Tlacitko::kresli(int x1, int y1, int x2, int y2)
{

	barva(BILA);
	obdelnik(x1,y1,x2,y2);

	cislapismenamalaivelka.umisti(x1+5,y1);
	cislapismenamalaivelka.kresli(nazev);
}
Ukazatel::Ukazatel(const char* nazev,int& hodnota): nazev(nazev), hodnota(hodnota)
{
	cislapismenamalaivelka.nacti("cislapismenamalaivelka.png", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
}
void Ukazatel::kresli(int x1, int y1, int x2, int y2)
{

	barva(BILA);
	obdelnik(x1,y1,x2,y2);

	cislapismenamalaivelka.umisti(x1+5,y1);
	char text[20];
	sprintf(text, "%s: %d", nazev, hodnota);
	cislapismenamalaivelka.kresli(text);
}
Menu::Menu()
{
	prvnitlacitko=true;
	pocettlacitek=0;
	vzdalenostoddelenitlacitek =0;
	velikosttlaciteksirka = 0;
	velikosttlacitekvyska = 0;

}

void Menu::pridej(Tlacitko* t){
	tlacitka.push_back(t);
}
void Tlacitko::klik(){
	funkce;
}
void Menu::klik(int kx, int ky){
	int x = 100;
	int y = 0;
	int i = 0;



	pocettlacitek = tlacitka.size();
	for (auto t = tlacitka.begin(); t != tlacitka.end(); ++t)
	{

		(*t)->klik();
	}

}
void Menu::kresli(int xk, int yk,int xk2, int yk2){
	int x = 100;
	int y = 0;
	int i = 0;



	pocettlacitek = tlacitka.size();
	for (auto t = tlacitka.begin(); t != tlacitka.end(); ++t)
	{

		(*t)->kresli(xk, yk, xk2 , yk2);
		i++;
	}

}


