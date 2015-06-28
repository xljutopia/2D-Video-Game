#include "player.h"
#include "gamedata.h"
#include "frameFactory.h"

void Player::advanceFrame(Uint32 ticks) {
	timeSinceLastFrame += ticks;
	if (timeSinceLastFrame > frameInterval) {
    	currentFrame = (currentFrame+1) % (numberOfFrames/2);
		if( !booRight ) //turn to opposite direction frames
			currentFrame += (numberOfFrames/2);
		timeSinceLastFrame = 0;
	}
}

CollisionStrategy* Player::getStrategy(const string& name){
	std::string sName = Gamedata::getInstance().getXmlStr(name+"/Strategy");
	if(sName == "midpoint") return new MidPointCollisionStrategy;

	if(sName == "rectangular") return new RectangularCollisionStrategy;

	if(sName == "perpixel") return new PerPixelCollisionStrategy;
	throw std::string("No strategy in getStrategy");
}

Player::Player( const std::string& name) :
  Drawable(name, 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY"))
           ),
  frames( FrameFactory::getInstance().getFrames(name) ),
  chunkFrames(),
  chunks(),
  freeList(), 
  strategy(getStrategy(name)),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  keyPressedX(false),
  keyPressedY(false),
  initialVelocity(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY")),
  slowDown(Gamedata::getInstance().getXmlInt(name+"/slowDown")),	
  currentFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
  timeSinceLastFrame( 0 ),
  frameWidth(frames[0]->getWidth()),
  frameHeight(frames[0]->getHeight()),
  booRight(true),
  isExploded(false),
  touchable(true),
  bulletName(Gamedata::getInstance().getXmlStr(name+"/Bullet")),
  bullets(bulletName),
  minSpeed(Gamedata::getInstance().getXmlInt( bulletName+"/speedX"))
{ }
 
Player::Player(const Player& s) :
  Drawable(s), 
  frames(s.frames),
  chunkFrames(s.chunkFrames),
  chunks(s.chunks),
  freeList(s.freeList),
  strategy(s.strategy),
  worldWidth( s.worldWidth ),
  worldHeight( s.worldHeight ),
  keyPressedX( s.keyPressedX ),
  keyPressedY( s.keyPressedY ),
  initialVelocity( s.initialVelocity ),
  slowDown( s.slowDown ),
  currentFrame(s.currentFrame),
  numberOfFrames( s.numberOfFrames ),
  frameInterval( s.frameInterval ),
  timeSinceLastFrame( s.timeSinceLastFrame ),
  frameWidth( s.frameWidth ),
  frameHeight( s.frameHeight ),
  booRight( s.booRight ),
  isExploded( s.isExploded),
  touchable(s.touchable),
  bulletName( s.bulletName ),
  bullets( s.bullets ),
  minSpeed( s.minSpeed )
  { }

Player::~Player(){
	delete strategy;
	for ( unsigned int i = 0; i < chunkFrames.size(); ++i ) {
    	delete chunkFrames[i]; // ExplodingSprite made them, so it deletes them
    }
	
}

void Player::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  if(!isExploded){
  	frames[currentFrame]->draw(x, y);
  	bullets.draw();

  }
  else{
	std::list<Chunk>::const_iterator ptr = chunks.begin();
	while ( ptr != chunks.end() ) {
		ptr->draw();
		++ptr;
	}
  }
}

void Player::draw(double angle) const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frames[currentFrame]->draw(x, y, angle);
}

void Player::update(Uint32 ticks) { 
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
	  
	  if ( velocityX() < 0 /*&& booRight*/ ) {
		booRight = false;
	  }
	  if ( velocityX() > 0 /* && !booRight*/ ) {
		booRight = true;
	  }

	  if( !keyPressedX ) stop();
	  if( !keyPressedY ) stop();
	  keyPressedX = false;
	  keyPressedY = false;
	  
	  bullets.update(ticks);
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
	  
  }

}

void Player::left(){
	keyPressedX = true;
	if( X() > 0 ){
		velocityX( -initialVelocity[0] );
	}
}

void Player::right(){
	keyPressedX = true;
	if( X() < worldWidth-frameWidth){
		velocityX( initialVelocity[0] );
	}
}

void Player::up(){
	keyPressedY = true;
	if( Y()>0 ){
		velocityY( -initialVelocity[1] );
	}
}

void Player::down(){
	keyPressedY = true;
	if( Y() < worldHeight-frameHeight ){
		velocityY( initialVelocity[1] );
	}
}

void Player::stop(){
	if( !keyPressedX ) {
		velocityX( slowDown*velocityX() );
	    //std::cout<<"stop"<<std::endl;
	}
	if( !keyPressedY ) velocityY(0);
}

void Player::shoot(){
	float x = 0;
	if(booRight){
		x = X() + getFrame()->getWidth();
	}
	else{
		x = X() - getFrame()->getWidth()/5;
	}
	float y = Y() + getFrame()->getHeight()/3;
	if(0 != velocityX()){
		bullets.shoot( Vector2f(x, y),
			Vector2f(1.5*velocityX(), 0));
	}
	else{
		bullets.shoot( Vector2f(x, y),
			Vector2f( (booRight ? minSpeed:((-1)*minSpeed)), 0));

	}
}

bool Player::collidedWith(const ScaleSprite* s) const{
	return bullets.collidedWith(s);
}

bool Player::getHurt(const ScaleSprite* s) const{
	if(strategy->execute(*this, *s)){
		//setExploded();
		//std::cout<<"get hurt"<<std::endl;
		return true;
	}
	else 
		return false;
}

bool Player::collidedWithSmart(const SmartSprite* s) const{
	if(strategy->execute(*this, *s)){
		//setExploded();
		//std::cout<<"get hurt"<<std::endl;
		return true;
	}
	else 
		return false;
}

void Player::setExploded(){
	//static unsigned count = 0;
	isExploded = true;
	touchable  = false;
	//set location for chunks
  	makeChunks(9); 
}

bool Player::done() {
	if( isExploded && 0 == chunkCount() ){
		isExploded = false;
		touchable = true;
		return true;
	}
	else 
		return false;
}
//reset player after explosion
void Player::reBorn(){
	//if();
	isExploded = false;
}

void Player::makeChunks(unsigned int n){ 
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

void Player::reset(){
	isExploded = false;
	touchable = true;
	string name = Drawable::getName();
	setPosition(Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")));
}
