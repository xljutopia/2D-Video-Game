#ifndef SMARTSPRIT_H
#define SMARTSPRIT_H

#include <iostream>
#include "sprite.h"
#include "ioManager.h"

class SmartSprite : public Sprite {
public:
  SmartSprite(const std::string&,  const Sprite& p);
  void draw() const;
  void update(Uint32 ticks);
  void goLeft();
  void goRight();
  void goUp();
  void goDown();

  static void incrSafeDistance() {++safeDistance; }
  static void decrSafeDistance() { --safeDistance; }
  static float getSafeDistance() { return safeDistance; }

  virtual void reset();
private:	
  enum MODE {NORMAL, EVADE};
  IOManager& io;	
  const Sprite & enemy;
  static float safeDistance;
  MODE currentMode;
  SmartSprite(const SmartSprite &);
  SmartSprite& operator=(const SmartSprite &rhs);
};

#endif
