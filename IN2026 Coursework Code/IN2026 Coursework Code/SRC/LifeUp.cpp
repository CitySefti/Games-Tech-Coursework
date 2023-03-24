#include <stdlib.h>
#include "GameUtil.h"
#include "LifeUp.h"
#include "BoundingShape.h"

LifeUp::LifeUp(void) : GameObject("LifeUp")
{
	// Stationary Object
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 0.0;
	mVelocity.y = 0.0;
	mVelocity.z = 0.0;
}

LifeUp::~LifeUp(void)
{
}


void LifeUp::Render(void) 
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.6, 0.6, 0.6);
	glVertex3f(-7, -7, 0.0);
	glVertex3f(-10, 0, 0.0);
	glVertex3f(-7, 7, 0.0);
	glVertex3f(0, 10, 0.0);
	glVertex3f(7, 7, 0.0);
	glVertex3f(10, 0, 0.0);
	glVertex3f(7, -7, 0.0);
	glVertex3f(0, -10, 0.0);
	glEnd();
	glEnable(GL_LIGHTING);
}

bool LifeUp::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Bullet")) return false; // Player gets Item effects by shooting
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void LifeUp::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}
