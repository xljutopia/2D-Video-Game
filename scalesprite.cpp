#include "scalesprite.h"
#include "gamedata.h"
#include "frameFactory.h"
#include "SDL/SDL_rotozoom.h"
bool end = false;

void ScaleSprite::advanceFrame(Uint32 ticks) {
	timeSinceLastFrame += ticks;
	if (timeSinceLastFrame > frameInterval) {
    	currentFrame = (currentFrame+1) % (numberOfFrames/2);
		if( !booRight ) //turn to opposite direction frames
			currentFrame += (numberOfFrames/2);
		timeSinceLastFrame = 0;
	}
}

ScaleSprite::ScaleSprite( const std::string& name ) :
  Drawable(name, 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           Vector2f( (rand()%2?1:-1)*Gamedata::getInstance().getRandInRange(Gamedata::getInstance().getXmlInt(name+"/cushion"), Gamedata::getInstance().getXmlInt(name+"/speedX")),
                   (rand()%2?1:-1)*Gamedata::getInstance().getRandInRange(Gamedata::getInstance().getXmlInt(name+"/cushion"), Gamedata::getInstance().getXmlInt(name+"/speedY"))   
	)),
  scale( Gamedata::getInstance().getRandFloat(Gamedata::getInstance().getXmlFloat(name+"/scale/min"),   Gamedata::getInstance().getXmlFloat(name+"/scale/max")) ),
  frames( FrameFactory::getInstance().getFrames(name) ),
  chunkFrames(),
  chunks(),
  freeList(),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),

  currentFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
  timeSinceLastFrame( 0 ),
  frameWidth(frames[0]->getWidth()),
  frameHeight(frames[0]->getHeight()),
  booRight(true),
  isExploded(false),
  touchable(true) {
  	//makeChunks(Gamedata::getInstance().getXmlInt(name+"/chunk/size")); 
  }


  /*
ScaleSprite::ScaleSprite(const ScaleSprite& s) :
  Drawable(s), 
  scale(s.scale),
  frames(s.frames),
  worldWidth( s.worldWidth ),
  worldHeight( s.worldHeight ),
  currentFrame(s.currentFrame),
  numberOfFrames( s.numberOfFrames ),
  frameInterval( s.frameInterval ),
  timeSinceLastFrame( s.timeSinceLastFrame ),
  frameWidth( s.frameWidth ),
  frameHeight( s.frameHeight ),
  booRight( s.booRight )
  { }
*/

ScaleSprite::~ScaleSprite() {
//	SDL_FreeSurface( scaledSurface );
//	delete frame;
	for ( unsigned int i = 0; i < chunkFrames.size(); ++i ) {
    	delete chunkFrames[i]; // ExplodingSprite made them, so it deletes them
  }
	chunks.clear();
	freeList.clear();
}

void ScaleSprite::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frames[currentFrame]->draw(x, y);
}

void ScaleSprite::draw(double angle) const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frames[currentFrame]->draw(x, y, angle);
}

void ScaleSprite::drawZoom( )  { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  
  if(!isExploded) {
	  frames[currentFrame]->draw(x, y, scale);
  }
  else {
	std::list<Chunk>::const_iterator ptr = chunks.begin();
	while ( ptr != chunks.end() ) {
		ptr->draw();
		++ptr;
	}
  }
}

void ScaleSprite::update(Uint32 ticks) {
	if(!isExploded){
	  advanceFrame(ticks);

	  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
	  setPosition(getPosition() + incr);
	  
	  if ( Y() < 0) {
		velocityY( abs( velocityY() ) );
	  }
	  if ( Y() > worldHeight-frameHeight) {
		velocityY( -abs( velocityY() ) );
	  }

	  if ( X() < 0) {
		velocityX( abs( velocityX() ) );
	  }
	  if ( X() > worldWidth-frameWidth) {
		velocityX( -abs( velocityX() ) );
	  } 
	  
	  if ( velocityX() < 0 && booRight ) {
		booRight = false;
	  }
	  if ( velocityX() > 0 && !booRight ) {
		booRight = true;
	  }
	}
  
	else {
	  std::list<Chunk>::iterator ptr = chunks.begin();
	  while (ptr != chunks.end()) {
		ptr->update(ticks);
		if (ptr->goneTooFar()) {  // Check to see if we should free a chunk
		  freeList.push_back(*ptr);
		  ptr = chunks.erase(ptr);
		}   
		else ++ptr;
	  }
	  if ( chunkCount() == 0 ) {
	    end = true;
	  }
  	}
}
void ScaleSprite::setExploded() {
	isExploded = true;
	touchable = false;
	//set location for chunks
  	makeChunks(9); 
}

bool ScaleSprite::done() {
	if( isExploded && 0 == chunkCount() ){
		isExploded = false;
		touchable = true;
		return true;
	}
	else 
		return false;
}

void ScaleSprite::makeChunks(unsigned int n){ 
  // Break the SDL_Surface into n*n squares; where each square
  // has width and height of frameWidth/n and frameHeight/n
  unsigned int chunk_width = std::max(1u, getFrame()->getWidth()/n);
  unsigned int chunk_height = std::max(1u, getFrame()->getHeight()/n);
  int speedx = static_cast<int>(velocityX()); // Wanna test for zero...
  int speedy = static_cast<int>(velocityY()); // Make sure it's an int.
  if (speedx == 0) speedx = 1; // Make sure it's not 0;
  if (speedy == 0) speedy = 1; // Make sure it's not 0;

  // Get the SDL_Surface so we can chunk it:
  SDL_Surface* spriteSurface(getFrame()->getSurface()); 
  Sint16 source_y = getFrame()->getSourceY();
  while  (source_y < getFrame()->getHeight() ) {
    Sint16 source_x = getFrame()->getSourceX();
    while ( source_x < getFrame()->getWidth() ) {
      // Give each chunk it's own speed/direction:
      float sx = (rand() % speedx + 40) * (rand()%2?-1:1); // 'cause %0 is 
      float sy = (rand() % speedy + 40) * (rand()%2?-1:1); // float except
      Frame* frame = 
            new Frame(spriteSurface, chunk_width, chunk_height,
                  source_x, source_y
                );
	  //start = Vector2f(X(), Y() );
      Chunk chunk(
                Vector2f(X()+source_x,   // x coord of destination 
                         Y()+source_y), // y coord of destination
                Vector2f(sx, sy),
                getName()+"/chunk",
                frame);
      // chunks uses value semantics, as does frames, but there's
      // a big difference:
      chunks.push_back(chunk);
      chunkFrames.push_back(frame);
      source_x += chunk_width;
    }
    source_y += chunk_height;
  }
}

void ScaleSprite::reset(){
	isExploded = false;
	touchable = true;
	string name = Drawable::getName();
	setPosition(Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), Gamedata::getInstance().getXmlInt(name+"/startLoc/y")));

}
