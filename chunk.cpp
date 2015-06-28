#include <iostream>
#include <cmath>

#include "chunk.h"
Chunk::Chunk(const Chunk& c):
	Drawable(c),
	frame(c.frame),
	frameWidth(c.getFrame()->getWidth()),
    frameHeight(c.getFrame()->getHeight()),
    worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
    worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
	distance(c.distance),
	maxDistance(c.maxDistance),
	tooFar(c.tooFar)
{}

Chunk& Chunk::operator=(const Chunk& rhs){
	if(this == &rhs) return *this;
	Drawable::operator=(rhs);
    frame = rhs.frame;  // shallow copy; Manager may reuse it
    frameWidth = rhs.frameWidth;
    frameHeight = rhs.frameHeight;
    worldWidth = rhs.worldWidth;
    worldHeight = rhs.worldHeight;
    distance = rhs.distance;
	maxDistance = rhs.maxDistance;
	tooFar = rhs.tooFar;
    return *this;
}

void Chunk::update(Uint32 ticks) { 
  float yincr = velocityY() * static_cast<float>(ticks) * 0.001;
  Y( Y() + yincr );
  float xincr = velocityX() * static_cast<float>(ticks) * 0.001;
  X( X() + xincr );
  distance += ( hypot(xincr, yincr) );
  if (distance > maxDistance) tooFar = true;
}

void Chunk::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frame->draw(x, y); 
}

void Chunk::draw(double angle) const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frame->draw(x, y, angle); 
}

void Chunk::draw(Uint32 x, Uint32 y, double scale) const { 
  frame->draw(x, y, scale); 
}
void Chunk::draw(Uint32 x, Uint32 y) const { 
  frame->draw(x, y); 
}

