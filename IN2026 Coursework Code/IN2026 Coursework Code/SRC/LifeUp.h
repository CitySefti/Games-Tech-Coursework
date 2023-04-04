#ifndef __LIFEUP_H__
#define __LIFEUP_H__

#include "GameObject.h"

class LifeUp : public GameObject
{
public:
	LifeUp(void);
	~LifeUp(void);

	//void Render(void);
	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
	
};

#endif
