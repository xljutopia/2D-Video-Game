#ifndef SCALESPRITE__H
#define SCALESPRITE__H
#include <string>
#include <vector>
#include <list>
#include "drawable.h"
#include "chunk.h"

class ScaleSprite : public Drawable {
public:
  ScaleSprite(const std::string& );
  virtual ~ScaleSprite();

  bool operator<(const ScaleSprite& rhs) const{
	return scale < rhs.scale;
  }

  virtual void draw() const;
  virtual void draw(double angle) const;
  void drawZoom( ) ;

  virtual void update(Uint32 ticks);
  virtual const Frame* getFrame() const { 
    return frames[currentFrame]; 
  }
  void makeChunks(unsigned);

  unsigned chunkCount() const { return chunks.size();}
  unsigned freeCount() const { return freeList.size();}
  float getScale() const { return scale; }
  void setExploded();
  bool done(); 
  bool isTouchable()const {return touchable;}
  void setTouchable(bool b) {touchable = b;}
  virtual void reset();
protected:
  float scale;
  const std::vector<Frame *> frames;
  std::vector<Frame *> chunkFrames;
  std::list<Chunk> chunks;
  std::list<Chunk> freeList;
  int worldWidth;
  int worldHeight;

  unsigned currentFrame;
  unsigned numberOfFrames;
  unsigned frameInterval;
  float timeSinceLastFrame;
  int frameWidth;
  int frameHeight;
  bool booRight;  //direction
  bool isExploded;
  bool touchable;
  void advanceFrame(Uint32 ticks);
private:
  ScaleSprite(const ScaleSprite&);
  ScaleSprite& operator=(const ScaleSprite& );
};
#endif

