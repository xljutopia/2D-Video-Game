#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include "menuManager.h"
#include "manager.h"

MenuManager::MenuManager() :
  env( SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center")) ),
  screen( IOManager::getInstance().getScreen() ),
  clock( Clock::getInstance() ),
  backColor(),
  menu(),
  numberOfSprites(-1)
{ 
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw string("Unable to initialize SDL: ");
  }
  backColor.r = Gamedata::getInstance().getXmlInt("back/red");
  backColor.g = Gamedata::getInstance().getXmlInt("back/green");
  backColor.b = Gamedata::getInstance().getXmlInt("back/blue");
  atexit(SDL_Quit); 
}

void MenuManager::drawBackground() const {
  SDL_FillRect( screen, NULL, 
    SDL_MapRGB(screen->format, backColor.r, backColor.g, backColor.b) );
  SDL_Rect dest = {0, 0, 0, 0};
  SDL_BlitSurface( screen, NULL, screen, &dest );
}

void MenuManager::getNumberOfSprites() {
  IOManager& io = IOManager::getInstance().getInstance();
  SDL_Event event;
  bool done = false;
  bool nameDone = false;
  const string msg("How many sprites: ");
  io.clearString();
  while ( not done ) {
    Uint8 *keystate = SDL_GetKeyState(NULL);
    while ( SDL_PollEvent(&event) )
    switch (event.type) {
      case SDL_KEYDOWN: {
        if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
          done = true;
        }
        if (keystate[SDLK_RETURN]) {
          nameDone = true;
        }
        io.buildString(event);
      }
    }
    drawBackground();
    io.printStringAfterMessage(msg, 20, 120);
    if ( nameDone ) {
      std::string number = io.getString();
      std::stringstream strm;
      strm << number;
      strm >> numberOfSprites;
      strm.clear(); // clear error flags
      strm.str(std::string()); // clear contents
      strm << "Okay -- you'll see " << numberOfSprites << " monsters";
      io.printMessageAt(strm.str(), 20, 160);
      SDL_Flip(screen);
      SDL_Delay(1000);
      done = true;
    }
    if ( !done ) {
      SDL_Flip(screen);
    }
  }
}

void MenuManager::play() {
  SDL_Event event;
  bool done = false;
  // Here, we need to create an instance of the Manager,
  // the one that manages the game not the menu:
  Manager manager;
  manager.pause();

  while ( not done ) {
    drawBackground();
    menu.draw();
    SDL_Flip(screen);
    while ( SDL_PollEvent(&event) ) {
      if (event.type ==  SDL_QUIT) { break; }
      if(event.type == SDL_KEYDOWN) {
        std::cout << "KEY: " << event.key.keysym.sym << std::endl;
        switch ( event.key.keysym.sym ) {
          case SDLK_ESCAPE :
          case SDLK_q : {
            done = true;
            break;
          }
          case SDLK_RETURN : {
            menu.lightOn();
            if ( menu.getIconClicked() == "Start Game" ) {
              // Here is where we call the play() function in Manager;
              // but first, unpause the game:
              manager.unpause();
              manager.play();
              manager.pause();
            }
            if ( menu.getIconClicked() == "Parameters" ) {
              getNumberOfSprites();
              // After we get the number, we must set it to Manager:
              manager.setNumberOfSprites( numberOfSprites );
            }
            if ( menu.getIconClicked() == "Help" ) {
              // Here is where we explain how to play the game"
            	showHelp();  
			}
            if ( menu.getIconClicked() == "Exit" ) {
              drawBackground();
              menu.draw();
              SDL_Flip(screen);
              SDL_Delay(250);
              done = true;
            }
          }
          break;
          case SDLK_DOWN   : {
              menu.increment();
            break;
          }
          case SDLK_UP   : {
              menu.decrement();
            break;
          }
          default          : break;
        }
      }
      if(event.type == SDL_KEYUP) { 
        menu.lightOff();
      }
    }
  }
}

void MenuManager::showHelp(){
  IOManager& io = IOManager::getInstance().getInstance();
  SDL_Event event;
  bool done = false;
  const string msg("Goal: catch the yellow ball! ");
  const string rule("Rule: control the phoenix to catch the yellow ball; shoot monsters; if collided with a monster, ");
  const string rule1("the health decrease.");
  const string hint("Hint: key F2 can trigger a god mood, make a try!");
  io.clearString();
  while ( not done ) {
    Uint8 *keystate = SDL_GetKeyState(NULL);
    while ( SDL_PollEvent(&event) )
    switch (event.type) {
      case SDL_KEYDOWN: {
        if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
          done = true;
        }
        if (keystate[SDLK_RETURN]) {
          done = true;
        }
        io.buildString(event);
      }
    }
    drawBackground();
    io.printStringAfterMessage(msg, 20, 120);
	io.printStringAfterMessage(rule, 20, 150);
	io.printStringAfterMessage(rule1, 20, 180);
	io.printStringAfterMessage(hint, 20, 210);
	if ( !done ) {
      SDL_Flip(screen);
    }

}
}
