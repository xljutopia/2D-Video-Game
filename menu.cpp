#include <sstream>
#include "menu.h"

Menu::~Menu() {
  SDL_FreeSurface(surfaceOff);
  SDL_FreeSurface(surfaceOn);
}

Menu::Menu() : 
  io( IOManager::getInstance() ),
  words(), 
  parser( "xmlSpec/menu.xml" ),
  position( parser.getXmlInt("words/x"), parser.getXmlInt("words/y") ),
  space(),
  surfaceOff(io.loadAndSet( parser.getXmlStr("clickoff/file"), true)),
  surfaceOn(io.loadAndSet( parser.getXmlStr("clickon/file"), true)),
  clicks(),
  nextIcon(0),
  click(OFF) 
{ 
  unsigned int n = parser.getXmlInt("words/number");
  std::stringstream strm;
  for ( unsigned i = 0; i < n; ++i ) {
    strm << "words/i" << i;
    words.push_back( parser.getXmlStr(strm.str()) );
    strm.clear(); // clear error flags
    strm.str(std::string()); // clear contents
  }

  space[0] = parser.getXmlInt("words/space/x");
  space[1] = parser.getXmlInt("words/space/y");

  Frame clickoff(surfaceOff, 
              parser.getXmlInt("clickoff/width"), 
              parser.getXmlInt("clickoff/height"), 
              parser.getXmlInt("clickoff/src/x"), 
              parser.getXmlInt("clickoff/src/y")); 
  clicks.push_back( clickoff );

  Frame clickon(surfaceOn, 
              parser.getXmlInt("clickon/width"), 
              parser.getXmlInt("clickon/height"), 
              parser.getXmlInt("clickon/src/x"), 
              parser.getXmlInt("clickon/src/y")); 
  clicks.push_back( clickon );

}

void Menu::draw() const {
  int x = position[0];
  int y = position[1];
  for (unsigned i = 0; i < words.size(); ++i) {
    io.printMessageAt(words[i], x, y);
    y += space[1];
  }
  y = position[1] + nextIcon*space[1];
  // To draw the lamp, I'm using the Frame::draw method
  // that does NOT adjust for the viewport position.
  // Thus, we'll draw the lamp relative to (0, 0):
  clicks[click].draw(0, 0, position[0]-space[0], y);
}

const string& Menu::getIconClicked() const { 
  return words[nextIcon]; 
} 

