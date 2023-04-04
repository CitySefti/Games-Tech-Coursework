#ifndef __BULLETLIFEUP_H__
#define __BULLETLIFEUP_H__

#include "GameObject.h"

class BulletLifeUp : public GameObject
{
public:
	BulletLifeUp(void);
	~BulletLifeUp(void);

	//void Render(void);
	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};

#endif
