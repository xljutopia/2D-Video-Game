#ifndef CHUNKS_H
#define CHUNKS_H
#include <iostream>
#include "drawable.h"
#include "gamedata.h"

class Chunk : public Drawable {
public:
  explicit Chunk(
    const Vector2f& pos, const Vector2f vel, 
    const string& name, const Frame* fm) :
    Drawable(name, pos, vel), 
	frame(fm),
	frameWidth(frame->getWidth()),
    frameHeight(frame->getHeight()),
    worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
    worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
    distance(0), 
    maxDistance(Gamedata::getInstance().getXmlInt(name+"/distance")), 
    tooFar(false) 
  { }
  Chunk(const Chunk&);
  Chunk& operator=(const Chunk&);
  virtual ~Chunk() {}

  virtual void draw() const;
  virtual void draw(double angle) const;
  void draw(Uint32 x, Uint32 y, double scale) const;
  void draw(Uint32 x, Uint32 y ) const;
  virtual void update(Uint32 ticks);
  virtual const Frame* getFrame() const { return frame; }
  bool goneTooFar() const { return tooFar; }
  void reset() {
    tooFar = false;
    distance = 0;
  }
private:
  const Frame* frame;
  int frameWidth;
  int frameHeight;
  int worldWidth;
  int worldHeight;
  float distance;
  float maxDistance;
  bool tooFar;
};
#endif
