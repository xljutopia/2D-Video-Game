#ifndef BULLETS_H
#define BULLETS_H

#include<list>
#include"bullet.h"
#include"scalesprite.h"
#include"collisionStrategy.h"

class Bullets{
public:
	Bullets(const std::string&);
	Bullets(const Bullets&);
	~Bullets();
	void draw() const;
	void update(Uint32 ticks);
	void shoot(const Vector2f& pos, const Vector2f& vel);

	unsigned bulletCount() const { return bulletList.size(); }
	unsigned freeCount() const { return freeList.size(); }
	bool shooting() const { return bulletList.empty(); }
	bool collidedWith(const ScaleSprite* obj) const;

private:
	std::string name;
	CollisionStrategy* strategy;
	SDL_Surface* bulletSurface;
	Frame* bulletFrame;
	float frameInterval;
	float timeSinceLastFrame;
	mutable std::list<Bullet> bulletList;
	mutable std::list<Bullet> freeList;

	Bullets& operator=(const Bullets&);
};
#endif

