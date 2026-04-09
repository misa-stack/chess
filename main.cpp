

#include <SDL/SDL.h>
#include "sachovnice.h"
#include "grafika.h"
#include "menu.h"

int main(int argc, char** argv)
{
	bool fullscreen = false;
	Obrazovka* obrazovka = Obrazovka::instance();
	obrazovka->inicializuj(1920, 1080, 0, fullscreen ? SDL_FULLSCREEN : 0);
	Menu m;
	Menu f;
	Menu cerneBody;
	Menu bileBody;
	Menu tahujuzpet;
	char bilebody[20];
	char cernebody[20];

	Sachovnice s;
	tahujuzpet.pridej(new Tlacitko("tah zpet",[&](){

	}));
	f.pridej(new Tlacitko("fullscreen",[&](){

	}));
	m.pridej(new Tlacitko("reset figurek", [&]() {
	}));

	while(1)
	{

		obrazovka->smaz();
		
		/* zacatek kresleni */
		s.robot();
		s.kresli();
		tahujuzpet.kresli(1200,750,1670,850);
		m.kresli(1200,150,1670,250);
		f.kresli(1200,300,1670,400);
		sprintf(bilebody,"%d",s.bileBody());
		sprintf(cernebody,"%d",s.cerneBody());
		bileBody.pridej(new Tlacitko(bilebody, [&]() {
		}));
		cerneBody.pridej(new Tlacitko(cernebody, [&]() {
		}));
		bileBody.kresli(1200,450,1670,550);
		cerneBody.kresli(1200,600,1670,700);
		SDL_Delay(1000);
		/* konec kresleni */
		obrazovka->aktualizuj();
		
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_MOUSEBUTTONDOWN:
				if(1199<event.button.x && event.button.x < 1671 && 750 < event.button.y && event.button.y < 850)
				{
					s.tahniZpet();
				}
				if(1199<event.button.x && event.button.x < 1671 && 150 < event.button.y && event.button.y < 250)
				{
					s.reset();
				}
				if(1199<event.button.x && event.button.x < 1671 && 300 < event.button.y && event.button.y < 400)
				{
					fullscreen = !fullscreen;
					obrazovka->inicializuj(1920, 1080, 0, fullscreen ? SDL_FULLSCREEN : 0);
				}
				s.klik(event.button.x,event.button.y);
				break;

			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					SDL_Quit();
					return 0;
				}
				break;
			}
		}
	}
}
