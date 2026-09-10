
#define BILAF 0
#define CERNAF 1

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <time.h>
#include <SDL/SDL.h>
#include "sachovnice.h"
#include "grafika.h"
#include "menu.h"
#include <pthread.h>


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
	std::thread aiVlakno;
	std::mutex aiVysledekMutex;
	Tah aiVysledek = {};
	std::atomic<bool> aiPremysli(false);
	std::atomic<bool> aiHotovo(false);
	tahujuzpet.pridej(new Tlacitko("tah zpet",[&](){
	}));

	f.pridej(new Tlacitko("fullscreen",[&](){

	}));

	m.pridej(new Tlacitko("reset figurek", [&]() {
	}));

	Body.pridej(new Tlacitko(hodnota,[&](){
	}));
	srand(time(NULL));
	while(1)
	{
		// SDL remains on this (main) thread.  The AI gets an independent,
		// graphics-free snapshot and can use every other hardware thread.
        if (s.barvicka == CERNAF && !aiPremysli && !aiHotovo) {
			aiPremysli = true;
			aiVlakno = std::thread([&]() {
				Sachovnice hledani(s, true);
				Tah tah = hledani.najdiTahRobota();
				std::lock_guard<std::mutex> zamyk(aiVysledekMutex);
				aiVysledek = tah;
				aiHotovo = true;
			});
		}
		if (aiHotovo) {
			if (aiVlakno.joinable()) aiVlakno.join();
			Tah tah;
			{ std::lock_guard<std::mutex> zamyk(aiVysledekMutex); tah = aiVysledek; }
			if (tah.fromX >= 0 && tah.fromX < 8)
				s.pohni(tah.fromY, tah.fromX, tah.toY, tah.toX, tah.promoceTyp);
			aiHotovo = false;
			aiPremysli = false;
        }
        sprintf(hodnota,"%d",s.hodnota);
	sprintf(hodnota,"%d",s.hodnota);






		obrazovka->smaz();
		
		/* zacatek kresleni */
		s.kresli();

		//s.prank();

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
				// The source position must stay unchanged until its snapshot has
				// finished searching.
				if (aiPremysli) break;
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
				case SDLK_r:
					s.nastavPromoci(1); // rook
					break;
				case SDLK_n:
					s.nastavPromoci(2); // knight
					break;
				case SDLK_b:
					s.nastavPromoci(3); // bishop
					break;
				case SDLK_q:
					s.nastavPromoci(4); // queen (default)
					break;
				case SDLK_ESCAPE:
					if (aiVlakno.joinable()) aiVlakno.join();
					SDL_Quit();
					return 0;
				}
				break;
			}
		}
	}
}
