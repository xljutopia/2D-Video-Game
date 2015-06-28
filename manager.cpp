#include <iostream>
#include <algorithm>
#include <string>
#include <iomanip>
#include "player.h"
#include "gamedata.h"
#include "manager.h"
#include "sprite.h"

bool showHud = true;
bool isGod = false;
//bool stop = false;
bool first3seconds = true;
unsigned numOfExplosion = 0;
unsigned numOfCollision = 0;
unsigned numOfHit = 0;
unsigned numOfMonster = 0;
unsigned startTime = 0;

bool isWin=false;
bool isLose=false;

class ScaleSpriteCompare{
public:
	bool operator()(const ScaleSprite* lhs, const ScaleSprite* rhs){
		return lhs->getScale() < rhs->getScale();
	}
};

Manager::~Manager() { 
  for (unsigned i = 0; i < players.size(); ++i) {
    	delete players[i];
  }
 
  for (std::list<ScaleSprite*>::iterator it = fallings.begin(); it != fallings.end(); ++it) {
    	delete *it;
  }

}

Manager::Manager() :
  env( SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center")) ),
  io( IOManager::getInstance() ),
  clock( Clock::getInstance() ),
  sound(),	
  bar(),
  screen( io.getScreen() ),
  back("back", Gamedata::getInstance().getXmlInt("back/factor") ),
  front("front", Gamedata::getInstance().getXmlInt("front/factor") ),
  hud(),	
  finalResult(),
  viewport( Viewport::getInstance() ),
  players(),
  fallings(),
  currentSprite(0),

  makeVideo( false ),
  frameCount( 0 ),
  username(  Gamedata::getInstance().getXmlStr("username") ),
  title( Gamedata::getInstance().getXmlStr("screenTitle") ),
  frameMax( Gamedata::getInstance().getXmlInt("frameMax") ){
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw string("Unable to initialize SDL: ");
  }
  SDL_WM_SetCaption(title.c_str(), NULL);
  atexit(SDL_Quit);
  players.push_back( new Player("phoenix") );
  Sprite* player = static_cast<Sprite*>( players[0] );
 
  players.push_back( new SmartSprite("smart", *player ) );
  finalResult.setPosition(Vector2f(300, 200)); 
  finalResult.setShape(Vector2f(200, 80)); 

  makeOrbs();
  
  viewport.setObjectToTrack( players[ currentSprite] );
}

void Manager::makeOrbs(){
	unsigned numberOfOrbs = Gamedata::getInstance().getXmlInt("numberOfOrbs");
	//fallings.reserve( numberOfOrbs );
	for(unsigned i = 0; i < numberOfOrbs; i++)
		fallings.push_back( new ScaleSprite( "orbs" ) );
	//sort( fallings.begin(), fallings.end(), ScaleSpriteCompare() );
	fallings.sort( ScaleSpriteCompare() );
	numOfMonster = fallings.size();
}

void Manager::draw() const {
  back.draw();
  
  std::list<ScaleSprite*>::const_iterator it = fallings.begin();
  for (unsigned i=0; i < fallings.size()/2; ++it, ++i) {
     (*it)->drawZoom( );
  }

  front.draw();
  bar.draw();

  for (unsigned i = fallings.size()/2; i < fallings.size(); ++it, ++i) {
     (*it)->drawZoom( );
  }

  for (unsigned i = 0; i < players.size(); ++i) {
      if(!isWin && !isLose)
	  	players[i]->draw();
  }
  
  if(showHud){
	  hud.draw();
	  hud.draw(static_cast<Player*>(players[0])->bulletsCount(), static_cast<Player*>(players[0])->freesCount(), numOfMonster, numOfExplosion, numOfCollision) ;
	 /*unsigned currentTime = clock.getSeconds();
	  if((currentTime - startTime) > 2 && !first3seconds)
		  showHud = false;
		  */
  }
  io.printMessageAt(title, 10, 450);
  viewport.draw();
	if(isWin){
		unsigned score = 10*numOfHit;
		finalResult.draw("        YOU  WIN", score);
		//io.printMessageCenteredAt("WIN", 200);
	}
	if(isLose){
		finalResult.draw("   Sorry, YOU   LOSE", 100);
		//io.printMessageCenteredAt("LOSE", 200);
	}
  SDL_Flip(screen);
}

void Manager::setNumberOfSprites(int number) {
  if ( number > static_cast<int>( fallings.size() ) ) {
	  number = number - fallings.size();
    for (int i = 0; i < number; ++i) {
      fallings.push_back( new ScaleSprite( "orbs" ));
      //sprites.push_back( new Sprite("greenorb") );
    }
  }
  else {
    number = fallings.size() - number;
    for (int i = 0; i < number; ++i) {
      delete fallings.back();
      fallings.pop_back();
    }
  }
}

void Manager::makeFrame() {
  std::stringstream strm;
  strm << "frames/" << username<< '.' 
       << std::setfill('0') << std::setw(4) 
       << frameCount++ << ".bmp";
  std::string filename( strm.str() );
  std::cout << "Making frame: " << filename << std::endl;
  SDL_SaveBMP(screen, filename.c_str());
}
unsigned countt=0;
void Manager::reset(){
 //reset positions for objects
    players[0]->reset();
	players[1]->reset();
	bar.reset();
	for (std::list<ScaleSprite*>::iterator it = fallings.begin(); it != fallings.end(); ++it) {
    	(*it)->reset();
  	}
	isGod=false;
	isWin=false;
	isLose=false;
	numOfExplosion = 0;
	numOfCollision = 0;
	numOfHit = 0;
	countt=0;
	numOfMonster = fallings.size();

}



void Manager::update() {
  ++clock;
  Uint32 ticks = clock.getElapsedTicks();
  back.update();
  front.update();
  for (unsigned i = 0; i < players.size(); ++i) {
     players[i]->update(ticks);
  }
  if( static_cast<Player*>( players[0] )->done() ){
	//bar.reset();
	//std::cout<<"player reborn"<<std::endl;
  }
  numOfMonster = fallings.size();
  for (std::list<ScaleSprite*>::iterator it = fallings.begin(); it != fallings.end(); ++it) {
	 (*it)->update(ticks);
	 if( (*it)->done() ){
		//re-appear after explosion
		numOfExplosion--;
		numOfMonster++;
		/*
		if(!showHud){
			showHud = true;
			startTime = clock.getSeconds();
		}*/
	 }
	 if( !isWin && !isLose && (*it)->isTouchable() && static_cast<Player*>( players[0] )->collidedWith(*it) ){
		 //bullet shoot a monster
		numOfExplosion++;
		numOfCollision++;
		numOfMonster--;
        numOfHit++;
		/*
		if(!showHud){
			showHud = true;
			startTime = clock.getSeconds();
		}*/
		(*it)->setExploded();
		sound[3];
		//std::cout<<"collision"<<std::endl;
	 }
	 else if(  !isWin && !isLose && !isGod && (*it)->isTouchable( ) && static_cast<Player*>( players[0] )->isTouchable() && static_cast<Player*>( players[0] )->getHurt(*it) ) {
		 //player collide with a monster
		numOfExplosion++;
		numOfMonster--;
		numOfCollision++;
		/*
		if(!showHud){
			showHud = true;
			startTime = clock.getSeconds();
		}*/
		++countt;
		//std::cout<<"player hurt"<<++countt<<std::endl;
		bar.update(40);
		(*it)->setExploded();
		sound[3];
		if(countt%5 == 0){
			static_cast<Player*>( players[0] )->setExploded();	
			isLose=true;
			isGod=true;
			//break;
			//
			//std::cout<<"boom"<<std::endl;
		}
	 }
}
	//check win
	if( !isWin && !isLose &&  static_cast<Player*>( players[0] )->collidedWithSmart(static_cast<SmartSprite*>( players[1] )) ){
		std::cout<<"win"<<std::endl;
		isWin=true;
		isGod=false;
	}

  if ( makeVideo && frameCount < frameMax ) {
     makeFrame();
  }
  viewport.update(); // always update viewport last
}

void Manager::play() {
  SDL_Event event;

  bool done = false;
  bool keyCatch = false;
  while ( not done ) {

    Uint8 *keystate = SDL_GetKeyState(NULL);
	SDL_PollEvent(&event);
	if( clock.getSeconds() == 4){
       showHud = false;
	   first3seconds = false;
    }

	if (event.type ==  SDL_QUIT) { done = true; break; }
	if(event.type == SDL_KEYUP) { 
	  keyCatch = false; 
	}
	if(event.type == SDL_KEYDOWN) {
	  if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
		done = true;
		break;
	  }

	  if (keystate[SDLK_F1] && !keyCatch) {
		keyCatch = true;
		showHud = !showHud;
		//showHud = true;
	  }
	  if (keystate[SDLK_F2] && !keyCatch) {
		keyCatch = true;
		//toggle God mode
		isGod = !isGod;
		//bar.reset();
	  }

	  if (keystate[SDLK_o] && !keyCatch) {
		keyCatch = true;
		clock.toggleSloMo();
	  }
				
		if (keystate[SDLK_p] && !keyCatch) {
		  keyCatch = true;
		  if ( clock.isPaused() ) clock.unpause();
		  else clock.pause();
		}
		if (keystate[SDLK_F4] && !makeVideo) {
		  std::cout << "Making video frames" << std::endl;
		  makeVideo = true;
		}

		if(keystate[SDLK_r]&& !keyCatch){
			keyCatch = true;
			reset();
		}
		if(keystate[SDLK_SPACE] && !keyCatch ){
			keyCatch = true;
			static_cast<Player*>( players[0] )->shoot();	
			//sound[3];
		}
		
	}
	if( false == keyCatch ){	
		static_cast<Player*>( players[0] )->stop();
	//	std::cout<<"stop"<<std::endl;
	}
	if(keystate[SDLK_a] && keystate[SDLK_d]){
		static_cast<Player*>( players[0] )->stop();
	}
	else if(keystate[SDLK_a]){
		static_cast<Player*>( players[0] )->left();
	}
	else if(keystate[SDLK_d]){
		static_cast<Player*>( players[0] )->right();
	}
	if(keystate[SDLK_w]){
		static_cast<Player*>( players[0] )->up();
	}
	if(keystate[SDLK_s]){
		static_cast<Player*>( players[0] )->down();
	}

	draw();
	update();
  }
}

