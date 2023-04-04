#include <stdlib.h>
#include "GameUtil.h"
#include "BulletLifeUp.h"
#include "BoundingShape.h"

BulletLifeUp::BulletLifeUp(void) : GameObject("BulletLifeUp")
{
	// Stationary Object
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 0.0;
	mVelocity.y = 0.0;
	mVelocity.z = 0.0;
}

BulletLifeUp::~BulletLifeUp(void)
{
}

/*
void BulletLifeUp::Render(void)
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
*/

bool BulletLifeUp::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Bullet")) return false; // Player gets Item effects by shooting
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void BulletLifeUp::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}