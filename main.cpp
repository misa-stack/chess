
#define BILAF 0
#define CERNAF 1

#include <time.h>
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
    Menu Body;
    char hodnota[20];

	Sachovnice s;
	tahujuzpet.pridej(new Tlacitko("tah zpet",[&](){
	}));

	f.pridej(new Tlacitko("fullscreen",[&](){

	}));

	m.pridej(new Tlacitko("reset figurek", [&]() {
	}));
	srand(time(NULL));
	while(1)
	{
        sprintf(hodnota,"%d",s.hodnota);
	sprintf(hodnota,"%d",s.hodnota);

        Body.pridej(new Tlacitko(hodnota,[&](){
        }));



		obrazovka->smaz();
		
		/* zacatek kresleni */
		s.kresli();
		if(s.barvicka == CERNAF){
	s.robot();
		}
		m.kresli(1200,150,1670,250);
		f.kresli(1200,300,1670,400);
        tahujuzpet.kresli(1200,450,1670,550);
        Body.kresli(1200,600,1670,750);
		/* konec kresleni */
		obrazovka->aktualizuj();
		
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_MOUSEBUTTONDOWN:
                if(1199<event.button.x && event.button.x < 1671 && 450 < event.button.y && event.button.y < 550)
				{
                    s.tahniZpetuser();
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
