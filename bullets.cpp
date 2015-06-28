#include "bullets.h"
#include<cmath>
#include<iostream>
#include "ioManager.h"
#include "gamedata.h"

CollisionStrategy* getStrategy(const string& name){
	std::string sName = Gamedata::getInstance().getXmlStr(name+"/Strategy");
	if(sName == "midpoint") return new MidPointCollisionStrategy;

	if(sName == "rectangular") return new RectangularCollisionStrategy;

	if(sName == "perpixel") return new PerPixelCollisionStrategy;
	throw std::string("No strategy in getStrategy");
}


Bullets::~Bullets(){
	SDL_FreeSurface( bulletSurface );
	delete bulletFrame;
	delete strategy;
}

Bullets::Bullets(const std::string& s):
	name(s),
	strategy( getStrategy(name)),
	bulletSurface(IOManager::getInstance().loadAndSet(Gamedata::getInstance().getXmlStr(name+"/file"), true)),
	bulletFrame( new Frame(bulletSurface, bulletSurface->w, bulletSurface->h, 0, 0)),
	frameInterval(Gamedata::getInstance().getXmlInt(name+"/BulletInterval")),
	timeSinceLastFrame(0),
	bulletList(),
	freeList()
{}

Bullets::Bullets(const Bullets& b):
	name(b.name),
	strategy(b.strategy),
	bulletSurface(b.bulletSurface),
	bulletFrame(b.bulletFrame),
	frameInterval(b.frameInterval),
	timeSinceLastFrame(b.timeSinceLastFrame),
	bulletList(b.bulletList),
	freeList(b.freeList)
{ }

bool Bullets::collidedWith(const ScaleSprite* obj) const{
	std::list<Bullet>::iterator ptr= bulletList.begin();
	while(ptr != bulletList.end()){
		if(strategy->execute(*ptr, *obj)){
			freeList.push_back(*ptr);
			ptr = bulletList.erase(ptr);
			//std::cout<<"hit a monster."<<std::endl;
			//explosion happened
			//obj->setExploded();
			return true;
		}
		++ptr;
	}
	return false;
}

void Bullets::shoot(const Vector2f& pos, const Vector2f& vel){
	if(timeSinceLastFrame > frameInterval){
		if(freeList.empty()){
			Bullet b(pos, vel, name, bulletFrame);
			bulletList.push_back(b);
		}
		else{
			Bullet b = freeList.front();
			freeList.pop_front();
			b.reset();
			b.setVelocity(vel);
			b.setPosition(pos);
			bulletList.push_back(b);
		}
		timeSinceLastFrame = 0;
	}
}

void Bullets::draw() const{
	std::list<Bullet>::const_iterator ptr = bulletList.begin();
	while(ptr != bulletList.end()){
		ptr->draw();
		++ptr;
	}
}

void Bullets::update(Uint32 ticks){
	timeSinceLastFrame += ticks;
	std::list<Bullet>::iterator ptr= bulletList.begin();
	while(ptr != bulletList.end()){
		ptr->update(ticks);
		if(ptr->goneTooFar()){
			freeList.push_back(*ptr);
			ptr = bulletList.erase(ptr);
		}
		else
			++ptr;
	}
}


