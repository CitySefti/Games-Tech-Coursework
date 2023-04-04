#ifndef __ALIENSHIP_H__
#define __ALIENSHIP_H__

#include "GameObject.h"

class Alienship : public GameObject
{
public:
	Alienship(void);
	~Alienship(void);

	//void Render(void);
	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};

#endif

