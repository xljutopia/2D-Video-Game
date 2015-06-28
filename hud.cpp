#include "hud.h"
#include "gamedata.h"

Hud::Hud() :
  screen(IOManager::getInstance().getScreen()),
  start(Vector2f(Gamedata::getInstance().getXmlInt("hud/srcX"), Gamedata::getInstance().getXmlInt("hud/srcY"))),
  width(Gamedata::getInstance().getXmlInt("hud/Width")),
  height(Gamedata::getInstance().getXmlInt("hud/Height")),
  io( IOManager::getInstance()),
  RED( SDL_MapRGB(screen->format, 0xff, 0x00, 0x00) ),
  clock( Clock::getInstance() )
  {}

void Hud::draw() const {
  Draw_AALine(screen, start[0], start[1]+height/2, 
                      start[0]+width, start[1]+height/2, 
                      height, 0xff, 0xff, 0xff, 0xff/2);
  // Two Horizontal lines
  Draw_AALine(screen, start[0], start[1], 
                      start[0]+width, start[1], 
                      1.0, RED);
  Draw_AALine(screen, start[0], start[1]+height, 
                      start[0]+width, start[1]+height, 
                      1.0, RED);
  // Two Vertical lines
  Draw_AALine(screen, start[0], start[1], 
                      start[0], start[1]+height, 
                      2.0, RED);
  Draw_AALine(screen, start[0]+width, start[1], 
                      start[0]+width, start[1]+height, 
                      2.0, RED);

   io.printMessageValueAt("Seconds: ", clock.getSeconds(), start[0]+2, start[1]+5);
   io.printMessageValueAt("fps: ", clock.getAvgFps(), start[0]+5, start[1]+20);
   io.printMessageAt("Press F1 to toggle Hud", start[0]+5, start[1]+35);
   io.printMessageAt("Press key AWSD to move", start[0]+5, start[1]+50);
   io.printMessageAt("Press key F2 to toggle god mode", start[0]+5, start[1]+65);
   io.printMessageAt("Press key R to reset", start[0]+5, start[1]+80);
   io.printMessageAt("Press key Space to shoot", start[0]+5, start[1]+95);
   io.printMessageAt("Press key Q to quit", start[0]+5, start[1]+110);

}

void Hud::draw(unsigned actives, unsigned frees, unsigned orbs, unsigned explosions, unsigned collisions) const {
   io.printMessageValueAt("Active bullets: ", actives, start[0]+5, start[1]+125);
   io.printMessageValueAt("Bullet pool: ", frees, start[0]+5, start[1]+140);
   io.printMessageValueAt("Monsters: ", orbs, start[0]+5, start[1]+155);
   io.printMessageValueAt("Explosions: ", explosions, start[0]+5, start[1]+170);
	io.printMessageValueAt("Collisions: ", collisions, start[0]+5, start[1]+185);

}

void Hud::setPosition(const Vector2f& pos){
	start=pos;
}

void Hud::setShape(const Vector2f& shape){
	width=shape[0];
	height=shape[1];
}

void Hud::draw(const std::string& s, unsigned score) const{
  Draw_AALine(screen, start[0], start[1]+height/2, 
                      start[0]+width, start[1]+height/2, 
                      height, 0xff, 0xff, 0xff, 0xff/2);
  // Two Horizontal lines
  Draw_AALine(screen, start[0], start[1], 
                      start[0]+width, start[1], 
                      1.0, RED);
  Draw_AALine(screen, start[0], start[1]+height, 
                      start[0]+width, start[1]+height, 
                      1.0, RED);
  // Two Vertical lines
  Draw_AALine(screen, start[0], start[1], 
                      start[0], start[1]+height, 
                      2.0, RED);
  Draw_AALine(screen, start[0]+width, start[1], 
                      start[0]+width, start[1]+height, 
                      2.0, RED);

   io.printMessageAt(s, start[0]+2, start[1]+5);
   io.printMessageValueAt("your score is: ", score, start[0]+5, start[1]+25);
   io.printMessageAt("press key R to restart.", start[0]+2, start[1]+50);



}
