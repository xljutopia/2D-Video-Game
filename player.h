#ifndef PLAYER__H
#define PLAYER__H
#include <string>
#include <vector>
#include <list>
#include "drawable.h"
#include "bullets.h"
#include "chunk.h"
#include "smartSprite.h"

class Player : public Drawable {
public:
  Player(const std::string&);
  Player(const Player&);
  virtual ~Player();
  virtual void draw() const;
  virtual void draw(double angle) const;

  virtual void update(Uint32 ticks);
  virtual const Frame* getFrame() const { 
    return frames[currentFrame]; 
  }
  void left();
  void right();
  void up();
  void down();
  void stop();
  bool collidedWith(const ScaleSprite*) const;
  bool collidedWithSmart(const SmartSprite*) const;
  bool getHurt(const ScaleSprite*) const;
  void makeChunks(unsigned);
  void shoot();
  void setExploded();
  unsigned chunkCount() const { return chunks.size();}
  unsigned freeCount() const { return freeList.size();}
  bool done();
  CollisionStrategy* getStrategy(const string&);
  void reBorn();
  bool isTouchable()const {return touchable;}
  void setTouchable(bool b) {touchable = b;}
  unsigned bulletsCount() const {return bullets.bulletCount();}
  unsigned freesCount() const {return bullets.freeCount();}
  virtual void reset();

protected:
  const std::vector<Frame *> frames;
  std::vector<Frame *> chunkFrames;
  std::list<Chunk> chunks;
  std::list<Chunk> freeList;
  CollisionStrategy* strategy;

  int worldWidth;
  int worldHeight;

  bool keyPressedX;
  bool keyPressedY;

  Vector2f initialVelocity;
  const float slowDown;

  unsigned currentFrame;
  unsigned numberOfFrames;
  unsigned frameInterval;
  float timeSinceLastFrame;
  int frameWidth;
  int frameHeight;
  bool booRight;  //direction
  bool isExploded;
  bool touchable;
  std::string bulletName;
  Bullets bullets;
  float minSpeed;

  void advanceFrame(Uint32 ticks);
private:
  Player& operator=(const Player&);
};
#endif

