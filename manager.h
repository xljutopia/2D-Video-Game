#include <vector>
#include <list>
#include <SDL.h>
#include "ioManager.h"
#include "clock.h"
#include "world.h"
#include "viewport.h"
#include "frameFactory.h"
#include "SDL/SDL_rotozoom.h"
#include "scalesprite.h"
#include "smartSprite.h"
#include "hud.h"
#include "sound.h"
#include "health.h"

class Manager {
public:
  Manager ();
  ~Manager ();
  void play();
  void pause() { clock.pause(); }
  void unpause() { clock.unpause(); }
  void setNumberOfSprites(int);

  void makeOrbs();
  void reset();
private:
  const bool env;
  const IOManager& io;
  Clock& clock;
  SDLSound sound;
  Health bar;
  SDL_Surface * const screen;

  World back;
  World front;
  
  Hud hud;
  Hud finalResult;
  Viewport& viewport;

  std::vector<Drawable*> players;
  std::list<ScaleSprite*> fallings;

  int currentSprite;

  bool makeVideo;
  int frameCount;
  const std::string username;
  const std::string title;
  const int frameMax;

  void draw() const;
  void update();

  Manager(const Manager&);
  Manager& operator=(const Manager&);
  void makeFrame();
};
