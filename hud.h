#ifndef HUD__H
#define HUD__H

#include "vector2f.h"
#include "ioManager.h"
#include "aaline.h"
#include "ioManager.h"
#include "clock.h"
#include <string>

class Hud {
public:
  Hud();
  void draw() const;
  void draw(unsigned, unsigned , unsigned, unsigned ,unsigned) const;
  void draw(const std::string&, unsigned) const;
  void setPosition(const Vector2f&);
  void setShape(const Vector2f&);
private:
  SDL_Surface* screen;
  Vector2f start;
  int width;
  int height;
  const IOManager& io;
  const Uint32 RED;
  Clock& clock;
  Hud(const Hud&);
  Hud& operator=(const Hud&);
};
#endif
